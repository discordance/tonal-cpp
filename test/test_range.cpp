#include "doctest.h"
#include "tonalcpp/range.h"
#include <vector>
#include <string>

using namespace tonalcpp::range;

TEST_CASE("Range numeric") {
    SUBCASE("special cases") {
        CHECK(numeric({}).empty());
        CHECK(numeric({"C4"}) == std::vector<int>{60});
    }
    
    SUBCASE("notes with names") {
        std::vector<int> expected1 = {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72};
        CHECK(numeric({"C4", "C5"}) == expected1);
        
        std::vector<int> expected2 = {72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60};
        CHECK(numeric({"C5", "C4"}) == expected2);
    }
    
    SUBCASE("multiple notes") {
        std::vector<int> expected = {36, 37, 38, 39, 40, 41, 40, 39, 38, 37, 36, 35, 34, 35, 36};
        CHECK(numeric({"C2", "F2", "Bb1", "C2"}) == expected);
    }
}

TEST_CASE("Range chromatic") {
    SUBCASE("note names") {
        std::vector<std::string> expected1 = {"A3", "Bb3", "B3", "C4", "Db4", "D4", "Eb4", "E4", "F4", "Gb4", "G4", "Ab4", "A4"};
        CHECK(chromatic({"A3", "A4"}) == expected1);
        
        std::vector<std::string> expected2 = {"A4", "Ab4", "G4", "Gb4", "F4", "E4", "Eb4", "D4", "Db4", "C4", "B3", "Bb3", "A3"};
        CHECK(chromatic({"A4", "A3"}) == expected2);
        
        std::vector<std::string> expected3 = {"C3", "Db3", "D3", "Eb3", "D3", "Db3", "C3", "B2", "Bb2", "A2"};
        CHECK(chromatic({"C3", "Eb3", "A2"}) == expected3);
    }
    
    SUBCASE("chromatic with sharps") {
        std::vector<std::string> expected = {"C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2", "C3"};
        CHECK(chromatic({"C2", "C3"}, true) == expected);
    }
}