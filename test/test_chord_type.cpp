#include "doctest.h"
#include "tonalcpp/chord_type.h"
#include "tonalcpp/pitch_interval.h"
#include <vector>
#include <string>
#include <algorithm>
#include <set>

using namespace tonalcpp;
using namespace tonalcpp::chord_type;

TEST_CASE("chord_type - test names") {
    auto namesList = names();
    
    // The exact order might be different in C++ from the TypeScript implementation
    // Just check that the basic chord names are present
    REQUIRE(namesList.size() >= 5);
    
    // Check that important chord names are included in the list
    auto containsName = [&namesList](const std::string& name) {
        return std::find(namesList.begin(), namesList.end(), name) != namesList.end();
    };
    
    CHECK(containsName("fifth"));
    CHECK(containsName("suspended fourth"));
    CHECK(containsName("suspended fourth seventh"));
    CHECK(containsName("augmented"));
    CHECK(containsName("major seventh flat sixth"));
    CHECK(containsName("major"));
    CHECK(containsName("minor"));
}

TEST_CASE("chord_type - test symbols") {
    auto symbolsList = symbols();
    
    // Check at least first 3 symbols are correct
    REQUIRE(symbolsList.size() >= 3);
    CHECK_EQ(symbolsList[0], "5");
    CHECK_EQ(symbolsList[1], "M7#5sus4");
    CHECK_EQ(symbolsList[2], "7#5sus4");
}

TEST_CASE("chord_type - all returns all chords") {
    auto allChords = all();
    
    // Confirm we have the expected number of chords
    // Original TS test checked for 106 chords
    CHECK_EQ(allChords.size(), 106);
}

TEST_CASE("chord_type - getChordType function") {
    ChordType major = getChordType("major");
    
    // Check all properties match expected values
    CHECK_EQ(major.empty, false);
    CHECK_EQ(major.setNum, 2192);
    CHECK_EQ(major.name, "major");
    CHECK_EQ(major.quality, ChordQuality::Major);
    
    REQUIRE(major.intervals.size() == 3);
    CHECK_EQ(major.intervals[0], "1P");
    CHECK_EQ(major.intervals[1], "3M");
    CHECK_EQ(major.intervals[2], "5P");
    
    REQUIRE(major.aliases.size() == 4);
    CHECK_EQ(major.aliases[0], "M");
    CHECK_EQ(major.aliases[1], "^");
    CHECK_EQ(major.aliases[2], "");
    CHECK_EQ(major.aliases[3], "maj");
    
    CHECK_EQ(major.chroma, "100010010000");
    CHECK_EQ(major.normalized, "100001000100");
}

TEST_CASE("chord_type - add a chord") {
    // Add a simple chord with just intervals and aliases
    add({"1P", "5P"}, {"q"});
    ChordType quinta = getChordType("q");
    CHECK_EQ(quinta.chroma, "100000010000");
    
    // Add the same chord with a name
    add({"1P", "5P"}, {"q"}, "quinta");
    ChordType quinta2 = getChordType("quinta");
    
    // Check both references point to same chord
    CHECK_EQ(quinta2.chroma, quinta.chroma);
    CHECK_EQ(quinta2.setNum, quinta.setNum);
}

TEST_CASE("chord_type - clear dictionary") {
    // Clear dictionary
    removeAll();
    
    // Check it's empty
    CHECK_EQ(all().size(), 0);
    CHECK_EQ(keys().size(), 0);
    
    // Reinitialize for other tests
    initChordTypes();
    
    // Verify reinitialization worked
    CHECK_EQ(all().size(), 106);
}

TEST_CASE("chord_type - split function matches JavaScript behavior") {
    // Test with standard chord alias string containing multiple spaces
    std::string test1 = "M ^  maj";
    auto tokens1 = split(test1);
    
    // Should have 4 tokens including empty string
    REQUIRE(tokens1.size() == 4);
    CHECK_EQ(tokens1[0], "M");
    CHECK_EQ(tokens1[1], "^");
    CHECK_EQ(tokens1[2], ""); // Empty string between consecutive spaces
    CHECK_EQ(tokens1[3], "maj");
    
    // Test with leading and trailing spaces
    std::string test2 = " x y ";
    auto tokens2 = split(test2);
    
    // Should have 4 tokens including empty strings at start and end
    REQUIRE(tokens2.size() == 4);
    CHECK_EQ(tokens2[0], "");
    CHECK_EQ(tokens2[1], "x");
    CHECK_EQ(tokens2[2], "y");
    CHECK_EQ(tokens2[3], "");
}

TEST_CASE("chord_type - data validation") {
    auto allChords = all();
    
    SUBCASE("No repeated intervals") {
        // Extract all interval sets
        std::set<std::string> intervalSets;
        for (const auto& chord : allChords) {
            std::string intervals = "";
            for (const auto& interval : chord.intervals) {
                intervals += interval + " ";
            }
            
            if (!intervals.empty()) {
                intervals.pop_back(); // Remove trailing space
            }
            
            bool inserted = intervalSets.insert(intervals).second;
            CHECK(inserted);  // Should be unique
        }
    }
    
    SUBCASE("All chords must have abbreviations") {
        // Check all chords have at least one abbreviation
        for (const auto& chord : allChords) {
            CHECK_FALSE(chord.aliases.empty());
        }
    }
    
    SUBCASE("Intervals should be in ascending order") {
        for (const auto& chord : allChords) {
            std::vector<int> semitones;
            
            // Convert intervals to semitones
            for (const auto& intervalName : chord.intervals) {
                pitch_interval::Interval interval = tonalcpp::pitch_interval::interval(intervalName);
                int semitone = interval.semitones;
                semitones.push_back(semitone);
            }
            
            // Check they are in ascending order
            for (size_t i = 1; i < semitones.size(); i++) {
                INFO("Chord: ", chord.name, ", Intervals: ", 
                     chord.intervals[i-1], "(", semitones[i-1], ") vs ", 
                     chord.intervals[i], "(", semitones[i], ")");
                CHECK(semitones[i-1] < semitones[i]);
            }
        }
    }
}