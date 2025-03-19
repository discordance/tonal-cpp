#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include "tonalcpp/pitch.h"

namespace tonalcpp {
namespace pitch_interval {

/**
 * Interval type representation
 */
enum class IntervalType {
    Perfectable,    // For intervals that can be perfect (1, 4, 5, 8)
    Majorable,      // For intervals that can be major/minor (2, 3, 6, 7)
    Unknown
};

/**
 * String typedefs to match TypeScript
 */
using IntervalName = std::string;
using Quality = std::string;

/**
 * Interval struct which extends Pitch with interval-specific properties
 */
struct Interval : public pitch::Pitch {
    bool empty;                          // Whether this is a valid interval
    IntervalName name;                   // The interval name
    int num;                             // The interval number (1, 2, 3, etc.)
    Quality q;                           // The interval quality string (P, M, m, d, A, etc.)
    IntervalType type;                   // The interval type (perfectable or majorable)
    int simple;                          // Simplified interval number
    int semitones;                       // The number of semitones
    int chroma;                          // The chromatic number (0-11)
    int oct;                             // Octave component
    pitch::IntervalCoordinates coord;    // The interval coordinates

    // Default constructor that creates an empty interval
    Interval();

    // Full constructor
    Interval(bool emp, const IntervalName& n, int nu, const Quality& qu, IntervalType ty, 
             int s, int a, int d, int sim, int semi, int ch, int o, 
             const pitch::IntervalCoordinates& co);
};

/**
 * Invalid interval constant
 */
extern const Interval NoInterval;

/**
 * Parse a string to an interval
 * @param src Interval string or Pitch
 * @return Interval object
 */
Interval interval(const std::string& src);

/**
 * Parse a string to an interval 
 * @param src Interval string
 * @param useCache Whether to use the cache for parsed intervals
 * @return Interval object
 */
Interval interval(const std::string& src, bool useCache);

/**
 * Tokenize an interval string into components
 * @param str Interval string
 * @return Pair of [number, quality]
 */
std::pair<std::string, std::string> tokenizeInterval(const std::string& str);

/**
 * Convert pitch properties to quality string
 * @param type The interval type (perfectable or majorable)
 * @param alt The alteration value
 * @return The quality string
 */
Quality altToQ(IntervalType type, int alt);

/**
 * Convert quality string to alteration
 * @param type The interval type (perfectable or majorable)
 * @param quality The quality string
 * @return The alteration value
 */
int qToAlt(IntervalType type, const Quality& quality);

/**
 * Convert coordinates to an interval
 * @param coord Pitch coordinates
 * @param forceDescending Force interval to be descending
 * @return Interval object
 */
Interval coordToInterval(const pitch::PitchCoordinates& coord, bool forceDescending = false);

/**
 * Generate interval name from pitch properties
 * @param props Pitch properties
 * @return Interval name
 */
std::string intervalPitchName(const pitch::Pitch& props);

} // namespace pitch_interval
} // namespace tonalcpp