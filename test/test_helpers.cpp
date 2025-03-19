#include "test_helpers.h"
#include <cmath>

namespace tonalcpp {
namespace test {

std::vector<std::string> split(const std::string& s) {
    std::vector<std::string> result;
    std::string current;
    
    for (char c : s) {
        if (c == ' ') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

bool approxEqual(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

} // namespace test
} // namespace tonalcpp