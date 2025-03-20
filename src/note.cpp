#include "tonalcpp/note.h"
#include <algorithm>
#include <unordered_set>

namespace tonalcpp {
namespace note {

// Constants
const std::vector<std::string> NAMES = {"C", "D", "E", "F", "G", "A", "B"};

// Helper function to get note name
static std::string toName(const pitch_note::Note& n) {
    return n.name;
}

// Helper function to filter valid notes from array
static std::vector<pitch_note::Note> onlyNotes(const std::vector<std::string>& array) {
    std::vector<pitch_note::Note> result;
    for (const auto& item : array) {
        pitch_note::Note n = pitch_note::note(item);
        if (!n.empty) {
            result.push_back(n);
        }
    }
    return result;
}

std::vector<std::string> names(const std::vector<std::string>& array) {
    if (array.empty()) {
        return NAMES;
    } else {
        std::vector<pitch_note::Note> notes = onlyNotes(array);
        std::vector<std::string> result;
        result.reserve(notes.size());
        for (const auto& n : notes) {
            result.push_back(toName(n));
        }
        return result;
    }
}

pitch_note::Note get(const std::string& src) {
    return pitch_note::note(src);
}

std::string name(const std::string& note) {
    return get(note).name;
}

std::string pitchClass(const std::string& note) {
    return get(note).pc;
}

std::string accidentals(const std::string& note) {
    return get(note).acc;
}

std::optional<int> octave(const std::string& note) {
    return get(note).oct;
}

std::optional<int> midi(const std::string& note) {
    return get(note).midi;
}

std::optional<double> freq(const std::string& note) {
    return get(note).freq;
}

int chroma(const std::string& note) {
    return get(note).chroma;
}

std::string fromMidi(int midiNum) {
    return midi::midiToNoteName(midiNum);
}

std::string fromMidiSharps(int midiNum) {
    midi::ToNoteNameOptions options;
    options.sharps = true;
    return midi::midiToNoteName(midiNum, options);
}

std::string fromFreq(double frequency) {
    double midiNumber = midi::freqToMidi(frequency);
    if (std::isnan(midiNumber)) {
        return "";
    }
    return midi::midiToNoteName(static_cast<int>(midiNumber));
}

std::string fromFreqSharps(double frequency) {
    double midiNumber = midi::freqToMidi(frequency);
    if (std::isnan(midiNumber)) {
        return "";
    }
    midi::ToNoteNameOptions options;
    options.sharps = true;
    return midi::midiToNoteName(static_cast<int>(midiNumber), options);
}

std::string distance(const std::string& from, const std::string& to) {
    return pitch_distance::distance(from, to);
}

std::string transpose(const std::string& noteName, const std::string& intervalName) {
    return pitch_distance::transpose(noteName, intervalName);
}

std::function<std::string(const std::string&)> transposeBy(const std::string& interval) {
    return [interval](const std::string& note) {
        return transpose(note, interval);
    };
}

std::function<std::string(const std::string&)> transposeFrom(const std::string& note) {
    return [note](const std::string& interval) {
        return transpose(note, interval);
    };
}

std::string transposeFifths(const std::string& noteName, int fifths) {
    return pitch_distance::transpose(noteName, std::vector<int>{fifths, 0});
}

std::string transposeOctaves(const std::string& noteName, int octaves) {
    return pitch_distance::transpose(noteName, std::vector<int>{0, octaves});
}

bool ascending(const pitch_note::Note& a, const pitch_note::Note& b) {
    // Return true if a should come before b (ascending order)
    return a.height < b.height;
}

bool descending(const pitch_note::Note& a, const pitch_note::Note& b) {
    // Return true if a should come before b (descending order)
    return a.height > b.height;
}

std::vector<std::string> sortedNames(
    const std::vector<std::string>& notes,
    const NoteComparator& comparator) {
    
    std::vector<pitch_note::Note> validNotes = onlyNotes(notes);
    std::sort(validNotes.begin(), validNotes.end(), comparator);
    
    std::vector<std::string> result;
    result.reserve(validNotes.size());
    for (const auto& note : validNotes) {
        result.push_back(toName(note));
    }
    
    return result;
}

std::vector<std::string> sortedUniqNames(const std::vector<std::string>& notes) {
    std::vector<std::string> sorted = sortedNames(notes, ascending);
    std::vector<std::string> result;
    
    if (sorted.empty()) {
        return result;
    }
    
    result.push_back(sorted[0]);
    for (size_t i = 1; i < sorted.size(); ++i) {
        if (sorted[i] != sorted[i-1]) {
            result.push_back(sorted[i]);
        }
    }
    
    return result;
}

std::string simplify(const std::string& noteName) {
    pitch_note::Note note = get(noteName);
    if (note.empty) {
        return "";
    }
    
    midi::ToNoteNameOptions options;
    options.sharps = note.alt > 0;
    options.pitchClass = !note.midi.has_value();
    
    return midi::midiToNoteName(note.midi.value_or(note.chroma), options);
}

std::string enharmonic(const std::string& noteName, const std::string& destName) {
    const pitch_note::Note src = get(noteName);
    if (src.empty) {
        return "";
    }
    
    // Determine destination name
    std::string destPc = destName;
    if (destPc.empty()) {
        midi::ToNoteNameOptions options;
        options.sharps = src.alt < 0;
        options.pitchClass = true;
        destPc = midi::midiToNoteName(src.midi.value_or(src.chroma), options);
    }
    
    const pitch_note::Note dest = get(destPc);
    
    // Ensure destination is valid
    if (dest.empty || dest.chroma != src.chroma) {
        return "";
    }
    
    // If source has no octave, just return the destination pitch class
    if (!src.oct.has_value()) {
        return dest.pc;
    }
    
    // Detect octave overflow
    int srcChroma = src.chroma - src.alt;
    int destChroma = dest.chroma - dest.alt;
    int destOctOffset = 0;
    
    if (srcChroma > 11 || destChroma < 0) {
        destOctOffset = -1;
    } else if (srcChroma < 0 || destChroma > 11) {
        destOctOffset = 1;
    }
    
    // Calculate the new octave
    int destOct = src.oct.value() + destOctOffset;
    return dest.pc + std::to_string(destOct);
}

} // namespace note
} // namespace tonalcpp