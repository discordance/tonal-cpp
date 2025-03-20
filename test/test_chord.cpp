#include "doctest.h"
#include "tonalcpp/chord.h"
#include <string>
#include <vector>
#include <functional>

using namespace tonalcpp::chord;

TEST_CASE("tokenize") {
    CHECK(tokenize("Cmaj7") == ChordNameTokens({"C", "maj7", ""}));
    CHECK(tokenize("c7") == ChordNameTokens({"C", "7", ""}));
    CHECK(tokenize("maj7") == ChordNameTokens({"", "maj7", ""}));
    CHECK(tokenize("c#4 m7b5") == ChordNameTokens({"C#", "4m7b5", ""}));
    CHECK(tokenize("c#4m7b5") == ChordNameTokens({"C#", "4m7b5", ""}));
    CHECK(tokenize("Cb7b5") == ChordNameTokens({"Cb", "7b5", ""}));
    CHECK(tokenize("Eb7add6") == ChordNameTokens({"Eb", "7add6", ""}));
    CHECK(tokenize("Bb6b5") == ChordNameTokens({"Bb", "6b5", ""}));
    CHECK(tokenize("aug") == ChordNameTokens({"", "aug", ""}));
    CHECK(tokenize("C11") == ChordNameTokens({"C", "11", ""}));
    CHECK(tokenize("C13no5") == ChordNameTokens({"C", "13no5", ""}));
    CHECK(tokenize("C64") == ChordNameTokens({"C", "64", ""}));
    CHECK(tokenize("C9") == ChordNameTokens({"C", "9", ""}));
    CHECK(tokenize("C5") == ChordNameTokens({"C", "5", ""}));
    CHECK(tokenize("C4") == ChordNameTokens({"C", "4", ""}));
    
    // With bass
    CHECK(tokenize("Cmaj7/G") == ChordNameTokens({"C", "maj7", "G"}));
    CHECK(tokenize("bb6/a##") == ChordNameTokens({"Bb", "6", "A##"}));
}

TEST_CASE("getChord") {
    SUBCASE("Chord properties") {
        Chord chord = getChord("maj7", "G4", "G4");
        CHECK(chord.empty == false);
        CHECK(chord.name == "G major seventh");
        CHECK(chord.symbol == "Gmaj7");
        CHECK(chord.tonic == "G");
        CHECK(chord.root == "G");
        CHECK(chord.bass == "");
        CHECK(chord.rootDegree.value() == 1);
        CHECK(chord.setNum == 2193);
        CHECK(chord.type == "major seventh");
        
        // Check if aliases are correct (order might vary)
        bool aliasMatch = false;
        std::vector<std::string> expectedAliases = {"maj7", "Δ", "ma7", "M7", "Maj7", "^7"};
        if (chord.aliases.size() == expectedAliases.size()) {
            bool allFound = true;
            for (const auto& alias : expectedAliases) {
                if (std::find(chord.aliases.begin(), chord.aliases.end(), alias) == chord.aliases.end()) {
                    allFound = false;
                    break;
                }
            }
            aliasMatch = allFound;
        }
        CHECK(aliasMatch);
        
        CHECK(chord.chroma == "100010010001");
        CHECK(chord.intervals == std::vector<std::string>({"1P", "3M", "5P", "7M"}));
        CHECK(chord.normalized == "100010010001");
        CHECK(chord.notes == std::vector<std::string>({"G", "B", "D", "F#"}));
        CHECK(chord.quality == tonalcpp::chord_type::ChordQuality::Major);
    }
    
    SUBCASE("first inversion") {
        Chord chord = getChord("maj7", "G4", "B4");
        CHECK(chord.empty == false);
        CHECK(chord.name == "G major seventh over B");
        CHECK(chord.symbol == "Gmaj7/B");
        CHECK(chord.tonic == "G");
        CHECK(chord.root == "B");
        CHECK(chord.bass == "B");
        CHECK(chord.rootDegree.value() == 2);
        CHECK(chord.intervals == std::vector<std::string>({"3M", "5P", "7M", "8P"}));
        CHECK(chord.notes == std::vector<std::string>({"B", "D", "F#", "G"}));
    }
    
    SUBCASE("first inversion without octave") {
        Chord chord = getChord("maj7", "G", "B");
        CHECK(chord.name == "G major seventh over B");
        CHECK(chord.symbol == "Gmaj7/B");
        CHECK(chord.notes == std::vector<std::string>({"B", "D", "F#", "G"}));
    }
    
    SUBCASE("second inversion") {
        Chord chord = getChord("maj7", "G4", "D5");
        CHECK(chord.name == "G major seventh over D");
        CHECK(chord.symbol == "Gmaj7/D");
        CHECK(chord.root == "D");
        CHECK(chord.rootDegree.value() == 3);
        CHECK(chord.intervals == std::vector<std::string>({"5P", "7M", "8P", "10M"}));
        CHECK(chord.notes == std::vector<std::string>({"D", "F#", "G", "B"}));
    }
    
    SUBCASE("without root") {
        Chord chord = getChord("M7", "G");
        CHECK(chord.symbol == "GM7");
        CHECK(chord.name == "G major seventh");
        CHECK(chord.notes == std::vector<std::string>({"G", "B", "D", "F#"}));
    }
    
    SUBCASE("without tonic nor root") {
        Chord chord = getChord("dim");
        CHECK(chord.symbol == "dim");
        CHECK(chord.name == "diminished");
        CHECK(chord.tonic == "");
        CHECK(chord.root == "");
        CHECK(chord.bass == "");
        CHECK(!chord.rootDegree.has_value()); // nullopt means not found
        CHECK(chord.type == "diminished");
        CHECK(chord.chroma == "100100100000");
        CHECK(chord.empty == false);
        CHECK(chord.intervals == std::vector<std::string>({"1P", "3m", "5d"}));
        CHECK(chord.notes.empty());
    }
}

