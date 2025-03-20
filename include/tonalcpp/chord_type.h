#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "tonalcpp/pcset.h"

namespace tonalcpp {
namespace chord_type {

/**
 * Possible chord qualities
 */
enum class ChordQuality {
    Major,
    Minor,
    Augmented,
    Diminished,
    Unknown
};

/**
 * ChordType data structure that extends Pcset
 */
struct ChordType : public pcset::Pcset {
    std::string name;                  // Full chord name
    ChordQuality quality;              // Chord quality
    std::vector<std::string> aliases;  // Alternative names/symbols
};

// Empty chord type constant
extern const ChordType NoChordType;

// Type for chord type identification (name, chroma, or setNum)
using ChordTypeName = std::string;

/**
 * Get a chord type by name, chroma, or set number
 * 
 * @param type The chord type name, chroma, or setNum
 * @return The chord type object
 */
ChordType getChordType(const std::string& type);
ChordType getChordType(int type);

/**
 * Get all chord (long) names
 * 
 * @return List of all chord names
 */
std::vector<std::string> names();

/**
 * Get all chord symbols
 * 
 * @return List of all chord symbols
 */
std::vector<std::string> symbols();

/**
 * Get keys used to reference chord types
 * 
 * @return List of all keys in the dictionary
 */
std::vector<std::string> keys();

/**
 * Return a list of all chord types
 * 
 * @return List of all chord types
 */
std::vector<ChordType> all();

/**
 * Clear the dictionary
 */
void removeAll();

/**
 * Add a chord to the dictionary
 * 
 * @param intervals List of intervals
 * @param aliases List of aliases
 * @param fullName Optional full name
 */
void add(const std::vector<std::string>& intervals, 
         const std::vector<std::string>& aliases, 
         const std::string& fullName = "");

/**
 * Add an alias for a chord
 * 
 * @param chord The chord type
 * @param alias The alias to add
 */
void addAlias(const ChordType& chord, const std::string& alias);

/**
 * Determine the quality of a chord based on its intervals
 * 
 * @param intervals List of intervals
 * @return The chord quality
 */
ChordQuality getQuality(const std::vector<std::string>& intervals);

/**
 * Initialize the chord dictionary with predefined data
 */
void initChordTypes();

// Using split from helpers

} // namespace chord_type
} // namespace tonalcpp