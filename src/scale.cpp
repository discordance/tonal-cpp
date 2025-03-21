#include "tonalcpp/scale.h"

#include <algorithm>
#include <set>
#include <functional>

#include "tonalcpp/chord_type.h"
#include "tonalcpp/collection.h"
#include "tonalcpp/note.h"
#include "tonalcpp/pcset.h"
#include "tonalcpp/pitch_distance.h"
#include "tonalcpp/pitch_note.h"

namespace tonalcpp {
namespace scale {

// Initialize the empty scale
const Scale NoScale = []() {
    Scale s;
    // Pcset fields
    s.name = "";
    s.empty = true;
    s.setNum = 0;
    s.chroma = "";
    s.normalized = "";
    s.intervals = {};
    // ScaleType fields
    s.aliases = {};
    // Scale fields
    s.tonic = std::nullopt;
    s.type = "";
    s.notes = {};
    return s;
}();

ScaleNameTokens tokenize(const ScaleName& name) {
    if (name.empty()) {
        return std::make_tuple("", "");
    }

    const size_t i = name.find(' ');
    const pitch_note::Note noteObj = pitch_note::note(name.substr(0, i));
    
    if (noteObj.empty) {
        const pitch_note::Note n = pitch_note::note(name);
        if (n.empty) {
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), 
                          [](unsigned char c){ return std::tolower(c); });
            return std::make_tuple("", lowerName);
        } else {
            return std::make_tuple(n.name, "");
        }
    }

    std::string type;
    if (i != std::string::npos && i + 1 < name.size()) {
        type = name.substr(i + 1);
        std::transform(type.begin(), type.end(), type.begin(), 
                      [](unsigned char c){ return std::tolower(c); });
    }
    
    return std::make_tuple(noteObj.name, type);
}

std::vector<std::string> names() {
    return scale_type::names();
}

Scale get(const ScaleName& src) {
    return get(tokenize(src));
}

Scale get(const ScaleNameTokens& tokens) {
    const std::string& tokenTonic = std::get<0>(tokens);
    const pitch_note::Note tonicNote = pitch_note::note(tokenTonic);
    const scale_type::ScaleType st = scale_type::get(std::get<1>(tokens));
    
    if (st.empty) {
        return NoScale;
    }

    Scale scale;
    // Copy ScaleType fields
    scale.name = st.name;
    scale.empty = st.empty;
    scale.setNum = st.setNum;
    scale.chroma = st.chroma;
    scale.normalized = st.normalized;
    scale.intervals = st.intervals;
    scale.aliases = st.aliases;
    
    // Add Scale-specific fields
    scale.type = st.name;
    
    // Only use valid note names as tonic
    if (tonicNote.empty) {
        scale.tonic = std::nullopt;
    } else {
        scale.tonic = tonicNote.name;
    }
    
    // Generate notes only if we have a valid tonic
    if (scale.tonic) {
        for (const auto& interval : st.intervals) {
            scale.notes.push_back(pitch_distance::transpose(*scale.tonic, interval));
        }
        scale.name = *scale.tonic + " " + st.name;
    }
    
    return scale;
}

std::vector<std::string> detect(
    const std::vector<std::string>& notes,
    const std::string& inputTonic,
    const std::string& match) {
    
    const std::string notesChroma = pcset::chroma(notes);
    
    // Determine tonic from input or first note
    std::string tonicName = inputTonic;
    if (tonicName.empty() && !notes.empty()) {
        tonicName = notes[0];
    }
    
    const pitch_note::Note tonicNote = pitch_note::note(tonicName);
    // In TypeScript, they check if tonicChroma === undefined
    // In C++, for the NoNote, the chroma value is valid (0), but the note is marked as empty
    if (tonicNote.empty) {
        return {};
    }
    
    const int tonicChroma = tonicNote.chroma;
    
    // Create a copy of the chroma
    std::string chromaCopy = notesChroma;
    // Ensure the tonic is included
    chromaCopy[tonicChroma] = '1';
    
    // Rotate the chroma to start from the tonic
    auto pitchClasses = std::vector<char>(chromaCopy.begin(), chromaCopy.end());
    auto rotated = collection::rotate(tonicChroma, pitchClasses);
    std::string scaleChroma(rotated.begin(), rotated.end());
    
    // Find exact matches
    std::vector<std::string> results;
    auto allScales = scale_type::all();
    
    // Check for exact match first
    auto exactMatch = std::find_if(allScales.begin(), allScales.end(),
        [&scaleChroma](const scale_type::ScaleType& scaleType) {
            return scaleType.chroma == scaleChroma;
        });
    
    if (exactMatch != allScales.end()) {
        results.push_back(tonicNote.name + " " + exactMatch->name);
    }
    
    // Return just exact matches if requested
    if (match == "exact") {
        return results;
    }
    
    // Add extended scales
    auto extendedScales = extended(scaleChroma);
    for (const auto& scaleName : extendedScales) {
        results.push_back(tonicNote.name + " " + scaleName);
    }
    
    return results;
}

