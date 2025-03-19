#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <vector>
#include <string>

namespace tonalcpp {
namespace test {

// Helper function to split a string into a vector of strings by spaces
std::vector<std::string> split(const std::string& s);

// Helper function to check if two doubles are approximately equal
bool approxEqual(double a, double b, double epsilon = 0.0001);

} // namespace test

// Import helpers directly into the tonalcpp namespace for compatibility with existing tests
using test::split;
using test::approxEqual;

} // namespace tonalcpp

#endif // TEST_HELPERS_H