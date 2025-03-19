#include "tonalcpp/pitch_interval.h"
#include "tonalcpp/pitch.h"
#include <regex>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iostream>

// Forward declaration of functions in pitch.cpp
namespace tonalcpp {
    Pitch pitchFromCoordinates(const PitchCoordinates& coord);
}

namespace tonalcpp {

// Helper function to create repeated strings (like fillStr in TypeScript)
// Renamed to avoid conflict with pitch_note.cpp
std::string intervalFillStr(const std::string& s, int n) {
    std::string result;
    result.reserve(s.length() * n);
    for (int i = 0; i < n; i++) {
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
    : Pitch(), empty(true), name(""), num(NAN), q(""), 
      type(IntervalType::Unknown), simple(NAN), semitones(NAN), 
      chroma(NAN), oct(NAN), coord{} {}

// Full constructor implementation
Interval::Interval(bool emp, const IntervalName& n, int nu, const Quality& qu, IntervalType ty,
                  int s, int a, int d, int sim, int semi, int ch, int o,
                  const IntervalCoordinates& co)
    : Pitch(s, a, o, d == 1 ? Direction::Ascending : Direction::Descending, n), 
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
    Pitch p(step, alt, oct, dirValue == 1 ? Direction::Ascending : Direction::Descending);
    PitchCoordinates pitch_coords = coordinates(p);
    IntervalCoordinates coord = {pitch_coords[0], 
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
Quality altToQ(IntervalType type, int alt) {
    if (alt == 0) {
        return (type == IntervalType::Majorable) ? "M" : "P";
    } else if (alt == -1 && type == IntervalType::Majorable) {
        return "m";
    } else if (alt > 0) {
        return intervalFillStr("A", alt);
    } else {
        return intervalFillStr("d", type == IntervalType::Perfectable ? std::abs(alt) : std::abs(alt + 1));
    }
}

// Convert pitch to interval name
// Renamed to avoid conflict with pitch_note.cpp
std::string intervalPitchName(const Pitch& props) {
    int step = props.step;
    int alt = props.alt;
    int oct = props.oct.value_or(0);
    Direction dir = props.dir.value_or(Direction::Ascending);
    
    if (!props.dir.has_value()) {
        return "";
    }
    
    int calcNum = step + 1 + 7 * oct;
    // Edge case for descending pitch class unison (as in TypeScript)
    int num = (calcNum == 0) ? step + 1 : calcNum;
    
    std::string d = (dir == Direction::Descending) ? "-" : "";
    IntervalType type = (TYPES[step] == 'M') ? IntervalType::Majorable : IntervalType::Perfectable;
    std::string name = d + std::to_string(num) + altToQ(type, alt);
    
    return name;
}

// Convert coordinates to interval with force descending option
Interval coordToInterval(const PitchCoordinates& coord, bool forceDescending) {
    // Handle different coordinate types
    if (coord.size() < 2) {
        // PitchClassCoordinates - need at least [fifths, octaves]
        return NoInterval;
    }
    
    int f = coord[0];
    int o = (coord.size() > 1) ? coord[1] : 0;
    
    // Check if descending
    bool isDescending = f * 7 + o * 12 < 0;
    
    // Create interval coordinates
    IntervalCoordinates ivl;
    if (forceDescending || isDescending) {
        ivl = {-f, -o, -1};
    } else {
        ivl = {f, o, 1};
    }
    
    // Convert coordinates to pitch and then to interval
    // The coordinates are already in the correct format for pitchFromCoordinates
    std::vector<int> coords = {ivl[0], ivl[1], ivl[2]};
    Pitch p = pitchFromCoordinates(coords);
    return interval(intervalPitchName(p));
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

} // namespace tonalcpp