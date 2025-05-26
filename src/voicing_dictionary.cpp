#include "tonalcpp/voicing_dictionary.h"
#include "tonalcpp/chord.h"
#include <algorithm>

namespace tonalcpp {
namespace voicing_dictionary {

// Basic triad voicings
const VoicingDictionary triads = {
    {"M", {"1P 3M 5P", "3M 5P 8P", "5P 8P 10M"}},
    {"m", {"1P 3m 5P", "3m 5P 8P", "5P 8P 10m"}},
    {"o", {"1P 3m 5d", "3m 5d 8P", "5d 8P 10m"}},
    {"aug", {"1P 3m 5A", "3m 5A 8P", "5A 8P 10m"}}
};

// Jazz left-hand voicings
const VoicingDictionary lefthand = {
    {"m7", {"3m 5P 7m 9M", "7m 9M 10m 12P"}},
    {"7", {"3M 6M 7m 9M", "7m 9M 10M 13M"}},
    {"^7", {"3M 5P 7M 9M", "7M 9M 10M 12P"}},
    {"69", {"3M 5P 6A 9M"}},
    {"m7b5", {"3m 5d 7m 8P", "7m 8P 10m 12d"}},
    {"7b9", {"3M 6m 7m 9m", "7m 9m 10M 13m"}}, // b9 / b13
    {"7b13", {"3M 6m 7m 9m", "7m 9m 10M 13m"}}, // b9 / b13
    {"o7", {"1P 3m 5d 6M", "5d 6M 8P 10m"}},
    {"7#11", {"7m 9M 11A 13A"}},
    {"7#9", {"3M 7m 9A"}},
    {"mM7", {"3m 5P 7M 9M", "7M 9M 10m 12P"}},
    {"m6", {"3m 5P 6M 9M", "6M 9M 10m 12P"}}
};

// Combined triads and left-hand voicings
const VoicingDictionary all = {
    // Triads
    {"M", {"1P 3M 5P", "3M 5P 8P", "5P 8P 10M"}},
    {"m", {"1P 3m 5P", "3m 5P 8P", "5P 8P 10m"}},
    {"o", {"1P 3m 5d", "3m 5d 8P", "5d 8P 10m"}},
    {"aug", {"1P 3m 5A", "3m 5A 8P", "5A 8P 10m"}},
    // Left-hand voicings
    {"m7", {"3m 5P 7m 9M", "7m 9M 10m 12P"}},
    {"7", {"3M 6M 7m 9M", "7m 9M 10M 13M"}},
    {"^7", {"3M 5P 7M 9M", "7M 9M 10M 12P"}},
    {"69", {"3M 5P 6A 9M"}},
    {"m7b5", {"3m 5d 7m 8P", "7m 8P 10m 12d"}},
    {"7b9", {"3M 6m 7m 9m", "7m 9m 10M 13m"}}, // b9 / b13
    {"7b13", {"3M 6m 7m 9m", "7m 9m 10M 13m"}}, // b9 / b13
    {"o7", {"1P 3m 5d 6M", "5d 6M 8P 10m"}},
    {"7#11", {"7m 9M 11A 13A"}},
    {"7#9", {"3M 7m 9A"}},
    {"mM7", {"3m 5P 7M 9M", "7M 9M 10m 12P"}},
    {"m6", {"3m 5P 6M 9M", "6M 9M 10m 12P"}}
};

// Default dictionary reference
const VoicingDictionary& defaultDictionary = lefthand;

std::optional<std::vector<std::string>> lookup(
    const std::string& symbol,
    const VoicingDictionary& dictionary
) {
    // Direct lookup first
    auto it = dictionary.find(symbol);
    if (it != dictionary.end()) {
        return it->second;
    }
    
    // Try to find using chord aliases
    chord::Chord chordInfo = chord::get("C" + symbol);
    if (!chordInfo.aliases.empty()) {
        // Look for any alias that matches a key in the dictionary
        for (const auto& alias : chordInfo.aliases) {
            auto aliasIt = dictionary.find(alias);
            if (aliasIt != dictionary.end()) {
                return aliasIt->second;
            }
        }
    }
    
    return std::nullopt;
}

} // namespace voicing_dictionary
} // namespace tonalcpp