#include "doctest.h"
#include "tonalcpp/chord_detect.h"

using namespace tonalcpp;

TEST_CASE("chord_detect - detect") {
    SUBCASE("basic detection") {
        auto result = detect({"D", "F#", "A", "C"});
        REQUIRE(result.size() == 1);
        CHECK(result[0] == "D7");
        
        result = detect({"F#", "A", "C", "D"});
        REQUIRE(result.size() == 1);
        CHECK(result[0] == "D7/F#");
        
        result = detect({"A", "C", "D", "F#"});
        REQUIRE(result.size() == 1);
        CHECK(result[0] == "D7/A");
        
        result = detect({"E", "G#", "B", "C#"});
        REQUIRE(result.size() == 2);
        CHECK(result[0] == "E6");
        CHECK(result[1] == "C#m7/E");
    }
    
    SUBCASE("assume perfect 5th") {
        DetectOptions options;
        options.assumePerfectFifth = true;
        
        auto result = detect({"D", "F", "C"}, options);
        REQUIRE(result.size() == 1);
        CHECK(result[0] == "Dm7");
        
        options.assumePerfectFifth = false;
        result = detect({"D", "F", "C"}, options);
        REQUIRE(result.size() == 0);
        
        options.assumePerfectFifth = true;
        result = detect({"D", "F", "A", "C"}, options);
        REQUIRE(result.size() == 2);
        CHECK(result[0] == "Dm7");
        CHECK(result[1] == "F6/D");
        
        options.assumePerfectFifth = false;
        result = detect({"D", "F", "A", "C"}, options);
        REQUIRE(result.size() == 2);
        CHECK(result[0] == "Dm7");
        CHECK(result[1] == "F6/D");
        
        options.assumePerfectFifth = true;
        result = detect({"D", "F", "Ab", "C"}, options);
        REQUIRE(result.size() == 2);
        CHECK(result[0] == "Dm7b5");
        CHECK(result[1] == "Fm6/D");
    }
    
    SUBCASE("detect augmented chords") {
        auto result = detect({"C", "E", "G#"});
        REQUIRE(result.size() == 3);
        CHECK(result[0] == "Caug");
        CHECK(result[1] == "Eaug/C");
        CHECK(result[2] == "G#aug/C");
    }
    
    SUBCASE("edge cases") {
        auto result = detect({});
        REQUIRE(result.size() == 0);
    }
}

TEST_CASE("chord_detect - internal functions") {
    SUBCASE("namedSet") {
        auto noteNameFunc = namedSet({"C4", "E4", "G4"});
        CHECK(noteNameFunc(0) == "C4");
        CHECK(noteNameFunc(4) == "E4");
        CHECK(noteNameFunc(7) == "G4");
        CHECK(noteNameFunc(2) == "");  // Not in the set
    }
    
    SUBCASE("withPerfectFifth") {
        // Binary for C major (C E G) = 100010010000
        std::string majorChroma = "100010010000";
        // Should not change, already has perfect fifth
        CHECK(withPerfectFifth(majorChroma) == majorChroma);
        
        // Binary for C minor 7 flat 5 (C Eb Gb Bb) = 100100001010
        std::string m7b5Chroma = "100100001010";
        // Should not change, has diminished fifth
        CHECK(withPerfectFifth(m7b5Chroma) == m7b5Chroma);
        
        // Binary for C diminished (C Eb Gb) = 100100100000
        std::string dimChroma = "100100100000";
        // Should not change, has diminished fifth
        CHECK(withPerfectFifth(dimChroma) == dimChroma);
        
        // Binary for C sus2 (C D G) = 101000010000
        std::string sus2Chroma = "101000010000";
        // Should not change, already has perfect fifth
        CHECK(withPerfectFifth(sus2Chroma) == sus2Chroma);
        
        // Binary for C sus4 (C F G) = 100001010000
        std::string sus4Chroma = "100001010000";
        // Should not change, already has perfect fifth
        CHECK(withPerfectFifth(sus4Chroma) == sus4Chroma);
        
        // Binary for C suspended (C F) = 100001000000
        std::string suspendedChroma = "100001000000";
        // Should add perfect fifth
        CHECK(withPerfectFifth(suspendedChroma) == "100001010000");
    }
}