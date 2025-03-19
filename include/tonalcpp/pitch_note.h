#pragma once

#include "tonalcpp/pitch.h"
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace tonalcpp {

// Type definitions similar to TypeScript
using NoteWithOctave = std::string;
using PcName = std::string;
using NoteName = std::string;

/**
 * A full Note representation
 */
struct Note : public Pitch {
    bool empty = false;
    std::string letter;
    std::string acc;
    std::string pc;
    int chroma;
    int height;
    PitchCoordinates coord;
    std::optional<int> midi;
    std::optional<double> freq;

    Note() : Pitch(), empty(true), letter(""), acc(""), pc(""), 
             chroma(0), height(0), coord({}), midi(std::nullopt), freq(std::nullopt) {}
};

// Create an empty note singleton
extern const Note NoNote;

/**
 * Parse a note name string to get letter, accidental, and octave
 * @param noteName The note name string to parse
 * @return Tuple containing letter, accidental, octave string, and remainder
 */
std::tuple<std::string, std::string, std::string, std::string> tokenizeNote(const std::string& noteName);

/**
 * Convert pitch step to letter name (0 -> C, 1 -> D, etc.)
 * @param step The step number (0-6)
 * @return The corresponding letter (C-B)
 */
std::string stepToLetter(int step);

/**
 * Convert alteration to accidental string (e.g., -2 -> bb, 1 -> #)
 * @param alt The alteration number
 * @return The accidental string
 */
std::string altToAcc(int alt);

/**
 * Convert accidental string to alteration value (e.g., bb -> -2, # -> 1)
 * @param acc The accidental string
 * @return The alteration value
 */
int accToAlt(const std::string& acc);

/**
 * Parse a note name into a full Note object
 * @param noteName The note name to parse
 * @return A complete Note object
 */
Note parse(const std::string& noteName);

/**
 * Convert a Pitch object to a note name
 * @param pitch The Pitch object to convert
 * @return A note name string
 */
std::string pitchName(const Pitch& pitch);

/**
 * Convert pitch coordinates to a Note object
 * @param coord The pitch coordinates
 * @return A Note object
 */
Note coordToNote(const PitchCoordinates& coord);

/**
 * Create a Note object from a note name or a pitch object
 * @param src The source (note name, pitch, or named pitch)
 * @return A Note object
 */
Note note(const std::string& src);
Note note(const Pitch& src);
Note note(const NamedPitch& src);

} // namespace tonalcpp