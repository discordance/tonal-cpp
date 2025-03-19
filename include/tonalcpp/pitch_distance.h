#pragma once

#include <string>
#include <vector>
#include <optional>
#include "tonalcpp/pitch.h"
#include "tonalcpp/pitch_note.h"
#include "tonalcpp/pitch_interval.h"

namespace tonalcpp {

/**
 * Transpose a note by an interval
 * @param noteName The note name to transpose
 * @param intervalName The interval to transpose by
 * @return The resulting note name
 */
std::string transpose(const std::string& noteName, const std::string& intervalName);

/**
 * Transpose a note by an interval (object version)
 * @param n The note to transpose
 * @param i The interval to transpose by
 * @return The resulting note name
 */
std::string transpose(const Note& n, const Interval& i);

/**
 * Calculate the interval between two notes
 * @param fromNote The source note
 * @param toNote The destination note
 * @return The interval name
 */
std::string distance(const std::string& fromNote, const std::string& toNote);

/**
 * Calculate the interval between two notes (object version)
 * @param fromNote The source note
 * @param toNote The destination note 
 * @return The interval name
 */
std::string distance(const Note& fromNote, const Note& toNote);

/**
 * Create a transposer function based on a tonic and a set of intervals
 * @param intervals The intervals to use for transposition
 * @param tonic The tonic note (reference point)
 * @return A vector of transposed notes
 */
std::vector<std::string> tonicIntervalsTransposer(
    const std::vector<std::string>& intervals, 
    const std::string& tonic);

} // namespace tonalcpp