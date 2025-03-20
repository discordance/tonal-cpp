#include "test_helpers.h"
#include <cmath>

namespace tonalcpp {
namespace test {

bool approxEqual(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

} // namespace test
} // namespace tonalcpp