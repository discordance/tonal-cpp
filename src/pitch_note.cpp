#include "tonalcpp/pitch_note.h"
#include <cmath>
#include <regex>
#include <map>
#include <string>
#include <algorithm>

namespace tonalcpp {

// Constants
const std::array<int, 7> SEMI = {0, 2, 4, 5, 7, 9, 11};

// Create the NoNote singleton
const Note NoNote = []() {
    Note n;
    n.empty = true;
    n.name = "";
    n.letter = "";
    n.acc = "";
    n.pc = "";
    n.step = std::numeric_limits<int>::quiet_NaN();
    n.alt = std::numeric_limits<int>::quiet_NaN();
    n.chroma = std::numeric_limits<int>::quiet_NaN();
    n.height = std::numeric_limits<int>::quiet_NaN();
    n.coord = {};
    n.midi = std::nullopt;
    n.freq = std::nullopt;
    return n;
}();

// Cache for parsed notes for performance
static std::map<std::string, Note> noteCache;

// Helper function to fill a string with repeated characters
std::string fillStr(const std::string& s, int n) {
    std::string result;
    for (int i = 0; i < std::abs(n); i++) {
        result += s;
    }
    return result;
}

// Convert step to letter
std::string stepToLetter(int step) {
    static const std::string LETTERS = "CDEFGAB";
    if (step >= 0 && step < 7) {
        return std::string(1, LETTERS[step]);
    }
    return "";
}

// Convert alteration to accidental string
std::string altToAcc(int alt) {
    if (alt < 0) {
        return fillStr("b", -alt);
    } else {
        return fillStr("#", alt);
    }
}

// Convert accidental string to alteration value
int accToAlt(const std::string& acc) {
    if (acc.empty()) {
        return 0;
    }
    if (acc[0] == 'b') {
        return -static_cast<int>(acc.length());
    } else {
        return static_cast<int>(acc.length());
    }
}

// Helper for modulo that works with negative numbers
int mod(int n, int m) {
    return ((n % m) + m) % m;
}

// Parse a note name into tokens
std::tuple<std::string, std::string, std::string, std::string> tokenizeNote(const std::string& noteName) {
    static const std::regex REGEX("^([a-gA-G]?)(#{1,}|b{1,}|x{1,}|)(-?\\d*)\\s*(.*)$");
    std::smatch matches;
    
    if (std::regex_match(noteName, matches, REGEX)) {
        std::string letter = matches[1].str();
        std::transform(letter.begin(), letter.end(), letter.begin(), ::toupper);
        
        std::string acc = matches[2].str();
        // Replace 'x' with '##' (double sharp)
        acc = std::regex_replace(acc, std::regex("x"), "##");
        
        return std::make_tuple(letter, acc, matches[3].str(), matches[4].str());
    }
    
    return std::make_tuple("", "", "", "");
}

Note parse(const std::string& noteName) {
    auto tokens = tokenizeNote(noteName);
    std::string letter = std::get<0>(tokens);
    std::string acc = std::get<1>(tokens);
    std::string octStr = std::get<2>(tokens);
    std::string remainder = std::get<3>(tokens);
    
    // Return NoNote if parsing failed or has remainder
    if (letter.empty() || !remainder.empty()) {
        return NoNote;
    }
    
    // Calculate step: Use ASCII offset
    // In TypeScript this is: step = (letter.charCodeAt(0) + 3) % 7
    // For 'A': charCodeAt(0) = 65, so (65 + 3) % 7 = 5
    // For C: step = (67 + 3) % 7 = 0
    // Note: In TS, 'A' has step 5, 'B' has step 6, 'C' has step 0, etc.
    int step = (letter[0] - 'A' + 5) % 7;
    int alt = accToAlt(acc);
    std::optional<int> oct = octStr.empty() ? std::nullopt : std::optional<int>(std::stoi(octStr));
    
    // Generate coordinates
    Pitch p;
    p.step = step;
    p.alt = alt;
    p.oct = oct;
    PitchCoordinates coord = coordinates(p);
    
    // Create note name and pitch class
    std::string name = letter + acc + octStr;
    std::string pc = letter + acc;
    
    // Calculate chroma, height, and MIDI
    // Match TypeScript implementation: (SEMI[step] + alt + 120) % 12
    int chroma = (SEMI[step] + alt + 120) % 12;
    int height;
    if (!oct.has_value()) {
        height = mod(SEMI[step] + alt, 12) - 12 * 99; // Special value for pitch classes
    } else {
        height = SEMI[step] + alt + 12 * (oct.value() + 1);
    }
    
    std::optional<int> midi = (height >= 0 && height <= 127) ? std::optional<int>(height) : std::nullopt;
    
    // Calculate frequency
    std::optional<double> freq = oct.has_value() 
        ? std::optional<double>(std::pow(2.0, (height - 69.0) / 12.0) * 440.0) 
        : std::nullopt;
    
    // Create and return the note
    Note result;
    result.empty = false;
    result.name = name;
    result.letter = letter;
    result.acc = acc;
    result.pc = pc;
    result.step = step;
    result.alt = alt;
    result.oct = oct;
    result.chroma = chroma;
    result.height = height;
    result.coord = coord;
    result.midi = midi;
    result.freq = freq;
    
    return result;
}

std::string pitchName(const Pitch& pitch) {
    const int step = pitch.step;
    const int alt = pitch.alt;
    
    std::string letter = stepToLetter(step);
    if (letter.empty()) {
        return "";
    }
    
    std::string pc = letter + altToAcc(alt);
    
    if (pitch.oct.has_value()) {
        return pc + std::to_string(pitch.oct.value());
    } else {
        return pc;
    }
}

Note coordToNote(const PitchCoordinates& coord) {
    return note(pitchFromCoordinates(coord));
}

// Main note function implementations
Note note(const std::string& src) {
    // Check cache first
    if (noteCache.find(src) != noteCache.end()) {
        return noteCache[src];
    }
    
    Note result = parse(src);
    noteCache[src] = result;
    return result;
}

Note note(const Pitch& src) {
    return note(pitchName(src));
}

Note note(const NamedPitch& src) {
    return note(src.name);
}

} // namespace tonalcpp