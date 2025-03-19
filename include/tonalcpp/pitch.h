#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>
#include <cstdint>

namespace tonalcpp {
namespace pitch {

/**
 * Direction of an interval (ascending or descending)
 */
enum class Direction {
    Ascending = 1,
    Descending = -1
};

/**
 * Coordinate representations
 */
using PitchClassCoordinates = std::array<int, 1>;
using NoteCoordinates = std::array<int, 2>;
using IntervalCoordinates = std::array<int, 3>;
using PitchCoordinates = std::vector<int>;

/**
 * NamedPitch interface for objects with a name representation
 */
struct NamedPitch {
    std::string name;
};

/**
 * The core Pitch struct - represents various musical entities:
 * - Pitch classes (like C, D#, Bb) when oct is nullopt
 * - Notes (like C4, F#3) when oct has a value and dir is nullopt
 * - Intervals (like P5, m3) when dir has a value
 */
struct Pitch : public NamedPitch {
    int step;                       // The step number: 0 = C, 1 = D, ... 6 = B
    int alt;                        // Number of alterations: -2 = 'bb', -1 = 'b', 0 = '', 1 = '#', ...
    std::optional<int> oct;         // The octave (nullopt for pitch classes)
    std::optional<Direction> dir;   // Interval direction (nullopt for notes)
    
    // Default constructor
    Pitch() : NamedPitch{""}  {
        step = 0;
        alt = 0;
        oct = std::nullopt;
        dir = std::nullopt;
    }
    
    // Full constructor
    Pitch(int s, int a, std::optional<int> o, std::optional<Direction> d, std::string n = "") 
        : NamedPitch{n}, step(s), alt(a), oct(o), dir(d) {}
};

/**
 * NotFound represents an entity that couldn't be found/parsed
 */
struct NotFound : public NamedPitch {
    bool empty = true;
    NotFound() : NamedPitch{""} {}
};

/**
 * Type checking utilities - only the void* versions are used by tests
 */
#include <string>
#include <typeinfo>

// Special void* test versions for compatibility
bool isNamedPitch(const void* src);
bool isPitch(const void* src);

/**
 * Computes the pitch class (0-11) of the given pitch
 * @param pitch The pitch to get the chroma for
 * @return The chroma (0-11)
 */
int chroma(const Pitch& pitch);

/**
 * Computes the absolute height of a pitch
 * - For pitch classes: it's relative to a reference point
 * - For notes: it's the absolute position in semitones
 * - For intervals: it's the number of semitones
 * 
 * @param pitch The pitch to get the height for
 * @return The height in semitones
 */
int height(const Pitch& pitch);

/**
 * Get the MIDI note number for a pitch if it's in the valid MIDI range (0-127)
 * @param pitch The pitch to get the MIDI note for
 * @return The MIDI note number or nullopt if out of range
 */
std::optional<int> midi(const Pitch& pitch);

/**
 * Convert a pitch to its coordinate representation
 * @param pitch The pitch to convert
 * @return Coordinate representation
 */
PitchCoordinates coordinates(const Pitch& pitch);

/**
 * Convert coordinates back to a pitch
 * @param coord The coordinates to convert
 * @return The pitch
 */
Pitch pitchFromCoordinates(const PitchCoordinates& coord);

} // namespace pitch
} // namespace tonalcpp