std::vector<std::string> scaleChords(const std::string& name) {
    const Scale s = get(name);
    if (s.empty) {
        return {};
    }
    
    auto isSubsetFunc = [&s](const std::string& chordChroma) {
        return pcset::isSubsetOf(s.chroma, chordChroma);
    };
    
    std::vector<std::string> result;
    auto allChordTypes = chord_type::all();
    
    for (const auto& chord : allChordTypes) {
        if (isSubsetFunc(chord.chroma) && !chord.aliases.empty()) {
            result.push_back(chord.aliases[0]);
        }
    }
    
    return result;
}

std::vector<std::string> extended(const std::string& name) {
    std::string chromaStr;
    if (pcset::isChroma(name)) {
        chromaStr = name;
    } else {
        const Scale s = get(name);
        if (s.empty) {
            return {};
        }
        chromaStr = s.chroma;
    }
    
    auto isSupersetFunc = [&chromaStr](const std::string& scaleChroma) {
        return pcset::isSupersetOf(chromaStr, scaleChroma);
    };
    
    std::vector<std::string> result;
    auto allScales = scale_type::all();
    
    for (const auto& scale : allScales) {
        if (isSupersetFunc(scale.chroma)) {
            result.push_back(scale.name);
        }
    }
    
    return result;
}

std::vector<std::string> reduced(const std::string& name) {
    const Scale s = get(name);
    if (s.empty) {
        return {};
    }
    
    auto isSubsetFunc = [&s](const std::string& scaleChroma) {
        return pcset::isSubsetOf(s.chroma, scaleChroma);
    };
    
    std::vector<std::string> result;
    auto allScales = scale_type::all();
    
    for (const auto& scale : allScales) {
        if (isSubsetFunc(scale.chroma)) {
            result.push_back(scale.name);
        }
    }
    
    return result;
}

std::vector<std::string> scaleNotes(const std::vector<std::string>& notes) {
    std::vector<std::string> pcset;
    
    // Extract pitch classes from notes
    for (const auto& noteName : notes) {
        const pitch_note::Note n = pitch_note::note(noteName);
        if (!n.pc.empty()) {
            pcset.push_back(n.pc);
        }
    }
    
    if (pcset.empty()) {
        return {};
    }
    
    const std::string tonic = pcset[0];
    
    // Get sorted unique note names
    auto sortedUnique = note::sortedUniqNames(pcset);
    
    // Find the index of the tonic
    auto tonicIt = std::find(sortedUnique.begin(), sortedUnique.end(), tonic);
    if (tonicIt == sortedUnique.end()) {
        return sortedUnique; // Should not happen, but return original if tonic not found
    }
    
    // Calculate rotation
    size_t tonicIdx = std::distance(sortedUnique.begin(), tonicIt);
    auto rotated = collection::rotate(tonicIdx, sortedUnique);
    
    return rotated;
}

std::vector<std::tuple<std::string, std::string>> modeNames(const std::string& name) {
    const Scale s = get(name);
    if (s.empty) {
        return {};
    }
    
    const std::vector<std::string>& tonics = s.tonic.has_value() ? s.notes : s.intervals;
    const auto modesChromas = pcset::modes(s.chroma);
    
    std::vector<std::tuple<std::string, std::string>> result;
    
    for (size_t i = 0; i < modesChromas.size(); ++i) {
        const std::string& chroma = modesChromas[i];
        const Scale modeScale = get(chroma);
        
        if (!modeScale.name.empty() && i < tonics.size()) {
            result.push_back(std::make_tuple(tonics[i], modeScale.name));
        }
    }
    
    // Filter out empty tonics
    result.erase(
        std::remove_if(result.begin(), result.end(), 
            [](const auto& tuple) { return std::get<0>(tuple).empty(); }), 
        result.end());
    
    return result;
}

