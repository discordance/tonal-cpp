#include "doctest.h"
#include "tonalcpp/interval.h"
#include <sstream>

using namespace tonalcpp;

TEST_CASE("interval - properties") {
    auto i = interval::get("P4");
    
    CHECK(i.alt == 0);
    CHECK(i.chroma == 5);
    CHECK(i.coord[0] == -1);
    CHECK(i.coord[1] == 1);
    CHECK(i.dir != std::nullopt);
    CHECK(i.empty == false);
    CHECK(i.name == "4P");
    CHECK(i.num == 4);
    CHECK(i.oct == 0);
    CHECK(i.q == "P");
    CHECK(i.semitones == 5);
    CHECK(i.simple == 4);
    CHECK(i.step == 3);
    CHECK(i.type == pitch_interval::IntervalType::Perfectable);
}

TEST_CASE("interval - shorthand properties") {
    CHECK(interval::name("d5") == "5d");
    CHECK(interval::num("d5") == 5);
    CHECK(interval::quality("d5") == "d");
    CHECK(interval::semitones("d5") == 6);
}

TEST_CASE("interval - distance") {
    CHECK(interval::distance("C4", "G4") == "5P");
}

TEST_CASE("interval - names") {
    auto expectedNames = std::vector<std::string>{"1P", "2M", "3M", "4P", "5P", "6m", "7m"};
    CHECK(interval::names() == expectedNames);
}

TEST_CASE("interval - simplify intervals") {
    std::vector<std::string> intervals = {"1P", "2M", "3M", "4P", "5P", "6M", "7M"};
    std::vector<std::string> simplified;
    
    for (const auto& i : intervals) {
        simplified.push_back(interval::simplify(i));
    }
    
    std::vector<std::string> expected = {"1P", "2M", "3M", "4P", "5P", "6M", "7M"};
    CHECK(simplified == expected);
    
    simplified.clear();
    intervals = {"8P", "9M", "10M", "11P", "12P", "13M", "14M"};
    for (const auto& i : intervals) {
        simplified.push_back(interval::simplify(i));
    }
    
    expected = {"8P", "2M", "3M", "4P", "5P", "6M", "7M"};
    CHECK(simplified == expected);
    
    simplified.clear();
    intervals = {"1d", "1P", "1A", "8d", "8P", "8A", "15d", "15P", "15A"};
    for (const auto& i : intervals) {
        simplified.push_back(interval::simplify(i));
    }
    
    expected = {"1d", "1P", "1A", "8d", "8P", "8A", "1d", "1P", "1A"};
    CHECK(simplified == expected);
    
    simplified.clear();
    intervals = {"-1P", "-2M", "-3M", "-4P", "-5P", "-6M", "-7M"};
    for (const auto& i : intervals) {
        simplified.push_back(interval::simplify(i));
    }
    
    expected = {"-1P", "-2M", "-3M", "-4P", "-5P", "-6M", "-7M"};
    CHECK(simplified == expected);
    
    simplified.clear();
    intervals = {"-8P", "-9M", "-10M", "-11P", "-12P", "-13M", "-14M"};
    for (const auto& i : intervals) {
        simplified.push_back(interval::simplify(i));
    }
    
    expected = {"-8P", "-2M", "-3M", "-4P", "-5P", "-6M", "-7M"};
    CHECK(simplified == expected);
}

TEST_CASE("interval - invert intervals") {
    std::vector<std::string> intervals = {"1P", "2M", "3M", "4P", "5P", "6M", "7M"};
    std::vector<std::string> inverted;
    
    for (const auto& i : intervals) {
        inverted.push_back(interval::invert(i));
    }
    
    std::vector<std::string> expected = {"1P", "7m", "6m", "5P", "4P", "3m", "2m"};
    CHECK(inverted == expected);
    
    inverted.clear();
    intervals = {"1d", "2m", "3m", "4d", "5d", "6m", "7m"};
    for (const auto& i : intervals) {
        inverted.push_back(interval::invert(i));
    }
    
    expected = {"1A", "7M", "6M", "5A", "4A", "3M", "2M"};
    CHECK(inverted == expected);
    
    inverted.clear();
    intervals = {"1A", "2A", "3A", "4A", "5A", "6A", "7A"};
    for (const auto& i : intervals) {
        inverted.push_back(interval::invert(i));
    }
    
    expected = {"1d", "7d", "6d", "5d", "4d", "3d", "2d"};
    CHECK(inverted == expected);
    
    inverted.clear();
    intervals = {"-1P", "-2M", "-3M", "-4P", "-5P", "-6M", "-7M"};
    for (const auto& i : intervals) {
        inverted.push_back(interval::invert(i));
    }
    
    expected = {"-1P", "-7m", "-6m", "-5P", "-4P", "-3m", "-2m"};
    CHECK(inverted == expected);
    
    inverted.clear();
    intervals = {"8P", "9M", "10M", "11P", "12P", "13M", "14M"};
    for (const auto& i : intervals) {
        inverted.push_back(interval::invert(i));
    }
    
    expected = {"8P", "14m", "13m", "12P", "11P", "10m", "9m"};
    CHECK(inverted == expected);
}