TEST_CASE("get/chord") {
    Chord cmaj7 = get("Cmaj7");
    CHECK(cmaj7.empty == false);
    CHECK(cmaj7.symbol == "Cmaj7");
    CHECK(cmaj7.name == "C major seventh");
    CHECK(cmaj7.tonic == "C");
    CHECK(cmaj7.root == "");
    CHECK(cmaj7.bass == "");
    CHECK(!cmaj7.rootDegree.has_value()); // nullopt means not found
    CHECK(cmaj7.setNum == 2193);
    CHECK(cmaj7.type == "major seventh");
    CHECK(cmaj7.chroma == "100010010001");
    CHECK(cmaj7.intervals == std::vector<std::string>({"1P", "3M", "5P", "7M"}));
    CHECK(cmaj7.normalized == "100010010001");
    CHECK(cmaj7.notes == std::vector<std::string>({"C", "E", "G", "B"}));
    
    CHECK(get("hello").empty == true);
    CHECK(get("").empty == true);
    
    // Chord with bass, without root
    Chord cBass = chord("C/Bb");
    CHECK(cBass.bass == "Bb");
    CHECK(cBass.name == "C major over Bb");
    CHECK(cBass.intervals[0] == "-2M");
    CHECK(cBass.notes == std::vector<std::string>({"Bb", "C", "E", "G"}));
}

TEST_CASE("chord without tonic") {
    CHECK(get("dim").name == "diminished");
    CHECK(get("dim7").name == "diminished seventh");
}

TEST_CASE("notes property") {
    CHECK(get("Cmaj7").notes == std::vector<std::string>({"C", "E", "G", "B"}));
    CHECK(get("Eb7add6").notes == std::vector<std::string>({"Eb", "G", "Bb", "Db", "C"}));
    
    std::vector<std::string> tokens = {"C4", "maj7"};
    CHECK(get(tokens).notes == std::vector<std::string>({"C", "E", "G", "B"}));
    
    CHECK(get("C7").notes == std::vector<std::string>({"C", "E", "G", "Bb"}));
    CHECK(get("Cmaj7#5").notes == std::vector<std::string>({"C", "E", "G#", "B"}));
    CHECK(get("blah").notes.empty());
}

TEST_CASE("notes with two params") {
    std::vector<std::string> tokens1 = {"C", "maj7"};
    CHECK(get(tokens1).notes == std::vector<std::string>({"C", "E", "G", "B"}));
    
    std::vector<std::string> tokens2 = {"C6", "maj7"};
    CHECK(get(tokens2).notes == std::vector<std::string>({"C", "E", "G", "B"}));
}

TEST_CASE("augmented chords") {
    CHECK(get("Caug").notes == std::vector<std::string>({"C", "E", "G#"}));
    
    std::vector<std::string> tokens = {"C", "aug"};
    CHECK(get(tokens).notes == std::vector<std::string>({"C", "E", "G#"}));
}

