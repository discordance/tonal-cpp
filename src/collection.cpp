#include "tonalcpp/collection.h"
#include <algorithm>

namespace tonalcpp {

// Ascending range helper
std::vector<int> ascendingRange(int begin, int count) {
    std::vector<int> result(count);
    for (int i = 0; i < count; ++i) {
        result[i] = begin + i;
    }
    return result;
}

// Descending range helper
std::vector<int> descendingRange(int begin, int count) {
    std::vector<int> result(count);
    for (int i = 0; i < count; ++i) {
        result[i] = begin - i;
    }
    return result;
}

std::vector<int> range(int from, int to) {
    return from < to 
        ? ascendingRange(from, to - from + 1) 
        : descendingRange(from, from - to + 1);
}

} // namespace tonalcpp