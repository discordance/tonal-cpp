#pragma once

#include <string>
#include <vector>
#include <array>
#include <optional>
#include <functional>
#include "tonalcpp/chord_type.h"
#include "tonalcpp/pitch_note.h"

namespace tonalcpp {
namespace chord {

/**
 * Represents tokenized chord name parts
 */
using ChordNameTokens = std::array<std::string, 3>; // [TONIC, CHORD_TYPE, BASS]

/**
 * Interface for Chord object that extends ChordType
 */
struct Chord : public chord_type::ChordType {
    std::optional<std::string> tonic; // The tonic note (or nullopt)
    std::string type;                 // Chord type name
    std::string root;                 // Root note
    std::string bass;                 // Bass note (for slash chords)
    std::optional<int> rootDegree;    // Position of the root in the chord (1-based) or nullopt if not present
    std::string symbol;               // Chord symbol
    std::vector<std::string> notes;   // Actual chord notes
};

// Empty chord constant
extern const Chord NoChord;

/**
 * Tokenize a chord name
 * 
 * @param name The chord name to tokenize
 * @return An array with [tonic, type, bass]
 */
ChordNameTokens tokenize(const std::string& name);

/**
 * Helper function to handle bass note tokenization
 * 
 * @param note The note part
 * @param chord The chord part
 * @return Tokenized array
 */
ChordNameTokens tokenizeBass(const std::string& note, const std::string& chord);

/**
 * Get a Chord from a chord name or tokens
 * 
 * @param src The chord name or array of tokens
 * @return Chord object
 */
Chord get(const std::string& src);
Chord get(const std::vector<std::string>& tokens);

/**
 * Get chord properties
 * 
 * @param typeName The chord type name
 * @param optionalTonic Optional tonic note
 * @param optionalBass Optional bass note
 * @return Chord object
 */
Chord getChord(const std::string& typeName, 
               const std::string& optionalTonic = "", 
               const std::string& optionalBass = "");

/**
 * Alias for get function
 */
inline Chord chord(const std::string& name) { return get(name); }

/**
 * Transpose a chord name
 * 
 * @param chordName The chord name
 * @param interval The interval to transpose by
 * @return The transposed chord name
 */
std::string transpose(const std::string& chordName, const std::string& interval);

/**
 * Get all scales where the given chord fits
 * 
 * @param name The chord name
 * @return List of scale names
 */
std::vector<std::string> chordScales(const std::string& name);

/**
 * Get all chords names that are a superset of the given one
 * 
 * @param chordName The chord name
 * @return List of extended chord names
 */
std::vector<std::string> extended(const std::string& chordName);

/**
 * Find all chords names that are a subset of the given one
 * 
 * @param chordName The chord name
 * @return List of reduced chord names
 */
std::vector<std::string> reduced(const std::string& chordName);

/**
 * Return the chord notes
 * 
 * @param chordName The chord name
 * @param tonic Optional tonic (overrides chord tonic)
 * @return List of note names
 */
std::vector<std::string> notes(const std::string& chordName, const std::string& tonic = "");
std::vector<std::string> notes(const std::vector<std::string>& tokens, const std::string& tonic = "");

/**
 * Get a note name from the chord by scale degree
 * 
 * @param chordName The chord name
 * @param degree The scale degree (1-based, can be negative)
 * @param tonic Optional tonic (overrides chord tonic)
 * @return The note name at the given degree
 */
std::string degreeToNote(const std::string& chordName, int degree, const std::string& tonic = "");
std::string degreeToNote(const std::vector<std::string>& tokens, int degree, const std::string& tonic = "");

/**
 * Get a note name from the chord by zero-based step
 * 
 * @param chordName The chord name
 * @param step The step (0-based, can be negative)
 * @param tonic Optional tonic (overrides chord tonic)
 * @return The note name at the given step
 */
std::string stepToNote(const std::string& chordName, int step, const std::string& tonic = "");
std::string stepToNote(const std::vector<std::string>& tokens, int step, const std::string& tonic = "");

} // namespace chord
} // namespace tonalcpp