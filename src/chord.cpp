#include "tonalcpp/chord.h"
#include "tonalcpp/chord_detect.h"
#include "tonalcpp/pcset.h"
#include "tonalcpp/pitch_distance.h"
// #include "tonalcpp/pitch_interval.h"
#include "tonalcpp/interval.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <string>
#include <vector>

namespace tonalcpp {
namespace chord {

// Define the NoChord constant
const Chord NoChord = {
    // ChordType part (which includes Pcset)
    {
        // Pcset part
        {
            "", // name (from Pcset)
            true, // empty (from Pcset)
            0, // setNum (from Pcset)
            "000000000000", // chroma (from Pcset)
            "000000000000", // normalized (from Pcset)
            {}  // intervals (from Pcset)
        },
        "", // name (from ChordType)
        chord_type::ChordQuality::Unknown, // quality
        {} // aliases
    },
    
    // Chord-specific fields
    std::nullopt, // tonic
    "", // type
    "", // root
    "", // bass
    std::nullopt,  // rootDegree
    "", // symbol
    {}  // notes
};

ChordNameTokens tokenize(const std::string& name) {
    auto [letter, acc, oct, type] = pitch_note::tokenizeNote(name);
    if (letter.empty()) {
        return tokenizeBass("", type);
    } else if (letter == "A" && type == "ug") {
        return tokenizeBass("", "aug");
    } else {
        return tokenizeBass(letter + acc, oct + type);
    }
}

ChordNameTokens tokenizeBass(const std::string& note, const std::string& chord) {
    // Split by slash for bass note
    size_t slashPos = chord.find('/');
    if (slashPos == std::string::npos) {
        return {note, chord, ""};
    }
    
    std::string chordPart = chord.substr(0, slashPos);
    std::string bassPart = chord.substr(slashPos + 1);
    
    auto [bassLetter, bassAcc, bassOct, bassType] = pitch_note::tokenizeNote(bassPart);
    
    // Only a pitch class is accepted as bass note
    if (!bassLetter.empty() && bassOct.empty() && bassType.empty()) {
        return {note, chordPart, bassLetter + bassAcc};
    } else {
        return {note, chord, ""};
    }
}

Chord get(const std::string& src) {
    if (src.empty()) {
        return NoChord;
    }
    
    auto tokens = tokenize(src);
    Chord result = getChord(tokens[1], tokens[0], tokens[2]);
    
    // If we couldn't find a chord, try using the whole name as the type
    if (result.empty) {
        return getChord(src);
    }
    
    return result;
}

Chord get(const std::vector<std::string>& tokens) {
    if (tokens.empty()) {
        return NoChord;
    }
    
    std::string type = tokens.size() > 1 ? tokens[1] : "";
    std::string tonic = !tokens.empty() ? tokens[0] : "";
    std::string bass = tokens.size() > 2 ? tokens[2] : "";
    
    return getChord(type, tonic, bass);
}

Chord getChord(const std::string& typeName, 
               const std::string& optionalTonic, 
               const std::string& optionalBass) {
    chord_type::ChordType type = chord_type::getChordType(typeName);
    pitch_note::Note tonic = pitch_note::note(optionalTonic);
    pitch_note::Note bass = pitch_note::note(optionalBass);
    
    // If any inputs are invalid, return the NoChord
    if (type.empty || (!optionalTonic.empty() && tonic.empty) || 
        (!optionalBass.empty() && bass.empty)) {
        return NoChord;
    }
    
    // Calculate bass interval and check if it's part of the chord
    std::string bassInterval = pitch_distance::distance(tonic.pc, bass.pc);
    
    // Find the bass note position in the chord
    int bassIndex = -1;
    for (size_t i = 0; i < type.intervals.size(); i++) {
        if (type.intervals[i] == bassInterval) {
            bassIndex = static_cast<int>(i);
            break;
        }
    }
    
    bool hasRoot = bassIndex >= 0;
    pitch_note::Note root = hasRoot ? bass : pitch_note::note("");
    std::optional<int> rootDegree = hasRoot ? std::make_optional(bassIndex + 1) : std::nullopt;
    bool hasBass = !bass.pc.empty() && bass.pc != tonic.pc;
    
    // Make a copy of the intervals that we can modify
    std::vector<std::string> intervals = type.intervals;
    
    if (hasRoot) {
        // Rotate intervals according to root degree
        for (int i = 1; i < rootDegree.value_or(0); i++) {
            if (!intervals.empty()) {
                // Get first interval
                std::string firstInterval = intervals[0];
                
                // Extract number and quality
                char num = firstInterval[0];
                char quality = firstInterval.size() > 1 ? firstInterval[1] : '\0';
                
                // Convert to integer, add 7, and convert back to string
                int newNum = (num - '0') + 7;
                
                // Construct new interval and add to end
                std::string newInterval = std::to_string(newNum) + quality;
                intervals.push_back(newInterval);
                
                // Remove the first interval
                intervals.erase(intervals.begin());
            }
        }
    } else if (hasBass) {
        // Add bass interval at the beginning if it's not part of the chord
        std::string ivl = interval::subtract(pitch_distance::distance(tonic.pc, bass.pc), "8P");
        if (!ivl.empty()) {
            intervals.insert(intervals.begin(), ivl);
        }
    }
    
    // Generate notes if tonic is present
    std::vector<std::string> chordNotes;
    if (!tonic.empty) {
        for (const auto& interval : intervals) {
            chordNotes.push_back(pitch_distance::transpose(tonic.pc, interval));
        }
    }
    
    // Generate chord name and symbol
    std::string preferredAlias = !type.aliases.empty() && std::find(type.aliases.begin(), type.aliases.end(), typeName) != type.aliases.end() 
        ? typeName 
        : (!type.aliases.empty() ? type.aliases[0] : "");
    
    // Build chord symbol
    std::string symbol;
    if (tonic.empty) {
        symbol = preferredAlias; // When no tonic, just use the chord type alias
    } else {
        symbol = tonic.pc + preferredAlias;
    }
    
    if (hasRoot && rootDegree.value_or(0) > 1) {
        symbol += "/" + root.pc;
    } else if (hasBass) {
        symbol += "/" + bass.pc;
    }
    
    // Build full chord name
    std::string name = (optionalTonic.empty() ? "" : tonic.pc + " ") + type.name;
    if (hasRoot && rootDegree.value_or(0) > 1) {
        name += " over " + root.pc;
    } else if (hasBass) {
        name += " over " + bass.pc;
    }
    
    // Construct and return the chord object
    Chord chord;
    
    // Copy ChordType fields
    chord.name = name;
    chord.empty = false;
    chord.setNum = type.setNum;
    chord.chroma = type.chroma;
    chord.normalized = type.normalized;
    chord.intervals = intervals;
    chord.quality = type.quality;
    chord.aliases = type.aliases;
    
    // Set Chord-specific fields
    chord.tonic = tonic.empty ? std::nullopt : std::make_optional(tonic.pc);
    chord.type = type.name;
    chord.root = root.pc;
    chord.bass = hasBass ? bass.pc : "";
    chord.rootDegree = rootDegree;
    chord.symbol = symbol;
    chord.notes = chordNotes;
    
    return chord;
}

std::string transpose(const std::string& chordName, const std::string& interval) {
    auto tokens = tokenize(chordName);
    if (tokens[0].empty()) {
        return chordName;
    }
    
    std::string transposedTonic = pitch_distance::transpose(tokens[0], interval);
    std::string transposedBass = tokens[2].empty() ? "" : pitch_distance::transpose(tokens[2], interval);
    
    std::string slash = transposedBass.empty() ? "" : "/" + transposedBass;
    return transposedTonic + tokens[1] + slash;
}

std::vector<std::string> chordScales(const std::string& name) {
    Chord s = get(name);
    
    // We need the scale_type module which hasn't been ported yet
    // This would use isSupersetOf to check if the chord fits in each scale
    // For now we return an empty vector since the scale_type module 
    // isn't available in C++ yet
    return {};
}

std::vector<std::string> extended(const std::string& chordName) {
    Chord s = get(chordName);
    
    std::vector<std::string> result;
    
    // Get all chord types and filter for supersets
    auto allChordTypes = chord_type::all();
    for (const auto& chordType : allChordTypes) {
        // Check if chordType is a superset of s
        if (pcset::isSupersetOf(s.chroma, chordType.chroma) && s.tonic.has_value()) {
            result.push_back(*s.tonic + (!chordType.aliases.empty() ? chordType.aliases[0] : ""));
        }
    }
    
    return result;
}

std::vector<std::string> reduced(const std::string& chordName) {
    Chord s = get(chordName);
    
    std::vector<std::string> result;
    
    // Get all chord types and filter for subsets
    auto allChordTypes = chord_type::all();
    for (const auto& chordType : allChordTypes) {
        // Check if chordType is a subset of s
        if (pcset::isSubsetOf(s.chroma, chordType.chroma) && s.tonic.has_value()) {
            result.push_back(*s.tonic + (!chordType.aliases.empty() ? chordType.aliases[0] : ""));
        }
    }
    
    return result;
}

std::vector<std::string> notes(const std::string& chordName, const std::string& tonic) {
    Chord chord = get(chordName);
    
    // Use provided tonic or the chord's tonic if available
    std::string noteToUse;
    if (!tonic.empty()) {
        noteToUse = tonic;
    } else if (chord.tonic.has_value()) {
        noteToUse = *chord.tonic;
    } else {
        return {}; // No tonic available
    }
    
    if (chord.empty) {
        return {};
    }
    
    std::vector<std::string> result;
    for (const auto& ivl : chord.intervals) {
        result.push_back(pitch_distance::transpose(noteToUse, ivl));
    }
    
    return result;
}

std::vector<std::string> notes(const std::vector<std::string>& tokens, const std::string& tonic) {
    return notes(tokens.empty() ? "" : tokens[0], tonic);
}

std::string degreeToNote(const std::string& chordName, int degree, const std::string& tonic) {
    if (degree == 0) return "";
    
    Chord chord = get(chordName);
    
    // Use provided tonic or the chord's tonic if available
    std::string noteToUse;
    if (!tonic.empty()) {
        noteToUse = tonic;
    } else if (chord.tonic.has_value()) {
        noteToUse = *chord.tonic;
    } else {
        return ""; // No tonic available
    }
    
    if (chord.empty) {
        return "";
    }
    
    // Convert degree to 0-based index
    int index = degree > 0 ? degree - 1 : degree;
    
    // Get the length of the interval array for modulo operations
    int len = static_cast<int>(chord.intervals.size());
    if (len == 0) return "";
    
    // Calculate the octave and index within intervals array
    int octaves = index / len;
    
    // For negative indices, we need special handling to get the correct octave
    int normalizedIndex;
    if (index < 0) {
        // This gives the correct normalized index for negative indices
        normalizedIndex = ((index % len) + len) % len;
        // If the normalized index is not 0, we need to adjust octaves
        if (normalizedIndex != 0) {
            octaves -= 1;
        }
    } else {
        normalizedIndex = index % len;
    }
    
    // First get the note from transposing by octaves
    std::string root = pitch_distance::transpose(noteToUse, std::vector<int>{0, octaves});
    
    // Then transpose by the interval
    return pitch_distance::transpose(root, chord.intervals[normalizedIndex]);
}

std::string degreeToNote(const std::vector<std::string>& tokens, int degree, const std::string& tonic) {
    return degreeToNote(tokens.empty() ? "" : tokens[0], degree, tonic);
}

std::string stepToNote(const std::string& chordName, int step, const std::string& tonic) {
    Chord chord = get(chordName);
    
    // Use provided tonic or the chord's tonic if available
    std::string noteToUse;
    if (!tonic.empty()) {
        noteToUse = tonic;
    } else if (chord.tonic.has_value()) {
        noteToUse = *chord.tonic;
    } else {
        return ""; // No tonic available
    }
    
    if (chord.empty) {
        return "";
    }
    
    // Get the length of the interval array for modulo operations
    int len = static_cast<int>(chord.intervals.size());
    if (len == 0) return "";
    
    // Calculate the octave and index within intervals array
    int octaves = step / len;
    
    // For negative steps, we need special handling to get the correct octave
    int normalizedIndex;
    if (step < 0) {
        // This gives the correct normalized index for negative steps
        normalizedIndex = ((step % len) + len) % len;
        // If the normalized index is not 0, we need to adjust octaves
        if (normalizedIndex != 0) {
            octaves -= 1;
        }
    } else {
        normalizedIndex = step % len;
    }
    
    // First get the note from transposing by octaves
    std::string root = pitch_distance::transpose(noteToUse, std::vector<int>{0, octaves});
    
    // Then transpose by the interval
    return pitch_distance::transpose(root, chord.intervals[normalizedIndex]);
}

std::string stepToNote(const std::vector<std::string>& tokens, int step, const std::string& tonic) {
    return stepToNote(tokens.empty() ? "" : tokens[0], step, tonic);
}

} // namespace chord
} // namespace tonalcpp