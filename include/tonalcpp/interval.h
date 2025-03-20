#pragma once

#include <string>
#include <vector>
#include <functional>
#include "tonalcpp/pitch.h"
#include "tonalcpp/pitch_distance.h"
#include "tonalcpp/pitch_interval.h"

namespace tonalcpp {
namespace interval {

/**
 * Get the natural list of names
 * @return Vector of interval names
 */
std::vector<pitch_interval::IntervalName> names();

/**
 * Get properties of an interval
 * @param name Interval name
 * @return Interval object
 */
inline pitch_interval::Interval get(const std::string& name) {
    return pitch_interval::interval(name);
}

/**
 * Get name of an interval
 * @param name Interval name
 * @return Interval name string
 */
std::string name(const std::string& name);

/**
 * Get semitones of an interval
 * @param name Interval name
 * @return Number of semitones
 */
int semitones(const std::string& name);

/**
 * Get quality of an interval
 * @param name Interval name
 * @return Quality string
 */
pitch_interval::Quality quality(const std::string& name);

/**
 * Get number of an interval
 * @param name Interval name
 * @return Interval number
 */
int num(const std::string& name);

/**
 * Get the simplified version of an interval
 * @param name Interval name
 * @return Simplified interval
 */
pitch_interval::IntervalName simplify(const pitch_interval::IntervalName& name);

/**
 * Get the inversion of an interval
 * @param name Interval name
 * @return Inverted interval
 */
pitch_interval::IntervalName invert(const pitch_interval::IntervalName& name);

/**
 * Get interval name from semitones number
 * @param semitones Number of semitones
 * @return Interval name
 */
pitch_interval::IntervalName fromSemitones(int semitones);

/**
 * Find interval between two notes
 * @param from First note
 * @param to Second note
 * @return Interval name
 */
inline pitch_interval::IntervalName distance(const std::string& from, const std::string& to) {
    return pitch_distance::distance(from, to);
}

/**
 * Adds two intervals
 * @param a First interval
 * @param b Second interval
 * @return Added interval name
 */
pitch_interval::IntervalName add(const pitch_interval::IntervalName& a, const pitch_interval::IntervalName& b);

/**
 * Returns a function that adds an interval
 * @param interval Interval to add
 * @return Function that adds the interval
 */
std::function<pitch_interval::IntervalName(const pitch_interval::IntervalName&)> addTo(const std::string& interval);

/**
 * Subtracts two intervals
 * @param minuend Interval to subtract from
 * @param subtrahend Interval to subtract
 * @return Subtracted interval name
 */
pitch_interval::IntervalName subtract(const pitch_interval::IntervalName& minuend, const pitch_interval::IntervalName& subtrahend);

/**
 * Transpose an interval by a number of fifths
 * @param interval Interval name
 * @param fifths Number of fifths to transpose
 * @return Transposed interval name
 */
pitch_interval::IntervalName transposeFifths(const pitch_interval::IntervalName& interval, int fifths);

} // namespace interval
} // namespace tonalcpp