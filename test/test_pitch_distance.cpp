#include "../doctest/doctest.h"
#include "tonalcpp/pitch_distance.h"
#include "tonalcpp/pitch_note.h"
#include "tonalcpp/pitch_interval.h"
#include "test_helpers.h"
#include <vector>
#include <string>

using namespace tonalcpp;

TEST_CASE("@tonaljs/pitch-distance") {
    SUBCASE("transpose") {
        CHECK(pitch_distance::transpose("C3", "3M") == "E3");
        CHECK(pitch_distance::transpose("D", "3M") == "F#");
        
        // Test with empty note or interval
        CHECK(pitch_distance::transpose("", "3M") == "");
        CHECK(pitch_distance::transpose("C3", "") == "");
        
        // Test with array input
        pitch_note::Note c3 = pitch_note::note("C3");
        pitch_interval::Interval maj3 = pitch_interval::interval("3M");
        CHECK(pitch_distance::transpose(c3, maj3) == "E3");
        
        // Test pitch classes (without octave)
        std::vector<std::string> notes = {"C", "D", "E", "F", "G"};
        std::vector<std::string> expected = {"E", "F#", "G#", "A", "B"};
        
        for (size_t i = 0; i < notes.size(); ++i) {
            CHECK(pitch_distance::transpose(notes[i], "3M") == expected[i]);
        }
        
        // Test with coordinate-based transposition using [fifths, octaves]
        // These tests reflect how the PitchCoordinates system works in the TypeScript code
        
        // Basic unison and octave tests
        CHECK(pitch_distance::transpose("C3", std::vector<int>{0, 0}) == "C3");       // Unison
        CHECK(pitch_distance::transpose("C3", std::vector<int>{0, 1}) == "C4");       // Octave up
        CHECK(pitch_distance::transpose("C3", std::vector<int>{0, -1}) == "C2");      // Octave down
        
        // Basic fifths - understanding the coordinate system
        CHECK(pitch_distance::transpose("C3", std::vector<int>{1, 0}) == "G3");       // 1 fifth up = G
        CHECK(pitch_distance::transpose("C3", std::vector<int>{2, 0}) == "D4");       // 2 fifths up = D (with octave shift)
        CHECK(pitch_distance::transpose("C3", std::vector<int>{3, 0}) == "A4");       // 3 fifths up = A (with octave shift)
        CHECK(pitch_distance::transpose("C3", std::vector<int>{4, 0}) == "E5");       // 4 fifths up = E (with octave shift)
        
        CHECK(pitch_distance::transpose("C3", std::vector<int>{-1, 0}) == "F2");      // 1 fifth down = F
        CHECK(pitch_distance::transpose("C3", std::vector<int>{-2, 0}) == "Bb1");     // 2 fifths down = Bb (with octave shift)
        CHECK(pitch_distance::transpose("C3", std::vector<int>{-3, 0}) == "Eb1");     // 3 fifths down = Eb (with octave shift)
        CHECK(pitch_distance::transpose("C3", std::vector<int>{-4, 0}) == "Ab0");     // 4 fifths down = Ab (with octave shift)
        
        // Mixed coordinates - demonstrating combined fifths and octave shifts
        CHECK(pitch_distance::transpose("C3", std::vector<int>{1, 1}) == "G4");       // Perfect fifth up + octave up
        CHECK(pitch_distance::transpose("C3", std::vector<int>{-1, -1}) == "F1");     // Perfect fifth down + octave down
        
        // Offsetting the octave shift caused by fifths
        CHECK(pitch_distance::transpose("C3", std::vector<int>{2, -1}) == "D3");      // 2 fifths up - 1 octave = D3 instead of D4
        CHECK(pitch_distance::transpose("C3", std::vector<int>{-2, 1}) == "Bb2");     // 2 fifths down + 1 octave = Bb2 instead of Bb1
        
        // Using just the pitch class (no octave)
        CHECK(pitch_distance::transpose("C", std::vector<int>{4, 0}) == "E");         // 4 fifths up (no octave in input)
        CHECK(pitch_distance::transpose("C", std::vector<int>{-4, 0}) == "Ab");       // 4 fifths down (no octave in input)
        
        // Verify edge cases
        // Moving 12 fifths in this coordinate system leads to a B# in a different octave
        CHECK(pitch_distance::transpose("C3", std::vector<int>{12, 0}) == "B#9");     
        // Moving -12 fifths leads to a double-flat D in a negative octave
        CHECK(pitch_distance::transpose("C3", std::vector<int>{-12, 0}) == "Dbb-4");  
        
        // Test backward compatibility through global namespace functions
        CHECK(transpose("C3", "3M") == "E3");
        CHECK(transpose("D", "3M") == "F#");
    }
    
    SUBCASE("distance") {
        // Test from C3 to various notes
        std::vector<std::string> toNotes = {"C3", "E3", "E4", "C2", "E2"};
        std::vector<std::string> expectedIntervals = {"1P", "3M", "10M", "-8P", "-6m"};
        
        for (size_t i = 0; i < toNotes.size(); ++i) {
            CHECK(pitch_distance::distance("C3", toNotes[i]) == expectedIntervals[i]);
        }
        
        // Test unison interval edge case (#243)
        CHECK(pitch_distance::distance("Db4", "C#5") == "7A");
        CHECK(pitch_distance::distance("Db4", "C#4") == "-2d");
        CHECK(pitch_distance::distance("Db", "C#") == "7A");
        CHECK(pitch_distance::distance("C#", "Db") == "2d");
        
        // Test adjacent octaves (#428)
        CHECK(pitch_distance::distance("B#4", "C4") == "-7A");
        CHECK(pitch_distance::distance("B#4", "C6") == "9d");
        CHECK(pitch_distance::distance("B#4", "C5") == "2d");
        CHECK(pitch_distance::distance("B##4", "C#5") == "2d");
        CHECK(pitch_distance::distance("B#5", "C6") == "2d");
        
        // Test that intervals between pitch classes are always ascending
        CHECK(pitch_distance::distance("C", "D") == "2M");
        
        std::vector<std::string> fromC = {"C", "D", "E", "F", "G", "A", "B"};
        std::vector<std::string> expectedFromC = {"1P", "2M", "3M", "4P", "5P", "6M", "7M"};
        
        for (size_t i = 0; i < fromC.size(); ++i) {
            CHECK(pitch_distance::distance("C", fromC[i]) == expectedFromC[i]);
        }
        
        std::vector<std::string> fromG = {"C", "D", "E", "F", "G", "A", "B"};
        std::vector<std::string> expectedFromG = {"4P", "5P", "6M", "7m", "1P", "2M", "3M"};
        
        for (size_t i = 0; i < fromG.size(); ++i) {
            CHECK(pitch_distance::distance("G", fromG[i]) == expectedFromG[i]);
        }
        
        // Test that if a note is a pitch class, the distance is between pitch classes
        CHECK(pitch_distance::distance("C", "C2") == "1P");
        CHECK(pitch_distance::distance("C2", "C") == "1P");
        
        // Test with invalid notes
        CHECK(pitch_distance::distance("one", "two") == "");
        
        // Test backward compatibility through global namespace functions
        CHECK(distance("C3", "E3") == "3M");
        CHECK(distance("C", "G") == "5P");
    }
    
    SUBCASE("tonicIntervalsTransposer") {
        std::vector<std::string> intervals = {"1P", "3M", "5P"};
        std::string tonic = "C";
        
        auto result = pitch_distance::tonicIntervalsTransposer(intervals, tonic);
        std::vector<std::string> expected = {"C", "E", "G"};
        
        CHECK(result.size() == expected.size());
        for (size_t i = 0; i < result.size(); ++i) {
            CHECK(result[i] == expected[i]);
        }
        
        // Test with another tonic
        tonic = "D";
        result = pitch_distance::tonicIntervalsTransposer(intervals, tonic);
        expected = {"D", "F#", "A"};
        
        CHECK(result.size() == expected.size());
        for (size_t i = 0; i < result.size(); ++i) {
            CHECK(result[i] == expected[i]);
        }
        
        // Test with empty tonic
        tonic = "";
        result = pitch_distance::tonicIntervalsTransposer(intervals, tonic);
        CHECK(result.empty());
        
        // Test backward compatibility through global namespace functions
        result = tonicIntervalsTransposer(intervals, "C");
        expected = {"C", "E", "G"};
        
        CHECK(result.size() == expected.size());
        for (size_t i = 0; i < result.size(); ++i) {
            CHECK(result[i] == expected[i]);
        }
    }
}