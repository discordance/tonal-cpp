#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "tonalcpp/chord_type.h"
#include "tonalcpp/pitch_note.h"
#include "tonalcpp/pcset.h"

namespace tonalcpp {
namespace chord_detect {

/**
 * Represents a detected chord with a weight to determine ranking
 */
struct FoundChord {
    double weight;
    std::string name;
};

/**
 * Options for chord detection
 */
struct DetectOptions {
    bool assumePerfectFifth = false;
};

/**
 * Function to create a mapping from chroma number to note name
 * 
 * @param notes List of note names
 * @return Function that maps from chroma to note name
 */
std::function<std::string(int)> namedSet(const std::vector<std::string>& notes);

/**
 * Detect chords from a list of notes
 * 
 * @param source List of note names
 * @param options Detection options (optional)
 * @return List of detected chord names, sorted by likelihood
 */
std::vector<std::string> detect(
    const std::vector<std::string>& source,
    const DetectOptions& options = DetectOptions()
);

/**
 * Internal function to find matching chords
 * 
 * @param notes List of note names
 * @param weight Base weight for matches
 * @param options Detection options
 * @return List of found chords with weights
 */
std::vector<FoundChord> findMatches(
    const std::vector<std::string>& notes,
    double weight,
    const DetectOptions& options
);

/**
 * Tests if a chord type has any third, perfect fifth, and any seventh
 * 
 * @param chordType The chord type to test
 * @return true if the chord type has these characteristics
 */
bool hasAnyThirdAndPerfectFifthAndAnySeventh(const chord_type::ChordType& chordType);

/**
 * Adds perfect fifth to a chroma if it doesn't have any fifth
 * 
 * @param chroma The chroma string
 * @return The modified chroma string with perfect fifth
 */
std::string withPerfectFifth(const std::string& chroma);

} // namespace chord_detect
} // namespace tonalcpp