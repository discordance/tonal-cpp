#include "tonalcpp/voicing.h"
#include "tonalcpp/chord.h"
#include "tonalcpp/interval.h"
#include "tonalcpp/note.h"
#include "tonalcpp/range.h"
#include "tonalcpp/voice_leading.h"
#include "tonalcpp/voicing_dictionary.h"
#include "tonalcpp/helpers.h"
#include <algorithm>

namespace tonalcpp {
namespace voicing {

// Default values
const std::vector<std::string> defaultRange = {"C3", "C5"};
const voicing_dictionary::VoicingDictionary& defaultDictionary = voicing_dictionary::all;
const voice_leading::VoiceLeadingFunction& defaultVoiceLeading = voice_leading::topNoteDiff;

std::vector<std::string> get(
    const std::string& chord,
    const std::vector<std::string>& range,
    const voicing_dictionary::VoicingDictionary& dictionary,
    const voice_leading::VoiceLeadingFunction& voiceLeading,
    const std::vector<std::string>& lastVoicing
) {
    auto voicings = search(chord, range, dictionary);
    
    if (voicings.empty()) {
        return {};
    }
    
    if (lastVoicing.empty()) {
        // Pick the first voicing (lowest)
        return voicings[0];
    } else {
        // Use voice leading to select the best voicing
        return voiceLeading(voicings, lastVoicing);
    }
}

std::vector<std::vector<std::string>> search(
    const std::string& chord,
    const std::vector<std::string>& range,
    const voicing_dictionary::VoicingDictionary& dictionary
) {
    // Tokenize the chord to get tonic and symbol
    auto tokens = chord::tokenize(chord);
    std::string tonic = tokens[0];
    std::string symbol = tokens[1];
    
    if (tonic.empty()) {
        return {};
    }
    
    // Look up voicing patterns for this chord symbol
    auto voicingPatterns = voicing_dictionary::lookup(symbol, dictionary);
    if (!voicingPatterns.has_value()) {
        return {};
    }
    
    // Convert interval pattern strings to vector of intervals
    std::vector<std::vector<std::string>> voicings;
    for (const auto& pattern : voicingPatterns.value()) {
        // Split the pattern string by spaces using helpers
        auto intervals = helpers::split(pattern);
        voicings.push_back(intervals);
    }
    
    // Get all notes in the specified range
    auto notesInRange = range::chromatic(range);
    
    std::vector<std::vector<std::string>> result;
    
    for (const auto& voicing : voicings) {
        if (voicing.empty()) continue;
        
        // Transpose intervals relative to first interval (e.g. 3m 5P > 1P 3M)
        std::vector<std::string> relativeIntervals;
        for (const auto& intervalStr : voicing) {
            auto subtracted = interval::subtract(intervalStr, voicing[0]);
            relativeIntervals.push_back(subtracted);
        }
        
        // Get enharmonic correct pitch class for the bottom note
        std::string bottomPitchClass = note::transpose(tonic, voicing[0]);
        
        // Get all possible start notes for voicing
        std::vector<std::string> starts;
        for (const auto& noteInRange : notesInRange) {
            // Only get notes that match the bottom pitch class
            if (note::chroma(noteInRange) == note::chroma(bottomPitchClass)) {
                // Filter out start notes that will overshoot the top end of the range
                std::string topNote = note::transpose(noteInRange, relativeIntervals.back());
                auto topMidi = note::midi(topNote);
                auto rangeMidi = note::midi(range.back());
                
                if (topMidi.has_value() && rangeMidi.has_value() && 
                    topMidi.value() <= rangeMidi.value()) {
                    // Replace Range.chromatic notes with the correct enharmonic equivalents
                    starts.push_back(note::enharmonic(noteInRange, bottomPitchClass));
                }
            }
        }
        
        // Render one voicing for each start note
        for (const auto& start : starts) {
            std::vector<std::string> notes;
            for (const auto& relativeInterval : relativeIntervals) {
                notes.push_back(note::transpose(start, relativeInterval));
            }
            result.push_back(notes);
        }
    }
    
    return result;
}

std::vector<std::vector<std::string>> sequence(
    const std::vector<std::string>& chords,
    const std::vector<std::string>& range,
    const voicing_dictionary::VoicingDictionary& dictionary,
    const voice_leading::VoiceLeadingFunction& voiceLeading,
    const std::vector<std::string>& lastVoicing
) {
    std::vector<std::vector<std::string>> voicings;
    std::vector<std::string> currentLastVoicing = lastVoicing;
    
    for (const auto& chord : chords) {
        auto voicing = get(chord, range, dictionary, voiceLeading, currentLastVoicing);
        currentLastVoicing = voicing;
        voicings.push_back(voicing);
    }
    
    return voicings;
}

} // namespace voicing
} // namespace tonalcpp