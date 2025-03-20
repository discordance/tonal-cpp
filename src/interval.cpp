#include "tonalcpp/interval.h"
#include <algorithm>
#include <cmath>

namespace tonalcpp {
namespace interval {

// Interval numbers
const std::vector<int> IN = {1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7};
// Interval qualities
const std::vector<std::string> IQ = {"P", "m", "M", "m", "M", "P", "d", "P", "m", "M", "m", "M"};

std::vector<pitch_interval::IntervalName> names() {
    return {"1P", "2M", "3M", "4P", "5P", "6m", "7m"};
}

std::string name(const std::string& name) {
    return get(name).name;
}

int semitones(const std::string& name) {
    return get(name).semitones;
}

pitch_interval::Quality quality(const std::string& name) {
    return get(name).q;
}

int num(const std::string& name) {
    return get(name).num;
}

pitch_interval::IntervalName simplify(const pitch_interval::IntervalName& name) {
    auto i = get(name);
    if (i.empty) {
        return "";
    }
    return std::to_string(i.simple) + i.q;
}

pitch_interval::IntervalName invert(const pitch_interval::IntervalName& name) {
    auto i = get(name);
    if (i.empty) {
        return "";
    }
    int step = (7 - i.step) % 7;
    int alt;
    if (i.type == pitch_interval::IntervalType::Perfectable) {
        alt = -i.alt;
    } else {
        alt = -(i.alt + 1);
    }
    
    pitch::Pitch p;
    p.step = step;
    p.alt = alt;
    p.oct = i.oct;
    // Preserve the same direction
    p.dir = i.dir;
    
    return pitch_interval::intervalPitchName(p);
}

pitch_interval::IntervalName fromSemitones(int semitones) {
    int d = semitones < 0 ? -1 : 1;
    int n = std::abs(semitones);
    int c = n % 12;
    int o = n / 12;
    return std::to_string(d * (IN[c] + 7 * o)) + IQ[c];
}

// Helper function for add and subtract operations
using Operation = std::function<pitch::PitchCoordinates(const pitch::IntervalCoordinates&, const pitch::IntervalCoordinates&)>;

pitch_interval::IntervalName combinator(Operation fn, const pitch_interval::IntervalName& a, const pitch_interval::IntervalName& b) {
    auto coordA = get(a).coord;
    auto coordB = get(b).coord;
    
    // Check if coordinates are valid (non-empty)
    if (coordA.size() > 0 && coordB.size() > 0) {
        auto coord = fn(coordA, coordB);

        return pitch_interval::coordToInterval(coord).name;
    }
    return "";
}

pitch_interval::IntervalName add(const pitch_interval::IntervalName& a, const pitch_interval::IntervalName& b) {
    auto addOperation = [](const pitch::IntervalCoordinates& a, const pitch::IntervalCoordinates& b) -> pitch::PitchCoordinates {
        // Simply return the sum of fifths and octaves
        // The coordToInterval function will determine direction based on the resulting values
        return {a[0] + b[0], a[1] + b[1]};
    };
    return combinator(addOperation, a, b);
}

std::function<pitch_interval::IntervalName(const pitch_interval::IntervalName&)> addTo(const std::string& interval) {
    return [interval](const pitch_interval::IntervalName& other) {
        return add(interval, other);
    };
}

pitch_interval::IntervalName subtract(const pitch_interval::IntervalName& minuend, const pitch_interval::IntervalName& subtrahend) {
    auto subtractOperation = [](const pitch::IntervalCoordinates& a, const pitch::IntervalCoordinates& b) -> pitch::PitchCoordinates {
        // Simply return the fifths and octaves difference
        // The coordToInterval function will determine direction based on the resulting values
        return {a[0] - b[0], a[1] - b[1]};
    };
    return combinator(subtractOperation, minuend, subtrahend);
}

pitch_interval::IntervalName transposeFifths(const pitch_interval::IntervalName& interval, int fifths) {
    auto ivl = get(interval);
    if (ivl.empty) {
        return "";
    }
    
    // Extract the coordinates components
    int nFifths = ivl.coord[0];
    int nOcts = ivl.coord[1];
    int dir = ivl.coord[2];
    
    // Create new coordinates with the fifths added
    pitch::PitchCoordinates newCoord = {nFifths + fifths, nOcts, dir};
    
    return pitch_interval::coordToInterval(newCoord).name;
}

} // namespace interval
} // namespace tonalcpp