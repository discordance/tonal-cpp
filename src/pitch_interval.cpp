#include "tonalcpp/pitch_interval.h"
#include "tonalcpp/pitch.h"
#include <regex>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <limits>

// Forward declaration of functions in pitch.cpp
namespace tonalcpp {
namespace pitch {
    Pitch pitchFromCoordinates(const PitchCoordinates& coord);
}
}

namespace tonalcpp {
namespace pitch_interval {

// Helper function to create repeated strings
// In TypeScript: fillStr = (s, n) => Array(Math.abs(n) + 1).join(s)
// Renamed to avoid conflict with pitch_note.cpp
std::string intervalFillStr(const std::string& s, int n) {
    std::string result;
    // JavaScript's Array(n+1).join(s) creates n copies of s
    // For example, Array(3+1).join("A") creates 3 copies: "AAA"
    int absN = std::abs(n);
    
    if (absN <= 0) {
        return "";
    }
    
    result.reserve(s.length() * absN);
    for (int i = 0; i < absN; i++) {
        result += s;
    }

    return result;
}

// Cache for interval objects to improve performance
static std::unordered_map<std::string, Interval> intervalCache;

// Arrays and constants - match TypeScript implementation
const std::vector<int> SIZES = {0, 2, 4, 5, 7, 9, 11};
const std::string TYPES = "PMMPPMM";

// Define NoInterval - exactly like in TypeScript
const Interval NoInterval = Interval();

// Default constructor implementation
Interval::Interval()
    : pitch::Pitch(), empty(true), name(""), num(std::numeric_limits<double>::quiet_NaN()), q(""), 
      type(IntervalType::Unknown), simple(std::numeric_limits<double>::quiet_NaN()), 
      semitones(std::numeric_limits<double>::quiet_NaN()), 
      chroma(std::numeric_limits<double>::quiet_NaN()), 
      oct(std::numeric_limits<double>::quiet_NaN()), coord{} {}

// Full constructor implementation
Interval::Interval(bool emp, const IntervalName& n, int nu, const Quality& qu, IntervalType ty,
                  int s, int a, int d, int sim, int semi, int ch, int o,
                  const pitch::IntervalCoordinates& co)
    : pitch::Pitch(s, a, o, d == 1 ? pitch::Direction::Ascending : pitch::Direction::Descending, n), 
      empty(emp), name(n), num(nu), q(qu), type(ty),
      simple(sim), semitones(semi), chroma(ch), oct(o), coord(co) {}

// Tokenize interval string exactly as in TypeScript
std::pair<std::string, std::string> tokenizeInterval(const std::string& str) {
    if (str.empty()) {
        return {"", ""};
    }

    // Regular expressions based on TypeScript implementation
    static const std::regex tonal_regex("([-+]?\\d+)(d{1,4}|m|M|P|A{1,4})");
    static const std::regex shorthand_regex("(AA|A|P|M|m|d|dd)([-+]?\\d+)");
    static const std::regex combined_regex("^([-+]?\\d+)(d{1,4}|m|M|P|A{1,4})|(AA|A|P|M|m|d|dd)([-+]?\\d+)$");

    std::smatch match;
    if (std::regex_match(str, match, combined_regex)) {
        if (match[1].matched) {
            // Format: [number][quality]
            return {match[1].str(), match[2].str()};
        } else {
            // Format: [quality][number] (shorthand)
            return {match[4].str(), match[3].str()};
        }
    }
    
    return {"", ""};
}

// Internal parse function - follows TypeScript implementation
// Renamed to avoid conflict with pitch_note.cpp
Interval parseInterval(const std::string& str) {
    auto [numStr, qStr] = tokenizeInterval(str);
    if (numStr.empty()) {
        return NoInterval;
    }

    int num = std::stoi(numStr);
    Quality q = qStr;
    int step = (std::abs(num) - 1) % 7;
    char typeChar = TYPES[step];
    
    // Validate for majorable intervals with perfect quality
    if (typeChar == 'M' && q == "P") {
        return NoInterval;
    }
    
    IntervalType type = (typeChar == 'M') ? IntervalType::Majorable : IntervalType::Perfectable;

    std::string name = numStr + q;
    int dirValue = (num < 0) ? -1 : 1;
    int simple = (num == 8 || num == -8) ? num : dirValue * (step + 1);
    int alt = qToAlt(type, q);
    int oct = std::floor((std::abs(num) - 1) / 7.0);
    int semitones = dirValue * (SIZES[step] + alt + 12 * oct);
    int chroma = ((dirValue * (SIZES[step] + alt)) % 12 + 12) % 12;
    
    // Create coordinates
    pitch::Pitch p(step, alt, oct, dirValue == 1 ? pitch::Direction::Ascending : pitch::Direction::Descending);
    pitch::PitchCoordinates pitch_coords = pitch::coordinates(p);
    pitch::IntervalCoordinates coord = {pitch_coords[0], 
                               pitch_coords.size() > 1 ? pitch_coords[1] : 0,
                               dirValue};
    
    return Interval(false, name, num, q, type, step, alt, dirValue, 
                    simple, semitones, chroma, oct, coord);
}

// Convert quality string to alteration
int qToAlt(IntervalType type, const Quality& q) {
    // Exactly follow TypeScript implementation
    if ((q == "M" && type == IntervalType::Majorable) ||
        (q == "P" && type == IntervalType::Perfectable)) {
        return 0;
    } else if (q == "m" && type == IntervalType::Majorable) {
        return -1;
    } else if (q.find_first_not_of('A') == std::string::npos) { // Only contains 'A'
        return q.length();
    } else if (q.find_first_not_of('d') == std::string::npos) { // Only contains 'd'
        return -1 * (type == IntervalType::Perfectable ? q.length() : q.length() + 1);
    }
    return 0;
}

// Convert alteration to quality
// This function implements the exact same logic as in TypeScript
Quality altToQ(IntervalType type, int alt) {
    if (alt == 0) {
        // Perfect or Major
        return (type == IntervalType::Majorable) ? "M" : "P";
    } else if (alt == -1 && type == IntervalType::Majorable) {
        // Minor (only for majorable intervals)
        return "m";
    } else if (alt > 0) {
        // Augmented (A, AA, AAA...)
        return intervalFillStr("A", alt);
    } else {
        // Diminished (d, dd, ddd...)
        // In TypeScript:
        // For perfectable intervals:
        //   5P -> 5d would be alt = -1, so we return "d"
        //   5P -> 5dd would be alt = -2, so we return "dd"
        // For majorable intervals:
        //   3M -> 3d would be alt = -2, so we return "d"
        //   3M -> 3dd would be alt = -3, so we return "dd"
        int diminished = (type == IntervalType::Perfectable) ? alt : alt + 1;
        return intervalFillStr("d", diminished);
    }
}

// Convert pitch to interval name (equivalent to pitchName in TypeScript)
// Renamed to avoid conflict with pitch_note.cpp
std::string intervalPitchName(const pitch::Pitch& props) {
    int step = props.step;
    int alt = props.alt;
    int oct = props.oct.value_or(0);
    
    // Check if direction is available - matching TypeScript implementation
    if (!props.dir.has_value()) {
        return "";
    }
    
    // Get direction value as an int to match TypeScript implementation
    int dir = props.dir.value() == pitch::Direction::Descending ? -1 : 1;
    
    // Calculate the interval number (always positive)
    int calcNum = step + 1 + 7 * oct;
    
    // Edge case for descending pitch class unison (as in TypeScript)
    // In TypeScript: const num = calcNum === 0 ? step + 1 : calcNum;
    int num = (calcNum == 0) ? step + 1 : calcNum;

    // Direction prefix
    // In TypeScript: const d = dir < 0 ? "-" : "";
    std::string d = dir < 0 ? "-" : "";
    
    // Get the interval type from the step
    IntervalType type = (TYPES[step] == 'M') ? IntervalType::Majorable : IntervalType::Perfectable;
    
    // Build the interval name: direction + number + quality
    std::string name = d + std::to_string(num) + altToQ(type, alt);
    
    return name;
}

// Convert coordinates to interval with force descending option
Interval coordToInterval(const pitch::PitchCoordinates& coord, bool forceDescending) {
    // Check for minimum required coordinates
    if (coord.empty()) {
        return NoInterval;
    }
    
    // Extract the fifths and octaves from coordinates - match TypeScript implementation
    int f = coord[0];
    int o = (coord.size() > 1) ? coord[1] : 0;

    // Check if descending - the calculation is f * 7 + o * 12 < 0
    // This is exactly the same as in TypeScript
    bool isDescending = f * 7 + o * 12 < 0;
    
    // Create interval coordinates - match the TypeScript implementation exactly
    pitch::PitchCoordinates ivl;
    if (forceDescending || isDescending) {
        // For descending intervals, we negate the fifths and octaves
        ivl = {-f, -o, -1};
    } else {
        ivl = {f, o, 1};
    }
    
    // Create a pitch from the coordinates
    pitch::Pitch p = pitch::pitchFromCoordinates(ivl);
    
    // Get the interval name and create the interval
    auto name = intervalPitchName(p);
    
    return interval(name);
}

// Main interval function with caching
Interval interval(const std::string& src, bool useCache) {
    if (src.empty()) {
        return NoInterval;
    }
    
    // Check cache first
    if (useCache && intervalCache.find(src) != intervalCache.end()) {
        return intervalCache[src];
    }
    
    // Parse string - using renamed function
    auto result = parseInterval(src);
    
    // Cache result if valid
    if (useCache && !result.empty) {
        intervalCache[src] = result;
    }
    
    return result;
}

// Default interval function (uses cache)
Interval interval(const std::string& src) {

    return interval(src, true);
}

} // namespace pitch_interval
} // namespace tonalcpp