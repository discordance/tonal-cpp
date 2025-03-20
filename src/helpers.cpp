#include "tonalcpp/helpers.h"

namespace tonalcpp {
namespace helpers {

std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> tokens;
    std::string token;
    size_t pos = 0;
    
    // If empty string, return empty vector
    if (str.empty()) {
        return tokens;
    }
    
    // Handle the JavaScript split behavior
    size_t found = str.find(' ');
    while (found != std::string::npos) {
        tokens.push_back(str.substr(pos, found - pos));
        pos = found + 1;
        found = str.find(' ', pos);
    }
    
    // Add the remaining part (or empty if ends with delimiter)
    tokens.push_back(str.substr(pos));
    
    return tokens;
}

} // namespace helpers
} // namespace tonalcpp