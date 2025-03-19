#include "../doctest/doctest.h"
#include "tonalcpp/pitch_distance.h"
#include "test_helpers.h"
#include <vector>
#include <string>

using namespace tonalcpp;

TEST_CASE("@tonaljs/pitch-distance") {
    SUBCASE("transpose") {
        CHECK(transpose("C3", "3M") == "E3");
        CHECK(transpose("D", "3M") == "F#");
        
        // Test with empty note or interval
        CHECK(transpose("", "3M") == "");
        CHECK(transpose("C3", "") == "");
        
        // Test with array input
        Note c3 = note("C3");
        Interval maj3 = interval("3M");
        CHECK(transpose(c3, maj3) == "E3");
        
        // Test pitch classes (without octave)
        std::vector<std::string> notes = {"C", "D", "E", "F", "G"};
        std::vector<std::string> expected = {"E", "F#", "G#", "A", "B"};
        
        for (size_t i = 0; i < notes.size(); ++i) {
            CHECK(transpose(notes[i], "3M") == expected[i]);
        }
        
        // Test with coordinate-based transposition using [fifths, octaves]
        // These tests reflect how the PitchCoordinates system works in the TypeScript code
        
        // Basic unison and octave tests
        CHECK(transpose("C3", std::vector<int>{0, 0}) == "C3");       // Unison
        CHECK(transpose("C3", std::vector<int>{0, 1}) == "C4");       // Octave up
        CHECK(transpose("C3", std::vector<int>{0, -1}) == "C2");      // Octave down
        
        // Basic fifths - understanding the coordinate system
        CHECK(transpose("C3", std::vector<int>{1, 0}) == "G3");       // 1 fifth up = G
        CHECK(transpose("C3", std::vector<int>{2, 0}) == "D4");       // 2 fifths up = D (with octave shift)
        CHECK(transpose("C3", std::vector<int>{3, 0}) == "A4");       // 3 fifths up = A (with octave shift)
        CHECK(transpose("C3", std::vector<int>{4, 0}) == "E5");       // 4 fifths up = E (with octave shift)
        
        CHECK(transpose("C3", std::vector<int>{-1, 0}) == "F2");      // 1 fifth down = F
        CHECK(transpose("C3", std::vector<int>{-2, 0}) == "Bb1");     // 2 fifths down = Bb (with octave shift)
        CHECK(transpose("C3", std::vector<int>{-3, 0}) == "Eb1");     // 3 fifths down = Eb (with octave shift)
        CHECK(transpose("C3", std::vector<int>{-4, 0}) == "Ab0");     // 4 fifths down = Ab (with octave shift)
        
        // Mixed coordinates - demonstrating combined fifths and octave shifts
        CHECK(transpose("C3", std::vector<int>{1, 1}) == "G4");       // Perfect fifth up + octave up
        CHECK(transpose("C3", std::vector<int>{-1, -1}) == "F1");     // Perfect fifth down + octave down
        
        // Offsetting the octave shift caused by fifths
        CHECK(transpose("C3", std::vector<int>{2, -1}) == "D3");      // 2 fifths up - 1 octave = D3 instead of D4
        CHECK(transpose("C3", std::vector<int>{-2, 1}) == "Bb2");     // 2 fifths down + 1 octave = Bb2 instead of Bb1
        
        // Using just the pitch class (no octave)
        CHECK(transpose("C", std::vector<int>{4, 0}) == "E");         // 4 fifths up (no octave in input)
        CHECK(transpose("C", std::vector<int>{-4, 0}) == "Ab");       // 4 fifths down (no octave in input)
        
        // Verify edge cases
        // Moving 12 fifths in this coordinate system leads to a B# in a different octave
        CHECK(transpose("C3", std::vector<int>{12, 0}) == "B#9");     
        // Moving -12 fifths leads to a double-flat D in a negative octave
        CHECK(transpose("C3", std::vector<int>{-12, 0}) == "Dbb-4");  
    }
    
    SUBCASE("distance") {
        // Test from C3 to various notes
        std::vector<std::string> toNotes = {"C3", "E3", "E4", "C2", "E2"};
        std::vector<std::string> expectedIntervals = {"1P", "3M", "10M", "-8P", "-6m"};
        
        for (size_t i = 0; i < toNotes.size(); ++i) {
            CHECK(distance("C3", toNotes[i]) == expectedIntervals[i]);
        }
        
        // Test unison interval edge case (#243)
        CHECK(distance("Db4", "C#5") == "7A");
        CHECK(distance("Db4", "C#4") == "-2d");
        CHECK(distance("Db", "C#") == "7A");
        CHECK(distance("C#", "Db") == "2d");
        
        // Test adjacent octaves (#428)
        CHECK(distance("B#4", "C4") == "-7A");
        CHECK(distance("B#4", "C6") == "9d");
        CHECK(distance("B#4", "C5") == "2d");
        CHECK(distance("B##4", "C#5") == "2d");
        CHECK(distance("B#5", "C6") == "2d");
        
        // Test that intervals between pitch classes are always ascending
        CHECK(distance("C", "D") == "2M");
        
        std::vector<std::string> fromC = {"C", "D", "E", "F", "G", "A", "B"};
        std::vector<std::string> expectedFromC = {"1P", "2M", "3M", "4P", "5P", "6M", "7M"};
        
        for (size_t i = 0; i < fromC.size(); ++i) {
            CHECK(distance("C", fromC[i]) == expectedFromC[i]);
        }
        
        std::vector<std::string> fromG = {"C", "D", "E", "F", "G", "A", "B"};
        std::vector<std::string> expectedFromG = {"4P", "5P", "6M", "7m", "1P", "2M", "3M"};
        
        for (size_t i = 0; i < fromG.size(); ++i) {
            CHECK(distance("G", fromG[i]) == expectedFromG[i]);
        }
        
        // Test that if a note is a pitch class, the distance is between pitch classes
        CHECK(distance("C", "C2") == "1P");
        CHECK(distance("C2", "C") == "1P");
        
        // Test with invalid notes
        CHECK(distance("one", "two") == "");
    }
    
    SUBCASE("tonicIntervalsTransposer") {
        std::vector<std::string> intervals = {"1P", "3M", "5P"};
        std::string tonic = "C";
        
        auto result = tonicIntervalsTransposer(intervals, tonic);
        std::vector<std::string> expected = {"C", "E", "G"};
        
        CHECK(result.size() == expected.size());
        for (size_t i = 0; i < result.size(); ++i) {
            CHECK(result[i] == expected[i]);
        }
        
        // Test with another tonic
        tonic = "D";
        result = tonicIntervalsTransposer(intervals, tonic);
        expected = {"D", "F#", "A"};
        
        CHECK(result.size() == expected.size());
        for (size_t i = 0; i < result.size(); ++i) {
            CHECK(result[i] == expected[i]);
        }
        
        // Test with empty tonic
        tonic = "";
        result = tonicIntervalsTransposer(intervals, tonic);
        CHECK(result.empty());
    }
}