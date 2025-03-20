#pragma once

#include "tonalcpp/pitch_note.h"
#include <optional>
#include <string>
#include <vector>
#include <functional>

namespace tonalcpp {
namespace midi {

using Midi = int;

// Constants
extern const double L2;  // Natural log of 2
extern const double L440;  // Natural log of 440

/**
 * Check if a value is a valid MIDI note number (0-127)
 * @param arg The value to check
 * @return True if it's a valid MIDI note number
 */
bool isMidi(const Midi& arg);

/**
 * Get the note midi number (a number between 0 and 127)
 * It returns std::nullopt if not a valid note name
 *
 * @param note The note name or midi number
 * @return The midi number or std::nullopt if not valid note
 */
std::optional<int> toMidi(const std::string& note);
std::optional<int> toMidi(int note);

/**
 * Get the frequency in hertz from midi number
 *
 * @param midi The note midi number
 * @param tuning A4 tuning frequency in Hz (440 by default)
 * @return The frequency
 */
double midiToFreq(int midi, double tuning = 440.0);

/**
 * Get the midi number from a frequency in hertz. The midi number can
 * contain decimals (with two digits precision)
 *
 * @param frequency The frequency in Hz
 * @return The MIDI note number (can include decimals)
 */
double freqToMidi(double freq);

/**
 * Options for converting MIDI to note name
 */
struct ToNoteNameOptions {
    bool pitchClass = false;
    bool sharps = false;
};

/**
 * Given a midi number, returns a note name. The altered notes will have
 * flats unless explicitly set with the sharps option.
 *
 * @param midi The midi note number
 * @param options Options for conversion (pitchClass, sharps)
 * @return The note name
 */
std::string midiToNoteName(int midi, const ToNoteNameOptions& options = {});

/**
 * Get the chroma (pitch class value 0-11) from a MIDI note number
 * @param midi The MIDI note number
 * @return The chroma value (0-11)
 */
int chroma(int midi);

/**
 * Convert a binary chroma string to a set of pitch classes
 * @param chroma A binary string representing a pitch class set (e.g., "101010101010")
 * @return Array of pitch class numbers
 */
std::vector<int> pcsetFromChroma(const std::string& chroma);

/**
 * Convert an array of MIDI note numbers to a unique set of pitch classes
 * @param midi Array of MIDI note numbers
 * @return Array of unique pitch class numbers, sorted
 */
std::vector<int> pcsetFromMidi(const std::vector<int>& midi);

/**
 * Given a list of midi numbers or chroma string, returns the pitch class set
 * @param notes Array of MIDI numbers or a chroma string
 * @return Array of pitch class numbers
 */
std::vector<int> pcset(const std::vector<int>& notes);
std::vector<int> pcset(const std::string& chroma);

/**
 * Returns a function that maps a midi note to the nearest note in the pcset
 * @param notes Array of MIDI numbers or a chroma string
 * @return Function that finds the nearest note in the set
 */
std::function<std::optional<int>(int)> pcsetNearest(const std::vector<int>& notes);
std::function<std::optional<int>(int)> pcsetNearest(const std::string& chroma);

/**
 * Returns a function that maps steps in the scale to midi notes
 * @param notes Array of MIDI numbers or a chroma string
 * @param tonic The tonic note as a MIDI note number
 * @return Function that converts scale steps to MIDI notes
 */
std::function<int(int)> pcsetSteps(const std::vector<int>& notes, int tonic);
std::function<int(int)> pcsetSteps(const std::string& chroma, int tonic);

/**
 * Returns a function that maps scale degrees to midi notes
 * @param notes Array of MIDI numbers or a chroma string
 * @param tonic The tonic note as a MIDI note number
 * @return Function that converts scale degrees to MIDI notes
 */
std::function<std::optional<int>(int)> pcsetDegrees(const std::vector<int>& notes, int tonic);
std::function<std::optional<int>(int)> pcsetDegrees(const std::string& chroma, int tonic);

} // namespace midi
} // namespace tonalcpp