#include "tonalcpp/pcset.h"
#include "tonalcpp/collection.h"
#include "tonalcpp/pitch_note.h"
#include "tonalcpp/pitch_interval.h"
#include "tonalcpp/pitch_distance.h"
#include <regex>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <bitset>

namespace tonalcpp {

// Constants - computed from semitones (0-11)
const std::vector<std::string> INTERVALS = {
    "1P", "2m", "2M", "3m", "3M", "4P", "5d", "5P", "6m", "6M", "7m", "7M"
};

// Empty Pcset definition
const Pcset EmptyPcset = {
    "", true, 0, "000000000000", "000000000000", {}
};

// Cache for computed pcsets
static std::unordered_map<std::string, Pcset> pcsetCache = { {EmptyPcset.chroma, EmptyPcset} };

// Regex for validating chromas
const std::regex CHROMA_REGEX("^[01]{12}$");

bool isChroma(const std::string& chroma) {
    return std::regex_match(chroma, CHROMA_REGEX);
}

bool isPcsetNum(int num) {
    return num >= 0 && num <= 4095;
}

bool isPcset(const Pcset& pcset) {
    return isChroma(pcset.chroma);
}

std::string setNumToChroma(int setNum) {
    // Match TypeScript implementation:
    // Return 12-character binary string padded with leading zeros
    std::bitset<12> bits(setNum);
    return bits.to_string();
}

int chromaToNumber(const std::string& chroma) {
    return std::stoi(chroma, nullptr, 2);
}

// Convert a chroma to intervals starting from C
std::vector<std::string> chromaToIntervals(const std::string& chroma) {
    std::vector<std::string> ivls;
    for (int i = 0; i < 12; i++) {
        if (chroma[i] == '1') {
            ivls.push_back(INTERVALS[i]);
        }
    }
    return ivls;
}

// Get all rotations of a chroma
std::vector<std::string> chromaRotations(const std::string& chroma) {
    std::vector<std::string> rotations;
    std::vector<char> binary(chroma.begin(), chroma.end());
    
    for (int i = 0; i < 12; i++) {
        std::vector<char> rotated = rotate(i, binary);
        rotations.push_back(std::string(rotated.begin(), rotated.end()));
    }
    
    return rotations;
}

// Convert a list of notes or intervals to a chroma
std::string listToChroma(const std::vector<std::string>& list) {
    if (list.empty()) {
        return EmptyPcset.chroma;
    }
    
    // Use a 12-bit array initialized to zeros
    std::vector<int> binary(12, 0);
    bool valid = false;
    
    for (const auto& item : list) {
        // Try to parse as a note
        Note n = note(item);
        
        if (!n.empty) {
            valid = true;
            // In Tonal.js, chromas are strings like "101010000000" where 
            // the first bit is C, second is C#, etc.
            if (n.chroma >= 0 && n.chroma < 12) {
                binary[n.chroma] = 1;
            }
        } else {
            // Try to parse as an interval
            Interval i = interval(item);
            if (i.name != "") {
                valid = true;
                // Intervals use chroma too, but starting from 0 (unison)
                if (i.chroma >= 0 && i.chroma < 12) {
                    binary[i.chroma] = 1;
                }
            }
        }
    }
    
    if (!valid) {
        return EmptyPcset.chroma;
    }
    
    // Convert binary to string
    std::string result;
    for (int bit : binary) {
        result += std::to_string(bit);
    }
    
    return result;
}

// Convert a chroma to a Pcset
Pcset chromaToPcset(const std::string& chroma) {
    int setNum = chromaToNumber(chroma);
    
    // Find the normalized form (chroma rotated to first 1, with highest binary value)
    // This matches TypeScript implementation:
    // normalizedNum = chromaRotations(chroma)
    //    .map(chromaToNumber)
    //    .filter((n) => n >= 2048)
    //    .sort()[0];
    auto rotations = chromaRotations(chroma);
    
    // Filter rotations that start with 1 (num >= 2048) and sort them
    std::vector<int> validNums;
    for (const auto& rotation : rotations) {
        int num = chromaToNumber(rotation);
        if (num >= 2048) {
            validNums.push_back(num);
        }
    }
    
    // Sort to find the highest binary value that starts with 1
    std::sort(validNums.begin(), validNums.end());
    
    // First element is the normalized form (if any were found)
    int normalizedNum = validNums.empty() ? 0 : validNums[0];
    std::string normalized = normalizedNum == 0 ? 
                            chroma : // Default if no rotation starts with 1
                            setNumToChroma(normalizedNum);
    
    std::vector<std::string> ivls = chromaToIntervals(chroma);
    
    Pcset pcset;
    pcset.name = "";
    pcset.empty = false;
    pcset.setNum = setNum;
    pcset.chroma = chroma;
    pcset.normalized = normalized;
    pcset.intervals = ivls;
    
    return pcset;
}

// Get implementation
Pcset getPcset(const std::string& src) {
    // Check if it's a chroma
    if (isChroma(src)) {
        if (pcsetCache.find(src) != pcsetCache.end()) {
            return pcsetCache[src];
        }
        
        Pcset pcset = chromaToPcset(src);
        pcsetCache[src] = pcset;
        return pcset;
    }
    
    // If not a chroma, treat as EmptyPcset
    return EmptyPcset;
}

Pcset getPcset(int src) {
    if (isPcsetNum(src)) {
        std::string chroma = setNumToChroma(src);
        return getPcset(chroma);
    }
    return EmptyPcset;
}

Pcset getPcset(const std::vector<std::string>& src) {
    std::string chroma = listToChroma(src);
    return getPcset(chroma);
}

Pcset getPcset(const Pcset& pcset) {
    if (isPcset(pcset)) {
        return pcset;
    }
    return EmptyPcset;
}

// Interval implementations
std::vector<std::string> intervals(const std::string& src) {
    return getPcset(src).intervals;
}

std::vector<std::string> intervals(int src) {
    return getPcset(src).intervals;
}

std::vector<std::string> intervals(const std::vector<std::string>& src) {
    return getPcset(src).intervals;
}

std::vector<std::string> intervals(const Pcset& pcset) {
    return getPcset(pcset).intervals;
}

// Chroma implementations
std::string chroma(const std::string& src) {
    return getPcset(src).chroma;
}

std::string chroma(int src) {
    return getPcset(src).chroma;
}

std::string chroma(const std::vector<std::string>& src) {
    return getPcset(src).chroma;
}

std::string chroma(const Pcset& pcset) {
    return getPcset(pcset).chroma;
}

// Num implementations
int num(const std::string& src) {
    return getPcset(src).setNum;
}

int num(int src) {
    return getPcset(src).setNum;
}

int num(const std::vector<std::string>& src) {
    return getPcset(src).setNum;
}

int num(const Pcset& pcset) {
    return getPcset(pcset).setNum;
}

// Notes implementation
std::vector<std::string> notes(const std::string& src) {
    return notes(getPcset(src));
}

std::vector<std::string> notes(int src) {
    return notes(getPcset(src));
}

std::vector<std::string> notes(const std::vector<std::string>& src) {
    return notes(getPcset(src));
}

std::vector<std::string> notes(const Pcset& pcset) {
    if (pcset.empty) {
        return {};
    }
    
    // Use the intervals from the pcset and transpose from C
    // This matches the TypeScript implementation:
    // return get(set).intervals.map((ivl) => transpose("C", ivl));
    return tonicIntervalsTransposer(pcset.intervals, "C");
}

// Get all possible chromas
std::vector<std::string> chromas() {
    std::vector<std::string> result;
    for (int i = 2048; i <= 4095; i++) {
        result.push_back(setNumToChroma(i));
    }
    return result;
}

// Modes implementation
std::vector<std::string> modes(const std::string& src, bool normalize) {
    Pcset pcs = getPcset(src);
    
    std::vector<char> binary(pcs.chroma.begin(), pcs.chroma.end());
    std::vector<std::string> result;
    
    for (int i = 0; i < 12; i++) {
        std::vector<char> rotated = rotate(i, binary);
        std::string rotatedStr(rotated.begin(), rotated.end());
        
        if (!normalize || rotatedStr[0] == '1') {
            result.push_back(rotatedStr);
        }
    }
    
    return result;
}

std::vector<std::string> modes(int src, bool normalize) {
    return modes(getPcset(src), normalize);
}

std::vector<std::string> modes(const std::vector<std::string>& src, bool normalize) {
    return modes(getPcset(src), normalize);
}

std::vector<std::string> modes(const Pcset& pcset, bool normalize) {
    return modes(pcset.chroma, normalize);
}

// IsEqual implementation
bool isEqual(const std::string& s1, const std::string& s2) {
    return getPcset(s1).setNum == getPcset(s2).setNum;
}

bool isEqual(int s1, int s2) {
    return getPcset(s1).setNum == getPcset(s2).setNum;
}

bool isEqual(const std::vector<std::string>& s1, const std::vector<std::string>& s2) {
    return getPcset(s1).setNum == getPcset(s2).setNum;
}

bool isEqual(const Pcset& s1, const Pcset& s2) {
    return getPcset(s1).setNum == getPcset(s2).setNum;
}

// IsSubsetOf implementation
bool isSubsetOf(const std::string& superset, const std::string& subset) {
    int s = getPcset(superset).setNum;
    int o = getPcset(subset).setNum;
    
    return s && s != o && (o & s) == o;
}

bool isSubsetOf(int superset, int subset) {
    return isSubsetOf(getPcset(superset), getPcset(subset));
}

bool isSubsetOf(const std::vector<std::string>& superset, const std::vector<std::string>& subset) {
    return isSubsetOf(getPcset(superset), getPcset(subset));
}

bool isSubsetOf(const Pcset& superset, const Pcset& subset) {
    int s = superset.setNum;
    int o = subset.setNum;
    
    return s && s != o && (o & s) == o;
}

// IsSupersetOf implementation
bool isSupersetOf(const std::string& subset, const std::string& superset) {
    int s = getPcset(subset).setNum;
    int o = getPcset(superset).setNum;
    
    return s && s != o && (o | s) == o;
}

bool isSupersetOf(int subset, int superset) {
    return isSupersetOf(getPcset(subset), getPcset(superset));
}

bool isSupersetOf(const std::vector<std::string>& subset, const std::vector<std::string>& superset) {
    return isSupersetOf(getPcset(subset), getPcset(superset));
}

bool isSupersetOf(const Pcset& subset, const Pcset& superset) {
    int s = subset.setNum;
    int o = superset.setNum;
    
    return s && s != o && (o | s) == o;
}

// IsNoteIncludedIn implementation
bool isNoteIncludedIn(const std::string& set, const std::string& noteName) {
    Pcset pcset = getPcset(set);
    Note n = note(noteName);
    
    if (n.empty || pcset.empty) {
        return false;
    }
    
    // Get the chroma of the set and check if the note's chroma bit is set
    return pcset.chroma.size() == 12 && pcset.chroma[n.chroma] == '1';
}

bool isNoteIncludedIn(int set, const std::string& noteName) {
    return isNoteIncludedIn(getPcset(set), noteName);
}

bool isNoteIncludedIn(const std::vector<std::string>& set, const std::string& noteName) {
    return isNoteIncludedIn(getPcset(set), noteName);
}

bool isNoteIncludedIn(const Pcset& set, const std::string& noteName) {
    Note n = note(noteName);
    
    if (n.empty || set.empty) {
        return false;
    }
    
    // Get the chroma of the set and check if the note's chroma bit is set
    return set.chroma.size() == 12 && set.chroma[n.chroma] == '1';
}

// Filter implementation
std::vector<std::string> filter(const std::string& set, const std::vector<std::string>& notes) {
    return filter(getPcset(set), notes);
}

std::vector<std::string> filter(int set, const std::vector<std::string>& notes) {
    return filter(getPcset(set), notes);
}

std::vector<std::string> filter(const std::vector<std::string>& set, const std::vector<std::string>& notes) {
    return filter(getPcset(set), notes);
}

std::vector<std::string> filter(const Pcset& set, const std::vector<std::string>& notes) {
    if (set.empty) {
        return {};
    }
    
    std::vector<std::string> filtered;
    for (const auto& noteName : notes) {
        if (isNoteIncludedIn(set, noteName)) {
            filtered.push_back(noteName);
        }
    }
    
    return filtered;
}

} // namespace tonalcpp