// Helper function to get note name from a scale
namespace {
    std::function<std::optional<std::string>(const std::string&)> getNoteNameOf(
        const std::string& scale) {
        
        std::vector<std::string> scaleNoteNames;
        std::vector<int> chromas;
        
        // Get scale notes
        if (scale.find(' ') != std::string::npos || scale.find(',') != std::string::npos) {
            auto s = get(scale);
            if (!s.empty && !s.notes.empty()) {
                scaleNoteNames = s.notes;
            }
        } else {
            // Assume it's a comma-separated string
            std::vector<std::string> notes;
            std::string current;
            for (char c : scale) {
                if (c == ',') {
                    if (!current.empty()) {
                        notes.push_back(current);
                        current.clear();
                    }
                } else {
                    current += c;
                }
            }
            if (!current.empty()) {
                notes.push_back(current);
            }
            
            scaleNoteNames = notes.empty() ? std::vector<std::string>{scale} : notes;
        }
        
        // Handle case where scale has no notes
        if (scaleNoteNames.empty()) {
            return [](const std::string&) -> std::optional<std::string> {
                return std::nullopt; // Return nothing for all inputs since scale has no notes
            };
        }

        // Extract chromas - follow TypeScript implementation:
        // const chromas = names.map((name) => note(name).chroma);
        for (const auto& name : scaleNoteNames) {
            auto n = pitch_note::note(name);
            chromas.push_back(n.chroma);
        }
        
        // Return function that finds note in scale
        return [scaleNoteNames, chromas](const std::string& noteOrMidi) -> std::optional<std::string> {
            pitch_note::Note currNote;
            
            // Handle MIDI number or note name
            if (std::all_of(noteOrMidi.begin(), noteOrMidi.end(), ::isdigit)) {
                try {
                    int midiNum = std::stoi(noteOrMidi);
                    currNote = pitch_note::note(note::fromMidi(midiNum));
                } catch (...) {
                    return std::nullopt;
                }
            } else {
                currNote = pitch_note::note(noteOrMidi);
            }
            
            // In TypeScript: if (height === undefined) return undefined;
            if (currNote.empty) {
                return std::nullopt;
            }
            
            // In TypeScript: const chroma = height % 12;
            const int chroma = currNote.height % 12;
            
            // Find position of this chroma in the scale
            auto it = std::find(chromas.begin(), chromas.end(), chroma);
            if (it == chromas.end()) {
                return std::nullopt;
            }
            
            size_t position = std::distance(chromas.begin(), it);
            return note::enharmonic(currNote.name, scaleNoteNames[position]);
        };
    }
    
    std::function<std::optional<std::string>(const std::string&)> getNoteNameOf(
        const std::vector<std::string>& scaleNotes) {
        
        auto actualNotes = scaleNotes;
        std::vector<int> chromas;
        
        // Handle case where scale has no notes
        if (actualNotes.empty()) {
            return [](const std::string&) -> std::optional<std::string> {
                return std::nullopt; // Return nothing for all inputs since scale has no notes
            };
        }

        // Extract chromas - follow TypeScript implementation:
        // const chromas = names.map((name) => note(name).chroma);
        for (const auto& name : actualNotes) {
            auto n = pitch_note::note(name);
            chromas.push_back(n.chroma);
        }
        
        // Return function that finds note in scale
        return [actualNotes, chromas](const std::string& noteOrMidi) -> std::optional<std::string> {
            pitch_note::Note currNote;
            
            // Handle MIDI number or note name
            if (std::all_of(noteOrMidi.begin(), noteOrMidi.end(), ::isdigit)) {
                try {
                    int midiNum = std::stoi(noteOrMidi);
                    currNote = pitch_note::note(note::fromMidi(midiNum));
                } catch (...) {
                    return std::nullopt;
                }
            } else {
                currNote = pitch_note::note(noteOrMidi);
            }
            
            // In TypeScript: if (height === undefined) return undefined;
            if (currNote.empty) {
                return std::nullopt;
            }
            
            // In TypeScript: const chroma = height % 12;
            const int chroma = currNote.height % 12;
            
            // Find position of this chroma in the scale
            auto it = std::find(chromas.begin(), chromas.end(), chroma);
            if (it == chromas.end()) {
                return std::nullopt;
            }
            
            size_t position = std::distance(chromas.begin(), it);
            return note::enharmonic(currNote.name, actualNotes[position]);
        };
    }
}

