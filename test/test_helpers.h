#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "tonalcpp/helpers.h"
#include <vector>
#include <string>

namespace tonalcpp {
namespace test {

// Helper function to check if two doubles are approximately equal
bool approxEqual(double a, double b, double epsilon = 0.0001);

} // namespace test

// Import helpers directly into the tonalcpp namespace for compatibility with existing tests
using test::approxEqual;
using helpers::split; // Use the centralized split function

} // namespace tonalcpp

#endif // TEST_HELPERS_H