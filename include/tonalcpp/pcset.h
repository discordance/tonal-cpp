#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include "tonalcpp/pitch.h"

namespace tonalcpp {

/**
 * The properties of a pitch class set
 */
struct Pcset {
    std::string name;                // Name of the set (often empty for computed sets)
    bool empty = false;              // Whether the set is empty
    int setNum = 0;                  // Unique integer identifier (0-4095)
    std::string chroma;              // Binary representation (12-character string with 1s and 0s)
    std::string normalized;          // Normalized chroma (shifted to the first 1)
    std::vector<std::string> intervals; // Intervals in the set starting from C
};

// Convenience type aliases
using PcsetChroma = std::string;
using PcsetNum = int;

// Empty Pcset constant
extern const Pcset EmptyPcset;

/**
 * Checks if a string is a valid pitch class set chroma
 * 
 * @param chroma The string to check
 * @return true if it's a valid chroma (12-digit binary string)
 */
bool isChroma(const std::string& chroma);

/**
 * Checks if an integer is a valid pitch class set number
 * 
 * @param num The number to check
 * @return true if it's a valid set number (0-4095)
 */
bool isPcsetNum(int num);

/**
 * Checks if an object is a valid Pcset
 * 
 * @param pcset The pcset to check
 * @return true if it's a valid Pcset
 */
bool isPcset(const Pcset& pcset);

/**
 * Convert a set number to its chroma representation
 * 
 * @param setNum The set number (0-4095)
 * @return The chroma string (12-digit binary)
 */
std::string setNumToChroma(int setNum);

/**
 * Convert a chroma string to its numeric representation
 * 
 * @param chroma The chroma string
 * @return The set number
 */
int chromaToNumber(const std::string& chroma);

/**
 * Get the pitch class set from various inputs:
 * - A chroma string
 * - A set number
 * - A list of note names
 * - A list of interval names
 * 
 * @param src The source (can be a Pcset, PcsetChroma, PcsetNum, or array of notes/intervals)
 * @return The Pcset object
 */
Pcset getPcset(const std::string& src);
Pcset getPcset(int src);
Pcset getPcset(const std::vector<std::string>& src);
Pcset getPcset(const Pcset& pcset);

/**
 * Get the intervals in a set (from C)
 * 
 * @param src The source set
 * @return List of interval names
 */
std::vector<std::string> intervals(const std::string& src);
std::vector<std::string> intervals(int src);
std::vector<std::string> intervals(const std::vector<std::string>& src);
std::vector<std::string> intervals(const Pcset& pcset);

/**
 * Get the chroma of a set
 * 
 * @param src The source set
 * @return The chroma string
 */
std::string chroma(const std::string& src);
std::string chroma(int src);
std::string chroma(const std::vector<std::string>& src);
std::string chroma(const Pcset& pcset);

/**
 * Get the set number
 * 
 * @param src The source set
 * @return The set number
 */
int num(const std::string& src);
int num(int src);
int num(const std::vector<std::string>& src);
int num(const Pcset& pcset);

/**
 * Get note names from a set
 * 
 * @param src The source set
 * @return List of note names
 */
std::vector<std::string> notes(const std::string& src);
std::vector<std::string> notes(int src);
std::vector<std::string> notes(const std::vector<std::string>& src);
std::vector<std::string> notes(const Pcset& pcset);

/**
 * Get all possible pitch class sets (all possible chromas)
 * 
 * @return List of all possible chromas
 */
std::vector<std::string> chromas();

/**
 * Given a list of notes or a pcset chroma, produce the rotations
 * of the chroma discarding the ones that start with "0"
 * 
 * @param src The source set
 * @param normalize Whether to remove rotations starting with 0 (default: true)
 * @return List of all modes (as chromas)
 */
std::vector<std::string> modes(const std::string& src, bool normalize = true);
std::vector<std::string> modes(int src, bool normalize = true);
std::vector<std::string> modes(const std::vector<std::string>& src, bool normalize = true);
std::vector<std::string> modes(const Pcset& pcset, bool normalize = true);

/**
 * Test if two pitch class sets are equal
 * 
 * @param s1 First set
 * @param s2 Second set
 * @return true if they are equal
 */
bool isEqual(const std::string& s1, const std::string& s2);
bool isEqual(int s1, int s2);
bool isEqual(const std::vector<std::string>& s1, const std::vector<std::string>& s2);
bool isEqual(const Pcset& s1, const Pcset& s2);

/**
 * Test if a set is a subset of another
 * 
 * @param superset The superset
 * @param subset The potential subset
 * @return true if subset is a subset of superset
 */
bool isSubsetOf(const std::string& superset, const std::string& subset);
bool isSubsetOf(int superset, int subset);
bool isSubsetOf(const std::vector<std::string>& superset, const std::vector<std::string>& subset);
bool isSubsetOf(const Pcset& superset, const Pcset& subset);

/**
 * Test if a set is a superset of another
 * 
 * @param subset The subset
 * @param superset The potential superset
 * @return true if superset is a superset of subset
 */
bool isSupersetOf(const std::string& subset, const std::string& superset);
bool isSupersetOf(int subset, int superset);
bool isSupersetOf(const std::vector<std::string>& subset, const std::vector<std::string>& superset);
bool isSupersetOf(const Pcset& subset, const Pcset& superset);

/**
 * Test if a note is included in a set
 * 
 * @param set The set
 * @param noteName The note name
 * @return true if the note is in the set
 */
bool isNoteIncludedIn(const std::string& set, const std::string& noteName);
bool isNoteIncludedIn(int set, const std::string& noteName);
bool isNoteIncludedIn(const std::vector<std::string>& set, const std::string& noteName);
bool isNoteIncludedIn(const Pcset& set, const std::string& noteName);

/**
 * Filter a list of notes using a pitch class set
 * 
 * @param set The filter set
 * @param notes List of notes to filter
 * @return Filtered list containing only notes in the set
 */
std::vector<std::string> filter(const std::string& set, const std::vector<std::string>& notes);
std::vector<std::string> filter(int set, const std::vector<std::string>& notes);
std::vector<std::string> filter(const std::vector<std::string>& set, const std::vector<std::string>& notes);
std::vector<std::string> filter(const Pcset& set, const std::vector<std::string>& notes);

} // namespace tonalcpp