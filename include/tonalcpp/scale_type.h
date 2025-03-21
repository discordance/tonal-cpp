#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "tonalcpp/pcset.h"

namespace tonalcpp {
namespace scale_type {

/**
 * Properties for a scale in the scale dictionary. It's a pitch class set
 * properties with the following additional information:
 * - name: the scale name
 * - aliases: alternative list of names
 * - intervals: an array of interval names
 */
struct ScaleType : public pcset::Pcset {
    std::vector<std::string> aliases;
};

// Empty ScaleType constant
extern const ScaleType NoScaleType;

// Type alias for scale type identifiers
using ScaleTypeName = std::string; // Can be string name or chroma

/**
 * Return the names of all scale types
 *
 * @return List of scale names
 */
std::vector<std::string> names();

/**
 * Given a scale name or chroma, return the scale properties
 *
 * @param type Scale name or pitch class set chroma
 * @return The scale type properties
 */
ScaleType get(const ScaleTypeName& type);
ScaleType get(int setNum);

/**
 * Return a list of all scale types
 *
 * @return List of all scale types
 */
std::vector<ScaleType> all();

/**
 * Keys used to reference scale types
 * 
 * @return List of all keys in the dictionary
 */
std::vector<std::string> keys();

/**
 * Clear the dictionary
 */
void removeAll();

/**
 * Add a scale into the dictionary
 * 
 * @param intervals List of intervals
 * @param name The scale name
 * @param aliases Optional list of alternative names
 * @return The created scale type
 */
ScaleType add(const std::vector<std::string>& intervals, 
              const std::string& name,
              const std::vector<std::string>& aliases = {});

/**
 * Add an alias for a scale
 * 
 * @param scale The scale
 * @param alias The alias name
 */
void addAlias(const ScaleType& scale, const std::string& alias);

// Initialize the scale type dictionary
void initialize();

} // namespace scale_type
} // namespace tonalcpp