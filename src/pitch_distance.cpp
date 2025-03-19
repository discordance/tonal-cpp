#include "tonalcpp/pitch_distance.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace tonalcpp {

/**
 * Helper function to convert interval coordinates to an Interval object
 * Matches the TypeScript coordToInterval function
 */
Interval coordToIntervalObj(const PitchCoordinates& coord, bool forceDescending = false) {
    if (coord.empty()) {
        return NoInterval;
    }
    
    int fifths = coord[0];
    int octaves = coord.size() > 1 ? coord[1] : 0;
    
    // Check if the interval is descending
    bool isDescending = (fifths * 7 + octaves * 12) < 0;
    
    // Create the interval coordinates
    IntervalCoordinates intervalCoord;
    if (forceDescending || isDescending) {
        intervalCoord = {-fifths, -octaves, -1};
    } else {
        intervalCoord = {fifths, octaves, 1};
    }
    
    // Convert to an interval
    // Convert array to vector for coordToInterval
    std::vector<int> coords = {intervalCoord[0], intervalCoord[1], intervalCoord[2]};
    return coordToInterval(coords, forceDescending);
}

/**
 * Transpose a note by an interval
 * Precisely follows the TypeScript implementation
 */
std::string transpose(const std::string& noteName, const std::string& intervalName) {
    // Parse note and interval
    Note n = note(noteName);
    Interval i = interval(intervalName);
    
    // Check for invalid inputs
    if (n.empty || i.name == "") {
        return "";
    }
    
    // Get note and interval coordinates
    PitchCoordinates noteCoord = n.coord;
    IntervalCoordinates intervalCoord = i.coord;
    
    // Create the transposed coordinates
    PitchCoordinates result;
    if (noteCoord.size() == 1) {
        // For pitch classes (no octave)
        result = {noteCoord[0] + intervalCoord[0]};
    } else {
        // For notes with octaves
        result = {
            noteCoord[0] + intervalCoord[0], 
            noteCoord[1] + intervalCoord[1]
        };
    }
    
    // Convert back to a note and return its name
    return coordToNote(result).name;
}

/**
 * Calculate the interval between two notes
 * Precisely follows the TypeScript implementation
 */
std::string distance(const std::string& fromNoteName, const std::string& toNoteName) {
    // Parse notes
    Note fromNote = note(fromNoteName);
    Note toNote = note(toNoteName);
    
    // Check for invalid inputs
    if (fromNote.empty || toNote.empty) {
        return "";
    }
    
    // Get note coordinates
    PitchCoordinates fromCoord = fromNote.coord;
    PitchCoordinates toCoord = toNote.coord;
    
    // Calculate difference in fifths
    int fifths = toCoord[0] - fromCoord[0];
    
    // Calculate difference in octaves
    int octaves;
    if (fromCoord.size() == 2 && toCoord.size() == 2) {
        // For notes with octaves
        octaves = toCoord[1] - fromCoord[1];
    } else {
        // For pitch classes, estimate octave difference based on fifths
        octaves = -static_cast<int>(std::floor((fifths * 7) / 12.0));
    }
    
    // Check if we need to force a descending interval
    // This handles special cases where the height is the same but the steps differ
    bool forceDescending = 
        (toNote.height == fromNote.height) &&
        (fromNote.midi.has_value()) &&  // TypeScript: to.midi !== null
        (fromNote.oct == toNote.oct) &&
        (fromNote.step > toNote.step);
    
    // Convert to an interval and return its name
    return coordToIntervalObj({fifths, octaves}, forceDescending).name;
}

/**
 * Calculate the interval between two notes (object version)
 */
std::string distance(const Note& fromNote, const Note& toNote) {
    return distance(fromNote.name, toNote.name);
}

/**
 * Transpose a note by an interval (object version)
 */
std::string transpose(const Note& n, const Interval& i) {
    return transpose(n.name, i.name);
}

/**
 * Create a transposer based on a tonic and intervals
 * Returns a list of notes transposed from the tonic
 */
std::vector<std::string> tonicIntervalsTransposer(
    const std::vector<std::string>& intervals, 
    const std::string& tonic) {
    
    std::vector<std::string> result;
    for (const auto& ivl : intervals) {
        result.push_back(transpose(tonic, ivl));
    }
    
    return result;
}

} // namespace tonalcpp