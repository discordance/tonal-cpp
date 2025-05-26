#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace tonalcpp {
namespace voicing_dictionary {

/**
 * Type representing a voicing dictionary mapping chord symbols to interval patterns
 */
using VoicingDictionary = std::map<std::string, std::vector<std::string>>;

/**
 * Basic triad voicings
 */
extern const VoicingDictionary triads;

/**
 * Jazz left-hand voicings
 */
extern const VoicingDictionary lefthand;

/**
 * Combined triads and left-hand voicings
 */
extern const VoicingDictionary all;

/**
 * Default voicing dictionary (same as lefthand)
 */
extern const VoicingDictionary& defaultDictionary;

/**
 * Look up voicing intervals for a chord symbol in a dictionary
 * @param symbol The chord symbol to look up
 * @param dictionary The voicing dictionary to search in (defaults to lefthand)
 * @return Vector of interval patterns for the chord, or nullopt if not found
 */
std::optional<std::vector<std::string>> lookup(
    const std::string& symbol,
    const VoicingDictionary& dictionary = defaultDictionary
);

} // namespace voicing_dictionary
} // namespace tonalcpp