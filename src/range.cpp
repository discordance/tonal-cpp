#include "tonalcpp/range.h"
#include "tonalcpp/collection.h"
#include "tonalcpp/midi.h"
#include "tonalcpp/note.h"
#include <optional>
#include <algorithm>

namespace tonalcpp {
namespace range {

std::vector<int> numeric(const std::vector<std::string>& notes) {
    if (notes.empty()) {
        return {};
    }
    
    // Convert notes to MIDI numbers, filter out invalid ones
    std::vector<int> midiNotes;
    midiNotes.reserve(notes.size());
    
    for (const auto& note : notes) {
        auto midiValue = midi::toMidi(note);
        if (midiValue.has_value()) {
            midiNotes.push_back(midiValue.value());
        }
    }
    
    if (midiNotes.empty() || midiNotes.size() != notes.size()) {
        // There are invalid notes
        return {};
    }
    
    // Build the range by connecting consecutive MIDI notes
    std::vector<int> result;
    result.push_back(midiNotes[0]);
    
    for (size_t i = 1; i < midiNotes.size(); ++i) {
        int last = result.back();
        int current = midiNotes[i];
        
        // Get the range from last to current (excluding the first element since it's already in result)
        auto subRange = collection::range(last, current);
        if (subRange.size() > 1) {
            result.insert(result.end(), subRange.begin() + 1, subRange.end());
        }
    }
    
    return result;
}

std::vector<std::string> chromatic(const std::vector<std::string>& notes, bool sharps) {
    auto midiNumbers = numeric(notes);
    
    std::vector<std::string> result;
    result.reserve(midiNumbers.size());
    
    for (int midiNum : midiNumbers) {
        midi::ToNoteNameOptions options;
        options.sharps = sharps;
        options.pitchClass = false;
        std::string noteName = midi::midiToNoteName(midiNum, options);
        result.push_back(noteName);
    }
    
    return result;
}

} // namespace range
} // namespace tonalcpp