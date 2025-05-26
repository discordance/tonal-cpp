#pragma once

#include <vector>
#include <string>
#include "tonalcpp/midi.h"

namespace tonalcpp {
namespace range {

/**
 * Create a numeric range. You supply a list of notes or numbers and it will
 * be connected to create complex ranges.
 * 
 * @param notes Vector of note names or midi numbers
 * @return Vector of midi numbers or empty vector if not valid parameters
 * 
 * @example
 * numeric({"C5", "C4"}) // => [ 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60 ]
 * numeric({10, 5}) // => [ 10, 9, 8, 7, 6, 5 ] (when passing midi numbers as strings)
 */
std::vector<int> numeric(const std::vector<std::string>& notes);

/**
 * Create a range of chromatic notes. The altered notes will use flats.
 * 
 * @param notes Vector of note names or midi note numbers to create a range from
 * @param sharps Whether to use sharps instead of flats for altered notes
 * @return Vector of note names
 * 
 * @example
 * chromatic({"C2", "E2", "D2"}) // => {"C2", "Db2", "D2", "Eb2", "E2", "Eb2", "D2"}
 * chromatic({"C2", "C3"}, true) // => {"C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2", "C3"}
 */
std::vector<std::string> chromatic(const std::vector<std::string>& notes, bool sharps = false);

} // namespace range
} // namespace tonalcpp