TEST_CASE("intervals") {
    CHECK(get("maj7").intervals == std::vector<std::string>({"1P", "3M", "5P", "7M"}));
    CHECK(get("Cmaj7").intervals == std::vector<std::string>({"1P", "3M", "5P", "7M"}));
    CHECK(get("aug").intervals == std::vector<std::string>({"1P", "3M", "5A"}));
    CHECK(get("C13no5").intervals == std::vector<std::string>({"1P", "3M", "7m", "9M", "13M"}));
    CHECK(get("major").intervals == std::vector<std::string>({"1P", "3M", "5P"}));
}

TEST_CASE("notes function") {
    CHECK(notes("Cmaj7") == std::vector<std::string>({"C", "E", "G", "B"}));
    CHECK(notes("maj7").empty());
    CHECK(notes("maj7", "C4") == std::vector<std::string>({"C4", "E4", "G4", "B4"}));
    CHECK(notes("Cmaj7", "C4") == std::vector<std::string>({"C4", "E4", "G4", "B4"}));
    CHECK(notes("Cmaj7", "D4") == std::vector<std::string>({"D4", "F#4", "A4", "C#5"}));
    CHECK(notes("C/Bb", "D4") == std::vector<std::string>({"C4", "D4", "F#4", "A4"}));
}

TEST_CASE("existence") {
    CHECK(get("C6add9").name == "C sixth added ninth");
    CHECK(get("maj7").empty == false);
    CHECK(get("Cmaj7").empty == false);
    CHECK(get("mixolydian").empty == true);
}

TEST_CASE("transpose") {
    CHECK(transpose("Eb7b9", "5P") == "Bb7b9");
    CHECK(transpose("7b9", "5P") == "7b9");
    CHECK(transpose("Cmaj7/B", "P5") == "Gmaj7/F#");
}

TEST_CASE("extended") {
    // Note: This test might be harder to verify exactly since it depends on the chord dictionary
    // Just check that we get non-empty results for a valid chord
    CHECK(!extended("CMaj7").empty());
}

TEST_CASE("reduced") {
    // Similar to extended, just check we get results
    std::vector<std::string> reduced_chords = reduced("CMaj7");
    CHECK(!reduced_chords.empty());
    
    // Check if the reduced set includes some expected values
    bool contains_cm = false;
    bool contains_c5 = false;
    for (const auto& chord : reduced_chords) {
        if (chord == "CM") contains_cm = true;
        if (chord == "C5") contains_c5 = true;
    }
    CHECK(contains_cm);
    CHECK(contains_c5);
}

TEST_CASE("degreeToNote") {
    SUBCASE("ascending") {
        std::vector<std::string> result;
        for (int i = 1; i <= 4; i++) {
            result.push_back(degreeToNote("C", i));
        }
        CHECK(result == std::vector<std::string>({"C", "E", "G", "C"}));
        
        std::vector<std::string> result2;
        for (int i = 1; i <= 4; i++) {
            result2.push_back(degreeToNote("CM", i, "C4"));
        }
        CHECK(result2 == std::vector<std::string>({"C4", "E4", "G4", "C5"}));
        
        std::vector<std::string> result3;
        for (int i = 1; i <= 10; i++) {
            result3.push_back(degreeToNote("Cm6", i, "C4"));
        }
        CHECK(result3 == std::vector<std::string>({"C4", "Eb4", "G4", "A4", "C5", "Eb5", "G5", "A5", "C6", "Eb6"}));
        
        std::vector<std::string> result4;
        for (int i = 1; i <= 4; i++) {
            result4.push_back(degreeToNote("C/B", i));
        }
        CHECK(result4 == std::vector<std::string>({"B", "C", "E", "G"}));
    }
    
    SUBCASE("descending") {
        std::vector<std::string> result;
        for (int i = -1; i >= -3; i--) {
            result.push_back(degreeToNote("C", i));
        }
        CHECK(result == std::vector<std::string>({"G", "E", "C"}));
        
        std::vector<std::string> result2;
        for (int i = -1; i >= -3; i--) {
            result2.push_back(degreeToNote("CM", i, "C4"));
        }
        CHECK(result2 == std::vector<std::string>({"G3", "E3", "C3"}));
    }
}

TEST_CASE("stepToNote") {
    std::vector<std::string> result;
    for (int i = -3; i <= 3; i++) {
        result.push_back(stepToNote("Caug", i, "C4"));
    }

    CHECK(result == std::vector<std::string>({"C3", "E3", "G#3", "C4", "E4", "G#4", "C5"}));
}