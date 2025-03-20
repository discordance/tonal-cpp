#pragma once

#include <string>
#include <vector>

namespace tonalcpp {
namespace helpers {

/**
 * Split a string by spaces, matching JavaScript's split() behavior
 * which includes empty tokens when there are consecutive delimiters
 * 
 * @param str The string to split
 * @return Vector of tokens
 */
std::vector<std::string> split(const std::string& str);

} // namespace helpers
} // namespace tonalcpp