#pragma once

#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <random>

namespace tonalcpp {
namespace collection {

/**
 * Creates a numeric range
 * 
 * @param from Starting number
 * @param to Ending number (inclusive)
 * @return Vector of numbers in the range
 *
 * @example
 * range(-2, 2) // => [-2, -1, 0, 1, 2]
 * range(2, -2) // => [2, 1, 0, -1, -2]
 */
std::vector<int> range(int from, int to);

/**
 * Rotates a vector a number of times.
 * 
 * @param times The number of rotations
 * @param vec The vector to rotate
 * @return The rotated vector
 *
 * @example
 * rotate(1, {1, 2, 3}) // => {2, 3, 1}
 */
template<typename T>
std::vector<T> rotate(int times, const std::vector<T>& vec) {
    if (vec.empty()) return vec;
    
    const size_t len = vec.size();
    const int n = ((times % len) + len) % len;
    
    std::vector<T> result;
    result.reserve(len);
    
    // Add elements from n to end
    result.insert(result.end(), vec.begin() + n, vec.end());
    
    // Add elements from start to n
    result.insert(result.end(), vec.begin(), vec.begin() + n);
    
    return result;
}

/**
 * Return a copy of the vector with nullopt/empty values removed
 * 
 * @param vec The vector to compact
 * @return Vector with null values removed
 *
 * @example
 * compact({"a", "b", "", "c"}) // => {"a", "b", "c"}
 */
template<typename T>
std::vector<T> compact(const std::vector<T>& vec) {
    std::vector<T> result;
    result.reserve(vec.size());
    
    for (const auto& item : vec) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

/**
 * Randomizes the order of the specified vector in-place, using the Fisher–Yates shuffle.
 * 
 * @param vec The vector to shuffle
 * @param rnd Random number generator (optional)
 * @return The shuffled vector
 *
 * @example
 * shuffle({"C", "D", "E", "F"}) // => [...]
 */
template<typename T>
std::vector<T> shuffle(std::vector<T> vec, 
                        std::function<double()> rnd = []() { return std::rand() / (RAND_MAX + 1.0); }) {
    int m = vec.size();
    while (m) {
        int i = static_cast<int>(std::floor(rnd() * m--));
        std::swap(vec[m], vec[i]);
    }
    return vec;
}

/**
 * Get all permutations of a vector
 * 
 * @param vec The input vector
 * @return Vector containing all permutations
 */
template<typename T>
std::vector<std::vector<T>> permutations(const std::vector<T>& vec) {
    std::vector<std::vector<T>> result;
    std::vector<T> sortedVec = vec;
    
    // Use C++ standard library's next_permutation
    // First sort to get all permutations
    std::sort(sortedVec.begin(), sortedVec.end());
    
    do {
        result.push_back(sortedVec);
    } while (std::next_permutation(sortedVec.begin(), sortedVec.end()));
    
    return result;
}

} // namespace collection
} // namespace tonalcpp