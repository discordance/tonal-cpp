#include "tonalcpp/voice_leading.h"
#include "tonalcpp/note.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace tonalcpp {
namespace voice_leading {

std::vector<std::string> topNoteDiff(
    const std::vector<std::vector<std::string>>& voicings,
    const std::vector<std::string>& lastVoicing
) {
    if (voicings.empty()) {
        return {};
    }
    
    if (lastVoicing.empty()) {
        return voicings[0];
    }
    
    // Helper function to get MIDI number of top note in a voicing
    auto topNoteMidi = [](const std::vector<std::string>& voicing) -> int {
        if (voicing.empty()) return 0;
        auto midi = note::midi(voicing[voicing.size() - 1]);
        return midi.value_or(0);
    };
    
    // Helper function to calculate difference between top notes
    auto diff = [&](const std::vector<std::string>& voicing) -> int {
        return std::abs(topNoteMidi(lastVoicing) - topNoteMidi(voicing));
    };
    
    // Find the voicing with minimum difference
    auto minVoicing = std::min_element(voicings.begin(), voicings.end(),
        [&](const std::vector<std::string>& a, const std::vector<std::string>& b) {
            return diff(a) < diff(b);
        });
    
    return *minVoicing;
}

} // namespace voice_leading
} // namespace tonalcpp