#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <optional>
#include <functional>

#include "tonalcpp/scale_type.h"
#include "tonalcpp/pitch_note.h"
#include "tonalcpp/note.h"

namespace tonalcpp {
namespace scale {

/**
 * A full Scale representation
 */
struct Scale : public scale_type::ScaleType {
    std::optional<std::string> tonic;
    std::string type;
    std::vector<std::string> notes;
};

// Empty Scale constant
extern const Scale NoScale;

// Type alias for scale name
using ScaleName = std::string;
// Type alias for scale name tokens (TONIC, SCALE TYPE)
using ScaleNameTokens = std::tuple<std::string, std::string>;

/**
 * Given a string with a scale name and (optionally) a tonic, split
 * those components.
 *
 * It returns a tuple with the form (tonic, name) where tonic can be a
 * note name or empty string, and name can be any arbitrary string
 * (this function doesn't check if that scale name exists)
 *
 * @param name The scale name
 * @return A tuple (tonic, name)
 */
ScaleNameTokens tokenize(const ScaleName& name);

/**
 * Return all scale names
 * @return Vector of all scale names
 */
std::vector<std::string> names();

/**
 * Get a Scale from a scale name
 * @param src The scale name or scale name tokens
 * @return The scale object
 */
Scale get(const ScaleName& src);
Scale get(const ScaleNameTokens& src);

/**
 * Detect scales that match a set of notes
 * @param notes The list of notes
 * @param tonic Optional tonic of the scale
 * @param match Type of match: "exact" or "fit"
 * @return List of scale names that match
 */
std::vector<std::string> detect(
    const std::vector<std::string>& notes,
    const std::string& tonic = "",
    const std::string& match = "fit");

/**
 * Get all chords that fit a given scale
 * @param name The scale name
 * @return List of chord names
 */
std::vector<std::string> scaleChords(const std::string& name);

/**
 * Get all scales names that are a superset of the given one
 * (has the same notes and at least one more)
 *
 * @param name The scale name or chroma
 * @return List of scale names
 */
std::vector<std::string> extended(const std::string& name);

/**
 * Find all scales names that are a subset of the given one
 * (has less notes but all from the given scale)
 *
 * @param name The scale name
 * @return List of scale names
 */
std::vector<std::string> reduced(const std::string& name);

/**
 * Given an array of notes, return the scale: a pitch class set starting from
 * the first note of the array
 *
 * @param notes List of note names
 * @return List of pitch classes with same tonic
 */
std::vector<std::string> scaleNotes(const std::vector<std::string>& notes);

/**
 * Find mode names of a scale
 * @param name The scale name
 * @return List of tuples (tonic, mode name)
 */
std::vector<std::tuple<std::string, std::string>> modeNames(const std::string& name);

/**
 * Returns all notes in the specified scale range
 * @param scale The scale name or array of notes
 * @param fromNote The starting note
 * @param toNote The ending note
 * @return List of notes in the range that belong to the scale
 */
std::vector<std::string> rangeOf(
    const std::string& scale, 
    const std::string& fromNote, 
    const std::string& toNote);
std::vector<std::string> rangeOf(
    const std::vector<std::string>& scale, 
    const std::string& fromNote, 
    const std::string& toNote);

/**
 * Returns a note from the scale degree
 * @param scaleName The scale name
 * @param degree The degree (1-based index, positive or negative)
 * @return The note name
 */
std::string degrees(const std::string& scaleName, int degree);
std::string degrees(const ScaleNameTokens& scaleName, int degree);

/**
 * Same as degree but with 0-based index
 * @param scaleName The scale name
 * @param step The step (0-based index)
 * @return The note name
 */
std::string steps(const std::string& scaleName, int step);
std::string steps(const ScaleNameTokens& scaleName, int step);

} // namespace scale
} // namespace tonalcpp