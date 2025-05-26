#pragma once

#include <vector>
#include <string>
#include "tonalcpp/voice_leading.h"
#include "tonalcpp/voicing_dictionary.h"

namespace tonalcpp {
namespace voicing {

/**
 * Default note range for voicing generation
 */
extern const std::vector<std::string> defaultRange;

/**
 * Default voicing dictionary
 */
extern const voicing_dictionary::VoicingDictionary& defaultDictionary;

/**
 * Default voice leading function
 */
extern const voice_leading::VoiceLeadingFunction& defaultVoiceLeading;

/**
 * Get a single voicing for a chord
 * @param chord The chord name
 * @param range The note range (defaults to ["C3", "C5"])
 * @param dictionary The voicing dictionary to use (defaults to all)
 * @param voiceLeading The voice leading function to use (defaults to topNoteDiff)
 * @param lastVoicing The previous voicing for voice leading (optional)
 * @return A single voicing as a vector of note names
 */
std::vector<std::string> get(
    const std::string& chord,
    const std::vector<std::string>& range = defaultRange,
    const voicing_dictionary::VoicingDictionary& dictionary = defaultDictionary,
    const voice_leading::VoiceLeadingFunction& voiceLeading = defaultVoiceLeading,
    const std::vector<std::string>& lastVoicing = {}
);

/**
 * Search for all possible voicings of a chord within a range
 * @param chord The chord name
 * @param range The note range (defaults to ["C3", "C5"])
 * @param dictionary The voicing dictionary to use (defaults to triads)
 * @return Vector of all possible voicings, each voicing being a vector of note names
 */
std::vector<std::vector<std::string>> search(
    const std::string& chord,
    const std::vector<std::string>& range = defaultRange,
    const voicing_dictionary::VoicingDictionary& dictionary = voicing_dictionary::triads
);

/**
 * Voice a sequence of chords with voice leading
 * @param chords Vector of chord names
 * @param range The note range (defaults to ["C3", "C5"])
 * @param dictionary The voicing dictionary to use (defaults to all)
 * @param voiceLeading The voice leading function to use (defaults to topNoteDiff)
 * @param lastVoicing The initial previous voicing for voice leading (optional)
 * @return Vector of voicings, one for each chord
 */
std::vector<std::vector<std::string>> sequence(
    const std::vector<std::string>& chords,
    const std::vector<std::string>& range = defaultRange,
    const voicing_dictionary::VoicingDictionary& dictionary = defaultDictionary,
    const voice_leading::VoiceLeadingFunction& voiceLeading = defaultVoiceLeading,
    const std::vector<std::string>& lastVoicing = {}
);

} // namespace voicing
} // namespace tonalcpp