#include "doctest.h"
#include "tonalcpp/voicing.h"
#include "tonalcpp/voicing_dictionary.h"
#include "tonalcpp/voice_leading.h"
#include <vector>
#include <string>

using namespace tonalcpp::voicing;
using namespace tonalcpp::voicing_dictionary;
using namespace tonalcpp::voice_leading;

TEST_CASE("Voicing search") {
    SUBCASE("C major triad inversions") {
        std::vector<std::vector<std::string>> expected = {
            {"C3", "E3", "G3"},
            {"C4", "E4", "G4"},
            {"E3", "G3", "C4"},
            {"E4", "G4", "C5"},
            {"G3", "C4", "E4"}
        };
        
        auto result = search("C", {"C3", "C5"}, triads);
        CHECK(result == expected);
    }
    
    SUBCASE("C^7 lefthand") {
        std::vector<std::vector<std::string>> expected = {
            {"E3", "G3", "B3", "D4"},
            {"E4", "G4", "B4", "D5"},
            {"B3", "D4", "E4", "G4"}
        };
        
        auto result = search("C^7", {"E3", "D5"}, lefthand);
        CHECK(result == expected);
    }
    
    SUBCASE("Cminor7 lefthand custom") {
        VoicingDictionary custom = {
            {"minor7", {"3m 5P 7m 9M", "7m 9M 10m 12P"}}
        };
        
        std::vector<std::vector<std::string>> expected = {
            {"Eb3", "G3", "Bb3", "D4"},
            {"Eb4", "G4", "Bb4", "D5"},
            {"Bb3", "D4", "Eb4", "G4"}
        };
        
        auto result = search("Cminor7", {"Eb3", "D5"}, custom);
        CHECK(result == expected);
    }
}

TEST_CASE("Voicing get") {
    SUBCASE("get default") {
        std::vector<std::string> expected = {"F3", "A3", "C4", "E4"};
        auto result = get("Dm7");
        CHECK(result == expected);
    }
    
    SUBCASE("get without lastVoicing") {
        std::vector<std::string> expected = {"F3", "A3", "C4", "E4"};
        auto result = get("Dm7", {"F3", "A4"}, lefthand, topNoteDiff);
        CHECK(result == expected);
    }
    
    SUBCASE("get with lastVoicing") {
        std::vector<std::string> expected = {"C4", "E4", "F4", "A4"};
        std::vector<std::string> lastVoicing = {"C4", "E4", "G4", "B4"};
        auto result = get("Dm7", {"F3", "A4"}, lefthand, topNoteDiff, lastVoicing);
        CHECK(result == expected);
    }
}

TEST_CASE("Voicing sequence") {
    std::vector<std::vector<std::string>> expected = {
        {"C4", "E4", "G4"},  // root position
        {"A3", "C4", "F4"},  // first inversion (F4 closest to G4)
        {"B3", "D4", "G4"}   // first inversion (G4 closest to F4)
    };
    
    auto result = sequence({"C", "F", "G"}, {"F3", "A4"}, triads, topNoteDiff);
    CHECK(result == expected);
}