std::vector<std::string> rangeOf(
    const std::string& scale, 
    const std::string& fromNote, 
    const std::string& toNote) {
    
    // Get the note name finder function
    auto getName = getNoteNameOf(scale);
    
    // If the scale has no tonic (like "pentatonic"), get(scale).notes will be empty
    // and the test should return an empty vector
    auto s = get(scale);
    if (s.notes.empty() || !s.tonic.has_value()) {
        return {};
    }
    
    const pitch_note::Note from = pitch_note::note(fromNote);
    const pitch_note::Note to = pitch_note::note(toNote);
    
    // In TypeScript, they check if height === undefined
    // In C++, check if notes are empty or have no height property
    if (from.empty || to.empty) {
        return {};
    }
    
    // Use collection.range to generate the range like in TypeScript
    auto heightRange = collection::range(from.height, to.height);
    
    // Map each height to a note name and filter out nullopt values
    std::vector<std::string> result;
    for (int h : heightRange) {
        auto noteName = getName(std::to_string(h));
        if (noteName.has_value()) {
            result.push_back(*noteName);
        }
    }
    
    return result;
}

std::vector<std::string> rangeOf(
    const std::vector<std::string>& scale, 
    const std::string& fromNote, 
    const std::string& toNote) {
    
    // Get the note name finder function
    auto getName = getNoteNameOf(scale);
    
    // If the scale is empty, we should return an empty vector
    if (scale.empty()) {
        return {};
    }
    
    const pitch_note::Note from = pitch_note::note(fromNote);
    const pitch_note::Note to = pitch_note::note(toNote);
    
    // In TypeScript, they check if height === undefined
    // In C++, check if notes are empty or have no height property
    if (from.empty || to.empty) {
        return {};
    }
    
    // Use collection.range to generate the range like in TypeScript
    auto heightRange = collection::range(from.height, to.height);
    
    // Map each height to a note name and filter out nullopt values
    std::vector<std::string> result;
    for (int h : heightRange) {
        auto noteName = getName(std::to_string(h));
        if (noteName.has_value()) {
            result.push_back(*noteName);
        }
    }
    
    return result;
}

std::string degrees(const std::string& scaleName, int degree) {
    if (degree == 0) {
        return "";
    }
    
    const Scale s = get(scaleName);
    if (s.empty || s.intervals.empty() || !s.tonic.has_value()) {
        return "";
    }
    
    const auto& intervals = s.intervals;
    const std::string& tonic = *s.tonic;
    
    // Use the same logic as steps but adjust the index
    int index;
    if (degree > 0) {
        // Positive degrees (1-based index)
        index = (degree - 1) % static_cast<int>(intervals.size());
    } else {
        // Negative degrees (work backwards from tonic)
        index = static_cast<int>(intervals.size()) - ((-degree) % static_cast<int>(intervals.size()));
        if (index == static_cast<int>(intervals.size())) {
            index = 0;
        }
    }
    
    // Calculate octave shift
    int octaveShift = 0;
    if (degree > 0) {
        octaveShift = (degree - 1) / static_cast<int>(intervals.size());
    } else {
        octaveShift = -((-degree) / static_cast<int>(intervals.size()));
        if ((-degree) % static_cast<int>(intervals.size()) != 0) {
            octaveShift -= 1;
        }
    }
    
    // Get the note at this degree
    std::string note = pitch_distance::transpose(tonic, intervals[index]);
    
    // Add octave shift if the tonic has an octave
    if (tonic.find_first_of("0123456789") != std::string::npos && octaveShift != 0) {
        pitch_note::Note noteObj = pitch_note::note(note);
        if (noteObj.midi.has_value()) {
            note = note::fromMidi(noteObj.midi.value() + (octaveShift * 12));
        }
    }
    
    return note;
}

