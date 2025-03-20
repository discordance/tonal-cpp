#include "tonalcpp/pitch_distance.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace tonalcpp {
namespace pitch_distance {

/**
 * Helper function to convert interval coordinates to an Interval object
 * Matches the TypeScript coordToInterval function
 */
pitch_interval::Interval coordToIntervalObj(const pitch::PitchCoordinates& coord, bool forceDescending = false) {
    if (coord.empty()) {
        return pitch_interval::NoInterval;
    }
    
    int fifths = coord[0];
    int octaves = coord.size() > 1 ? coord[1] : 0;
    
    // Check if the interval is descending
    bool isDescending = (fifths * 7 + octaves * 12) < 0;
    
    // Create the interval coordinates
    pitch::IntervalCoordinates intervalCoord;
    if (forceDescending || isDescending) {
        intervalCoord = {-fifths, -octaves, -1};
    } else {
        intervalCoord = {fifths, octaves, 1};
    }
    
    // Convert to an interval
    // Convert array to vector for coordToInterval
    std::vector<int> coords = {intervalCoord[0], intervalCoord[1], intervalCoord[2]};
    return pitch_interval::coordToInterval(coords, forceDescending);
}

/**
 * Transpose a note by an interval
 * Precisely follows the TypeScript implementation
 */
std::string transpose(const std::string& noteName, const std::string& intervalName) {
    // Parse note and interval
    pitch_note::Note n = pitch_note::note(noteName);
    pitch_interval::Interval i = pitch_interval::interval(intervalName);
    
    // Check for invalid inputs
    if (n.empty || i.name == "") {
        return "";
    }
    
    // Get note and interval coordinates
    pitch::PitchCoordinates noteCoord = n.coord;
    pitch::IntervalCoordinates intervalCoord = i.coord;
    
    // Create the transposed coordinates
    pitch::PitchCoordinates result;
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

    auto nn = pitch_note::coordToNote(result).name;
    
    // Convert back to a note and return its name
    return nn;
}

/**
 * Calculate the interval between two notes
 * Precisely follows the TypeScript implementation
 */
std::string distance(const std::string& fromNoteName, const std::string& toNoteName) {
    // Parse notes
    pitch_note::Note fromNote = pitch_note::note(fromNoteName);
    pitch_note::Note toNote = pitch_note::note(toNoteName);
    
    // Check for invalid inputs
    if (fromNote.empty || toNote.empty) {
        return "";
    }
    
    // Get note coordinates
    pitch::PitchCoordinates fromCoord = fromNote.coord;
    pitch::PitchCoordinates toCoord = toNote.coord;
    
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
        (toNote.midi.has_value()) &&  // TypeScript: to.midi !== null
        (fromNote.oct == toNote.oct) &&
        (fromNote.step > toNote.step);
    
    // Check for descending intervals (notes with octaves)
    // This is to handle cases where the interval is actually descending
    bool isDescending = false;
    if (fromCoord.size() == 2 && toCoord.size() == 2) {
        if (toNote.height < fromNote.height) {
            isDescending = true;
        }
    }
    
    // Convert to an interval and return its name
    return coordToIntervalObj({fifths, octaves}, forceDescending || isDescending).name;
}

/**
 * Calculate the interval between two notes (object version)
 */
std::string distance(const pitch_note::Note& fromNote, const pitch_note::Note& toNote) {
    return distance(fromNote.name, toNote.name);
}

/**
 * Transpose a note by an interval using coordinates
 * This matches the TypeScript implementation that accepts [fifths, octaves]
 * 
 * The coordinate system uses:
 * - coords[0]: fifths (how many perfect fifth steps to move)
 * - coords[1]: octaves (how many octaves to move)
 */
std::string transpose(const std::string& noteName, const std::vector<int>& coords) {
    // Parse note
    pitch_note::Note n = pitch_note::note(noteName);
    
    // Convert input coordinates to a proper interval coordinate
    pitch::PitchCoordinates intervalCoord;
    
    if (coords.size() >= 2) {
        // We have both fifths and octaves
        intervalCoord = {coords[0], coords[1]};
    } else if (coords.size() == 1) {
        // We only have fifths
        intervalCoord = {coords[0], 0};
    } else {
        // No coordinates provided
        return "";
    }
    
    // Check for invalid inputs
    if (n.empty) {
        return "";
    }
    
    // Get note coordinates
    pitch::PitchCoordinates noteCoord = n.coord;
    
    // Create the transposed coordinates
    pitch::PitchCoordinates result;
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
    return pitch_note::coordToNote(result).name;
}

/**
 * Transpose a note by an interval (object version)
 */
std::string transpose(const pitch_note::Note& n, const pitch_interval::Interval& i) {
    return transpose(n.name, i.name);
}

/**
 * Create a transposer function based on a tonic and a set of intervals
 * This closely matches the TypeScript implementation
 * Takes a normalized index value and returns the corresponding transposed note
 * @param intervals The intervals to use for transposition
 * @param tonic The tonic note (reference point)
 * @return A vector of transposed notes
 */
std::vector<std::string> tonicIntervalsTransposer(
    const std::vector<std::string>& intervals, 
    const std::string& tonic) {
    
    if (tonic.empty()) {
        return {};
    }
    
    int len = static_cast<int>(intervals.size());
    std::vector<std::string> result;
    
    // In TypeScript, this returns a function. In C++, we pre-compute all possible notes
    // In TypeScript, this creates a function that takes a normalized index
    // In C++, we precompute results for normalized indices 0 to len-1
    for (int normalized = 0; normalized < len; normalized++) {
        // Calculate index within the interval array
        int index = normalized % len;
        
        // Calculate octave shift
        int octaves = normalized / len;
        
        // First transpose the tonic by the octave shift using [fifths, octaves] coordinate
        std::vector<int> coords = {0, octaves};
        std::string root = transpose(tonic, coords);
        
        // Then transpose by the interval at the calculated index
        std::string note = transpose(root, intervals[index]);
        result.push_back(note);
    }
    
    return result;
}

} // namespace pitch_distance
} // namespace tonalcpp