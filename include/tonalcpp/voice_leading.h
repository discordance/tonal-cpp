#pragma once

#include <vector>
#include <string>
#include <functional>

namespace tonalcpp {
namespace voice_leading {

/**
 * A function that decides which of a set of voicings is picked as a follow up to lastVoicing.
 */
using VoiceLeadingFunction = std::function<std::vector<std::string>(
    const std::vector<std::vector<std::string>>&,
    const std::vector<std::string>&
)>;

/**
 * Voice leading function that selects the voicing with the closest top note to the last voicing
 * @param voicings Array of possible voicings (each voicing is an array of note names)
 * @param lastVoicing The previous voicing to compare against
 * @return The voicing with the closest top note
 */
std::vector<std::string> topNoteDiff(
    const std::vector<std::vector<std::string>>& voicings,
    const std::vector<std::string>& lastVoicing
);

} // namespace voice_leading
} // namespace tonalcpp