std::string degrees(const ScaleNameTokens& scaleName, int degree) {
    if (degree == 0) {
        return "";
    }
    
    const Scale s = get(scaleName);
    if (s.empty || s.intervals.empty() || !s.tonic.has_value()) {
        return "";
    }
    
    const auto& intervals = s.intervals;
    const std::string& tonic = *s.tonic;
    
    // Use the same logic as steps but adjust the index
    int index;
    if (degree > 0) {
        // Positive degrees (1-based index)
        index = (degree - 1) % static_cast<int>(intervals.size());
    } else {
        // Negative degrees (work backwards from tonic)
        index = static_cast<int>(intervals.size()) - ((-degree) % static_cast<int>(intervals.size()));
        if (index == static_cast<int>(intervals.size())) {
            index = 0;
        }
    }
    
    // Calculate octave shift
    int octaveShift = 0;
    if (degree > 0) {
        octaveShift = (degree - 1) / static_cast<int>(intervals.size());
    } else {
        octaveShift = -((-degree) / static_cast<int>(intervals.size()));
        if ((-degree) % static_cast<int>(intervals.size()) != 0) {
            octaveShift -= 1;
        }
    }
    
    // Get the note at this degree
    std::string note = pitch_distance::transpose(tonic, intervals[index]);
    
    // Add octave shift if the tonic has an octave
    if (tonic.find_first_of("0123456789") != std::string::npos && octaveShift != 0) {
        pitch_note::Note noteObj = pitch_note::note(note);
        if (noteObj.midi.has_value()) {
            note = note::fromMidi(noteObj.midi.value() + (octaveShift * 12));
        }
    }
    
    return note;
}

std::string steps(const std::string& scaleName, int step) {
    const Scale s = get(scaleName);
    if (s.empty || s.intervals.empty() || !s.tonic.has_value()) {
        return "";
    }
    
    const auto& intervals = s.intervals;
    const std::string& tonic = *s.tonic;
    
    // Calculate the actual index in the intervals array
    const int size = static_cast<int>(intervals.size());
    int index = ((step % size) + size) % size;
    
    // Calculate octave shift
    int octaveShift = 0;
    if (step >= 0) {
        octaveShift = step / size;
    } else {
        octaveShift = -1 - ((-step - 1) / size);
    }
    
    // Get the note at this step
    std::string note = pitch_distance::transpose(tonic, intervals[index]);
    
    // Add octave shift if the tonic has an octave
    if (tonic.find_first_of("0123456789") != std::string::npos && octaveShift != 0) {
        pitch_note::Note noteObj = pitch_note::note(note);
        if (noteObj.midi.has_value()) {
            note = note::fromMidi(noteObj.midi.value() + (octaveShift * 12));
        }
    }
    
    return note;
}

std::string steps(const ScaleNameTokens& scaleName, int step) {
    const Scale s = get(scaleName);
    if (s.empty || s.intervals.empty() || !s.tonic.has_value()) {
        return "";
    }
    
    const auto& intervals = s.intervals;
    const std::string& tonic = *s.tonic;
    
    // Calculate the actual index in the intervals array
    const int size = static_cast<int>(intervals.size());
    int index = ((step % size) + size) % size;
    
    // Calculate octave shift
    int octaveShift = 0;
    if (step >= 0) {
        octaveShift = step / size;
    } else {
        octaveShift = -1 - ((-step - 1) / size);
    }
    
    // Get the note at this step
    std::string note = pitch_distance::transpose(tonic, intervals[index]);
    
    // Add octave shift if the tonic has an octave
    if (tonic.find_first_of("0123456789") != std::string::npos && octaveShift != 0) {
        pitch_note::Note noteObj = pitch_note::note(note);
        if (noteObj.midi.has_value()) {
            note = note::fromMidi(noteObj.midi.value() + (octaveShift * 12));
        }
    }
    
    return note;
}

} // namespace scale
} // namespace tonalcpp