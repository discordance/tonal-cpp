#include "doctest.h"
#include "tonalcpp/midi.h"
#include "test_helpers.h"
#include <vector>
#include <string>
#include <optional>

using namespace tonalcpp;
using namespace tonalcpp::midi;
using namespace tonalcpp::test;

TEST_CASE("MIDI") {
    SUBCASE("isMidi") {
        CHECK(isMidi(100) == true);
        CHECK(isMidi(0) == true);
        CHECK(isMidi(127) == true);
        CHECK(isMidi(-1) == false);
        CHECK(isMidi(128) == false);
    }

    SUBCASE("toMidi") {
        auto midi100 = toMidi(100);
        CHECK(midi100.has_value());
        CHECK(midi100.value() == 100);
        
        auto midiC4 = toMidi("C4");
        CHECK(midiC4.has_value());
        CHECK(midiC4.value() == 60);
        
        auto midi60 = toMidi("60");
        CHECK(midi60.has_value());
        CHECK(midi60.value() == 60);
        
        auto midi0 = toMidi(0);
        CHECK(midi0.has_value());
        CHECK(midi0.value() == 0);
        
        auto midiStr0 = toMidi("0");
        CHECK(midiStr0.has_value());
        CHECK(midiStr0.value() == 0);
        
        // Invalid values
        CHECK(!toMidi(-1).has_value());
        CHECK(!toMidi(128).has_value());
        CHECK(!toMidi("blah").has_value());
    }

    SUBCASE("freqToMidi") {
        // Test with exact values from the TS implementation
        CHECK(freqToMidi(220.0) == 57.0);
        CHECK(freqToMidi(261.62) == 60.0);
        CHECK(freqToMidi(261.0) == 59.96);
        
        // Test round-trip with midiToFreq (integer values will be exact)
        int midiValue = 64;
        double freq = midiToFreq(midiValue);
        CHECK(approxEqual(freqToMidi(freq), midiValue, 0.01));
    }

    SUBCASE("midiToFreq") {
        CHECK(approxEqual(midiToFreq(60), 261.6255653005986));
        CHECK(approxEqual(midiToFreq(69, 443), 443.0));
    }

    SUBCASE("midiToNoteName") {
        std::vector<int> notes = {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72};
        std::vector<std::string> expected = {
            "C4", "Db4", "D4", "Eb4", "E4", "F4", "Gb4", "G4", "Ab4", "A4", "Bb4", "B4", "C5"
        };
        
        for (size_t i = 0; i < notes.size(); i++) {
            CHECK(midiToNoteName(notes[i]) == expected[i]);
        }
        
        // With sharps option
        std::vector<std::string> expectedSharps = {
            "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4", "C5"
        };
        
        for (size_t i = 0; i < notes.size(); i++) {
            ToNoteNameOptions options;
            options.sharps = true;
            CHECK(midiToNoteName(notes[i], options) == expectedSharps[i]);
        }
        
        // With pitchClass option
        std::vector<std::string> expectedPC = {
            "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B", "C"
        };
        
        for (size_t i = 0; i < notes.size(); i++) {
            ToNoteNameOptions options;
            options.pitchClass = true;
            CHECK(midiToNoteName(notes[i], options) == expectedPC[i]);
        }
        
        // Invalid inputs
        // Skip NaN test as it's not directly comparable in C++
        CHECK(midiToNoteName(std::numeric_limits<int>::min()) == "");
        CHECK(midiToNoteName(std::numeric_limits<int>::max()) == "");
    }
    
    SUBCASE("pcset") {
        // From chroma string
        std::vector<int> expected1 = {0, 3, 6, 9, 11};
        std::vector<int> result1 = pcset("100100100101");
        CHECK(result1 == expected1);
        
        // From MIDI numbers
        std::vector<int> input2 = {62, 63, 60, 65, 70, 72};
        std::vector<int> expected2 = {0, 2, 3, 5, 10};
        std::vector<int> result2 = pcset(input2);
        CHECK(result2 == expected2);
    }
    
    SUBCASE("pcsetNearest") {
        // Create test set [0, 5, 7] (C, F, G)
        std::vector<int> testSet = {0, 5, 7};
        auto nearest = pcsetNearest(testSet);
        
        // Test various inputs
        std::vector<int> inputs = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        std::vector<int> expected = {0, 0, 0, 5, 5, 5, 7, 7, 7, 7, 7, 7, 12};
        
        for (size_t i = 0; i < inputs.size(); i++) {
            auto result = nearest(inputs[i]);
            CHECK(result.has_value());
            CHECK(result.value() == expected[i]);
        }
        
        // Test empty set
        auto emptyNearest = pcsetNearest(std::vector<int>{});
        CHECK(!emptyNearest(10).has_value());
        CHECK(!emptyNearest(30).has_value());
        CHECK(!emptyNearest(40).has_value());
    }
    
    SUBCASE("pcsetSteps") {
        // Create a whole-tone scale (101010) starting from C (midi 60)
        auto scale = pcsetSteps("101010", 60);
        
        // Test ascending steps
        std::vector<int> ascSteps = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::vector<int> ascExpected = {60, 62, 64, 72, 74, 76, 84, 86, 88, 96};
        
        for (size_t i = 0; i < ascSteps.size(); i++) {
            CHECK(scale(ascSteps[i]) == ascExpected[i]);
        }
        
        // Test descending steps
        std::vector<int> descSteps = {0, -1, -2, -3, -4, -5, -6, -7, -8, -9};
        std::vector<int> descExpected = {60, 52, 50, 48, 40, 38, 36, 28, 26, 24};
        
        for (size_t i = 0; i < descSteps.size(); i++) {
            CHECK(scale(descSteps[i]) == descExpected[i]);
        }
    }
    
    SUBCASE("pcsetDegrees") {
        // Create a whole-tone scale (101010) starting from C (midi 60)
        auto scale = pcsetDegrees("101010", 60);
        
        // Test degrees
        std::vector<int> degrees = {1, 2, 3, 4, 5};
        std::vector<int> expected = {60, 62, 64, 72, 74};
        
        for (size_t i = 0; i < degrees.size(); i++) {
            auto result = scale(degrees[i]);
            CHECK(result.has_value());
            CHECK(result.value() == expected[i]);
        }
        
        // Test negative degrees
        std::vector<int> negDegrees = {-1, -2, -3};
        std::vector<int> negExpected = {52, 50, 48};
        
        for (size_t i = 0; i < negDegrees.size(); i++) {
            auto result = scale(negDegrees[i]);
            CHECK(result.has_value());
            CHECK(result.value() == negExpected[i]);
        }
        
        // Test degree 0 (should be null)
        CHECK(!scale(0).has_value());
    }
}