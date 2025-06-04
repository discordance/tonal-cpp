#include "tonalcpp/midi.h"
#include <cmath>
#include <algorithm>
#include <string>
#include <limits>
#include <iterator>

namespace tonalcpp {
namespace midi {

// Constants
const std::vector<std::string> SHARPS = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const std::vector<std::string> FLATS = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
const double L2 = std::log(2.0);
const double L440 = std::log(440.0);

bool isMidi(const Midi& arg) {
    return arg >= 0 && arg <= 127;
}

std::optional<int> toMidi(const std::string& note) {
    // Try to convert to integer first
    try {
        int midiNum = std::stoi(note);
        return toMidi(midiNum);
    } catch (const std::exception&) {
        // If not a number, parse as note name
        const pitch_note::Note n = pitch_note::note(note);
        return n.midi;
    }
}

std::optional<int> toMidi(int note) {
    if (isMidi(note)) {
        return note;
    }
    return std::nullopt;
}

double midiToFreq(int midi, double tuning) {
    return std::pow(2.0, (midi - 69.0) / 12.0) * tuning;
}

double freqToMidi(double freq) {
    double v = (12.0 * (std::log(freq) - L440)) / L2 + 69.0;
    // Match TypeScript implementation which rounds to 2 decimal places
    return std::round(v * 100.0) / 100.0;
}

std::string midiToNoteName(int midi, const ToNoteNameOptions& options) {
    // Handle invalid inputs
    if (midi == std::numeric_limits<int>::min() || 
        midi == std::numeric_limits<int>::max()) {
        return "";
    }
    
    // Round to nearest MIDI value
    midi = std::round(midi);
    
    // Choose sharps or flats
    const auto& pcs = options.sharps ? SHARPS : FLATS;
    
    // Get pitch class
    const std::string& pc = pcs[midi % 12];
    
    // Return pitch class only if requested
    if (options.pitchClass) {
        return pc;
    }
    
    // Otherwise, add octave
    const int octave = (midi / 12) - 1;
    return pc + std::to_string(octave);
}

int chroma(int midi) {
    return midi % 12;
}

std::vector<int> pcsetFromChroma(const std::string& chroma) {
    std::vector<int> pcset;
    
    for (size_t i = 0; i < chroma.size() && i < 12; i++) {
        if (chroma[i] == '1') {
            pcset.push_back(static_cast<int>(i));
        }
    }
    
    return pcset;
}

std::vector<int> pcsetFromMidi(const std::vector<int>& midi) {
    // Extract chromas
    std::vector<int> chromas;
    std::transform(midi.begin(), midi.end(), std::back_inserter(chromas),
                  [](int m) { return chroma(m); });
    
    // Sort
    std::sort(chromas.begin(), chromas.end());
    
    // Remove duplicates
    auto it = std::unique(chromas.begin(), chromas.end());
    chromas.erase(it, chromas.end());
    
    return chromas;
}

std::vector<int> pcset(const std::vector<int>& notes) {
    return pcsetFromMidi(notes);
}

std::vector<int> pcset(const std::string& chroma) {
    return pcsetFromChroma(chroma);
}

std::function<std::optional<int>(int)> pcsetNearest(const std::vector<int>& notes) {
    const std::vector<int> set = pcset(notes);
    
    return [set](int midi) -> std::optional<int> {
        // If set is empty, return nothing
        if (set.empty()) {
            return std::nullopt;
        }
        
        const int ch = chroma(midi);
        
        // Search both directions for the nearest pitch class
        for (int i = 0; i < 12; i++) {
            // Match TypeScript implementation exactly: checking if set includes ch+i or ch-i
            // Note: No modulo here, directly checking if the set includes these values
            if (std::find(set.begin(), set.end(), ch + i) != set.end()) {
                return midi + i;
            }
            
            if (std::find(set.begin(), set.end(), ch - i) != set.end()) {
                return midi - i;
            }
        }
        
        // Should never get here (as we check all 12 possibilities), but just in case
        return std::nullopt;
    };
}

std::function<std::optional<int>(int)> pcsetNearest(const std::string& chroma) {
    return pcsetNearest(pcsetFromChroma(chroma));
}

std::function<int(int)> pcsetSteps(const std::vector<int>& notes, int tonic) {
    const std::vector<int> set = pcset(notes);
    const int len = static_cast<int>(set.size());
    
    return [set, len, tonic](int step) -> int {
        // Calculate the index within the set, matching the TypeScript behavior
        int index;
        if (step < 0) {
            // In TypeScript: (len - (-step % len)) % len
            // C++ modulo for negative numbers behaves differently than JavaScript
            int modStep = (-step) % len;  // This is positive in C++
            if (modStep == 0) {
                index = 0;
            } else {
                index = len - modStep;
            }
        } else {
            index = step % len;
        }
        
        // Calculate octave displacement
        int octaves;
        if (step < 0) {
            // For negative steps, Math.floor(step / len) in JS
            // Equivalent in C++ for negative values:
            octaves = (step - len + 1) / len;
        } else {
            octaves = step / len;
        }
        
        // Return the note
        return set[index] + octaves * 12 + tonic;
    };
}

std::function<int(int)> pcsetSteps(const std::string& chroma, int tonic) {
    return pcsetSteps(pcsetFromChroma(chroma), tonic);
}

std::function<std::optional<int>(int)> pcsetDegrees(const std::vector<int>& notes, int tonic) {
    auto steps = pcsetSteps(notes, tonic);
    
    return [steps](int degree) -> std::optional<int> {
        if (degree == 0) {
            return std::nullopt;
        }
        
        // Match TypeScript behavior
        // For positive degrees, we adjust by -1 (1-based to 0-based)
        // For negative degrees, we keep as is (already correct mapping)
        return steps(degree > 0 ? degree - 1 : degree);
    };
}

std::function<std::optional<int>(int)> pcsetDegrees(const std::string& chroma, int tonic) {
    return pcsetDegrees(pcsetFromChroma(chroma), tonic);
}

} // namespace midi
} // namespace tonalcpp