#include "tonalcpp/chord_detect.h"
#include <algorithm>
#include <bitset>
#include <functional>
#include <unordered_map>

namespace tonalcpp {
namespace chord_detect {

// Bitmask constants for bit operations on chroma numbers
namespace BITMASK {
    // 3m 000100000000 = 256
    // 3M 000010000000 = 128
    constexpr int anyThirds = 384;        // 256 + 128
    // 5P 000000010000 = 16
    constexpr int perfectFifth = 16;
    // 5d 000000100000 = 32
    // 5A 000000001000 = 8
    constexpr int nonPerfectFifths = 40;  // 32 + 8
    constexpr int anySeventh = 3;         // 2 + 1 (last two bits)
}

// Returns a function that tests if a chroma number has a specific bitmask
std::function<bool(int)> testChromaNumber(int bitmask) {
    return [bitmask](int chromaNumber) -> bool {
        return (chromaNumber & bitmask) != 0;
    };
}

// Test functions for specific chord characteristics
auto hasAnyThird = testChromaNumber(BITMASK::anyThirds);
auto hasPerfectFifth = testChromaNumber(BITMASK::perfectFifth);
auto hasAnySeventh = testChromaNumber(BITMASK::anySeventh);
auto hasNonPerfectFifth = testChromaNumber(BITMASK::nonPerfectFifths);

// Creates a function that maps from chroma to note name
std::function<std::string(int)> namedSet(const std::vector<std::string>& notes) {
    std::unordered_map<int, std::string> pcToName;
    
    for (const auto& n : notes) {
        pitch_note::Note noteObj = pitch_note::note(n);
        if (noteObj.chroma >= 0) {
            // Only set if not already set (first occurrence has priority)
            if (pcToName.find(noteObj.chroma) == pcToName.end()) {
                pcToName[noteObj.chroma] = noteObj.name;
            }
        }
    }
    
    return [pcToName](int chroma) -> std::string {
        auto it = pcToName.find(chroma);
        if (it != pcToName.end()) {
            return it->second;
        }
        return "";
    };
}

// Tests if a chord type has any third, perfect fifth, and any seventh
bool hasAnyThirdAndPerfectFifthAndAnySeventh(const chord_type::ChordType& chordType) {
    int chromaNumber = std::stoi(chordType.chroma, nullptr, 2);
    return hasAnyThird(chromaNumber) && 
           hasPerfectFifth(chromaNumber) && 
           hasAnySeventh(chromaNumber);
}

// Adds perfect fifth to a chroma if it doesn't have any fifth
std::string withPerfectFifth(const std::string& chroma) {
    int chromaNumber = std::stoi(chroma, nullptr, 2);
    if (hasNonPerfectFifth(chromaNumber)) {
        return chroma;
    } else {
        return std::bitset<12>(chromaNumber | BITMASK::perfectFifth).to_string();
    }
}

// Finds matching chords
std::vector<FoundChord> findMatches(
    const std::vector<std::string>& notes,
    double weight,
    const DetectOptions& options
) {
    if (notes.empty()) {
        return {};
    }
    
    std::string tonic = notes[0];
    int tonicChroma = pitch_note::note(tonic).chroma;
    auto noteName = namedSet(notes);
    
    // Get all modes (rotations of the pitch class set)
    auto allModes = pcset::modes(notes, false);
    
    std::vector<FoundChord> found;
    for (size_t index = 0; index < allModes.size(); ++index) {
        std::string mode = allModes[index];
        std::string modeWithPerfectFifth;
        
        if (options.assumePerfectFifth) {
            modeWithPerfectFifth = withPerfectFifth(mode);
        }
        
        // Find all chord types that match this mode
        auto allChordTypes = chord_type::all();
        for (const auto& chordType : allChordTypes) {
            bool matches = false;
            
            if (options.assumePerfectFifth && 
                hasAnyThirdAndPerfectFifthAndAnySeventh(chordType)) {
                matches = (chordType.chroma == modeWithPerfectFifth);
            } else {
                matches = (chordType.chroma == mode);
            }
            
            if (matches && !chordType.aliases.empty()) {
                std::string chordName = chordType.aliases[0];
                std::string baseNote = noteName(static_cast<int>(index));
                bool isInversion = static_cast<int>(index) != tonicChroma;
                
                if (isInversion) {
                    found.push_back({
                        0.5 * weight,
                        baseNote + chordName + "/" + tonic
                    });
                } else {
                    found.push_back({
                        1.0 * weight,
                        baseNote + chordName
                    });
                }
            }
        }
    }
    
    return found;
}

// Detects chords from a list of notes
std::vector<std::string> detect(
    const std::vector<std::string>& source,
    const DetectOptions& options
) {
    // Filter to valid note names with pitch classes
    std::vector<std::string> notes;
    for (const auto& n : source) {
        std::string pc = pitch_note::note(n).pc;
        if (!pc.empty()) {
            notes.push_back(pc);
        }
    }
    
    if (notes.empty()) {
        return {};
    }
    
    // Find chord matches
    std::vector<FoundChord> found = findMatches(notes, 1.0, options);
    
    // Sort by weight (higher weights first)
    std::sort(found.begin(), found.end(), 
              [](const FoundChord& a, const FoundChord& b) {
                  return a.weight > b.weight;
              });
    
    // Extract just the chord names
    std::vector<std::string> result;
    for (const auto& chord : found) {
        if (chord.weight > 0) {
            result.push_back(chord.name);
        }
    }
    
    return result;
}

} // namespace chord_detect
} // namespace tonalcpp