TEST_CASE("interval - fromSemitones") {
    std::vector<int> semis = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::vector<std::string> intervals;
    
    for (int semi : semis) {
        intervals.push_back(interval::fromSemitones(semi));
    }
    
    std::vector<std::string> expected = {"1P", "2m", "2M", "3m", "3M", "4P", "5d", "5P", "6m", "6M", "7m", "7M"};
    CHECK(intervals == expected);
    
    intervals.clear();
    semis = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
    for (int semi : semis) {
        intervals.push_back(interval::fromSemitones(semi));
    }
    
    expected = {"8P", "9m", "9M", "10m", "10M", "11P", "12d", "12P", "13m", "13M", "14m", "14M"};
    CHECK(intervals == expected);
    
    intervals.clear();
    semis = {0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11};
    for (int semi : semis) {
        intervals.push_back(interval::fromSemitones(semi));
    }
    
    expected = {"1P", "-2m", "-2M", "-3m", "-3M", "-4P", "-5d", "-5P", "-6m", "-6M", "-7m", "-7M"};
    CHECK(intervals == expected);
    
    intervals.clear();
    semis = {-12, -13, -14, -15, -16, -17, -18, -19, -20, -21, -22, -23};
    for (int semi : semis) {
        intervals.push_back(interval::fromSemitones(semi));
    }
    
    expected = {"-8P", "-9m", "-9M", "-10m", "-10M", "-11P", "-12d", "-12P", "-13m", "-13M", "-14m", "-14M"};
    CHECK(intervals == expected);
}

TEST_CASE("interval - add") {
    CHECK(interval::add("3m", "5P") == "7m");
    
    auto names = interval::names();
    std::vector<std::string> added;
    
    for (const auto& n : names) {
        added.push_back(interval::add("5P", n));
    }
    
    std::vector<std::string> expected = {"5P", "6M", "7M", "8P", "9M", "10m", "11P"};
    CHECK(added == expected);
    
    added.clear();
    auto addTo5P = interval::addTo("5P");
    for (const auto& n : names) {
        added.push_back(addTo5P(n));
    }
    
    CHECK(added == expected);
}

TEST_CASE("interval - subtract") {
    // Testing subtract with direct interval calls
    // Look at the coords: 
    // 5P = [1, 0]
    // 3M = [2, -1]
    // 5P - 3M = [1, 0] - [2, -1] = [-1, 1] => 3m
    CHECK(interval::subtract("5P", "3M") == "3m");
    
    // 3M - 5P = [2, -1] - [1, 0] = [1, -1] => -3m
    // But wait! [1, -1] doesn't represent -3m directly...
    // The proper coordinate for -3m should be [-1, -1] or similar
    CHECK(interval::subtract("3M", "5P") == "-3m");
    
    auto names = interval::names();
    std::vector<std::string> subtracted;

    for (const auto& n : names) {
        subtracted.push_back(interval::subtract("5P", n));
    }

    std::vector<std::string> expected = {"5P", "4P", "3m", "2M", "1P", "-2m", "-3m"};
    CHECK(subtracted == expected);
}

TEST_CASE("interval - transposeFifths") {
    CHECK(interval::transposeFifths("4P", 1) == "8P");
    
    std::vector<int> fifths = {0, 1, 2, 3, 4};
    std::vector<std::string> transposed;
    
    for (int fifth : fifths) {
        transposed.push_back(interval::transposeFifths("1P", fifth));
    }
    
    std::vector<std::string> expected = {"1P", "5P", "9M", "13M", "17M"};
    CHECK(transposed == expected);
    
    transposed.clear();
    fifths = {0, -1, -2, -3, -4};
    for (int fifth : fifths) {
        transposed.push_back(interval::transposeFifths("1P", fifth));
    }
    
    expected = {"1P", "-5P", "-9M", "-13M", "-17M"};
    CHECK(transposed == expected);
}