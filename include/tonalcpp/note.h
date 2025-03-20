#pragma once

#include "tonalcpp/pitch.h"
#include "tonalcpp/pitch_note.h"
#include "tonalcpp/pitch_distance.h"
#include "tonalcpp/pitch_interval.h"
#include "tonalcpp/midi.h"
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace tonalcpp {
namespace note {

using NoteName = pitch_note::NoteName;
using NoteComparator = std::function<bool(const pitch_note::Note&, const pitch_note::Note&)>;

/**
 * Return the natural note names without octave
 * @param array Optional array of note strings to extract names from
 * @return Vector of note names
 */
std::vector<std::string> names(const std::vector<std::string>& array = {});

/**
 * Get a note from a note name
 * @param src The note name or note object
 * @return A Note object with all note properties
 */
pitch_note::Note get(const std::string& src);

/**
 * Get the note name
 * @param note The note name
 * @return The note name string
 */
std::string name(const std::string& note);

/**
 * Get the note pitch class
 * @param note The note name
 * @return The pitch class string
 */
std::string pitchClass(const std::string& note);

/**
 * Get the note accidentals
 * @param note The note name
 * @return The accidentals string
 */
std::string accidentals(const std::string& note);

/**
 * Get the note octave
 * @param note The note name
 * @return The octave or nullopt if not present
 */
std::optional<int> octave(const std::string& note);

/**
 * Get the note midi number
 * @param note The note name
 * @return The midi number or nullopt if invalid
 */
std::optional<int> midi(const std::string& note);

/**
 * Get the note frequency
 * @param note The note name
 * @return The frequency or nullopt if invalid
 */
std::optional<double> freq(const std::string& note);

/**
 * Get the note chroma
 * @param note The note name
 * @return The chroma (0-11)
 */
int chroma(const std::string& note);

/**
 * Given a midi number, returns a note name using flats for altered notes
 * @param midiNum The midi note number
 * @return The note name
 */
std::string fromMidi(int midiNum);

/**
 * Given a midi number, returns a note name using sharps for altered notes
 * @param midiNum The midi note number
 * @return The note name
 */
std::string fromMidiSharps(int midiNum);

/**
 * Given a frequency in Hz, returns a note name using flats for altered notes
 * @param frequency The frequency in Hz
 * @return The note name
 */
std::string fromFreq(double frequency);

/**
 * Given a frequency in Hz, returns a note name using sharps for altered notes
 * @param frequency The frequency in Hz
 * @return The note name
 */
std::string fromFreqSharps(double frequency);

/**
 * Calculate the interval between two notes
 * @param from The origin note
 * @param to The destination note
 * @return The interval name
 */
std::string distance(const std::string& from, const std::string& to);

/**
 * Transpose a note by an interval
 * @param noteName The note to transpose
 * @param intervalName The interval to transpose by
 * @return The transposed note name
 */
std::string transpose(const std::string& noteName, const std::string& intervalName);

/**
 * Alias for transpose function
 */
inline std::string tr(const std::string& noteName, const std::string& intervalName) {
    return transpose(noteName, intervalName);
}

/**
 * Create a function that transposes by the given interval
 * @param interval The interval to transpose by
 * @return A function that transposes notes by the interval
 */
std::function<std::string(const std::string&)> transposeBy(const std::string& interval);

/**
 * Alias for transposeBy function
 */
inline std::function<std::string(const std::string&)> trBy(const std::string& interval) {
    return transposeBy(interval);
}

/**
 * Create a function that transposes from the given note
 * @param note The note to transpose from
 * @return A function that transposes by different intervals from the note
 */
std::function<std::string(const std::string&)> transposeFrom(const std::string& note);

/**
 * Alias for transposeFrom function
 */
inline std::function<std::string(const std::string&)> trFrom(const std::string& note) {
    return transposeFrom(note);
}

/**
 * Transpose a note by a number of perfect fifths
 * @param noteName The note name to transpose
 * @param fifths The number of fifths to transpose by
 * @return The transposed note name
 */
std::string transposeFifths(const std::string& noteName, int fifths);

/**
 * Alias for transposeFifths function
 */
inline std::string trFifths(const std::string& noteName, int fifths) {
    return transposeFifths(noteName, fifths);
}

/**
 * Transpose a note by a number of octaves
 * @param noteName The note name to transpose
 * @param octaves The number of octaves to transpose by
 * @return The transposed note name
 */
std::string transposeOctaves(const std::string& noteName, int octaves);

/**
 * Ascending note comparator (a comes before b if a.height < b.height)
 */
bool ascending(const pitch_note::Note& a, const pitch_note::Note& b);

/**
 * Descending note comparator (a comes before b if a.height > b.height)
 */
bool descending(const pitch_note::Note& a, const pitch_note::Note& b);

/**
 * Sort and extract note names from an array
 * @param notes Array of note names
 * @param comparator The comparator function to use for sorting
 * @return Sorted array of note names
 */
std::vector<std::string> sortedNames(
    const std::vector<std::string>& notes,
    const NoteComparator& comparator = ascending);

/**
 * Sort and extract unique note names from an array
 * @param notes Array of note names
 * @return Sorted array of unique note names
 */
std::vector<std::string> sortedUniqNames(const std::vector<std::string>& notes);

/**
 * Simplify a note
 * @param noteName The note name to simplify
 * @return The simplified note name
 */
std::string simplify(const std::string& noteName);

/**
 * Get enharmonic of a note
 * @param noteName The note name
 * @param destName Optional destination pitch class
 * @return The enharmonic note name
 */
std::string enharmonic(const std::string& noteName, const std::string& destName = "");

} // namespace note
} // namespace tonalcpp