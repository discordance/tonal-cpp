#include "doctest.h"
#include "tonalcpp/scale.h"
#include "tonalcpp/note.h"
#include "tonalcpp/helpers.h"
#include <vector>
#include <string>

using namespace tonalcpp;

// Helper function to simplify test access to the split function
std::vector<std::string> split(const std::string& s) {
    return helpers::split(s);
}

TEST_CASE("scale::get") {
    scale::Scale major = scale::get("major");
    CHECK(major.empty == false);
    CHECK(major.tonic.has_value() == false);
    CHECK(major.notes.empty());
    CHECK(major.type == "major");
    CHECK(major.name == "major");
    CHECK(major.intervals == split("1P 2M 3M 4P 5P 6M 7M"));
    CHECK(major.aliases == std::vector<std::string>{"ionian"});
    CHECK(major.setNum == 2773);
    CHECK(major.chroma == "101011010101");
    CHECK(major.normalized == "101010110101");

    scale::Scale pentatonic = scale::get("c5 pentatonic");
    CHECK(pentatonic.empty == false);
    CHECK(pentatonic.name == "C5 major pentatonic");
    CHECK(pentatonic.type == "major pentatonic");
    CHECK(pentatonic.tonic.has_value() == true);
    CHECK(*pentatonic.tonic == "C5");
    CHECK(pentatonic.notes == split("C5 D5 E5 G5 A5"));
    CHECK(pentatonic.intervals == split("1P 2M 3M 5P 6M"));
    CHECK(pentatonic.aliases == std::vector<std::string>{"pentatonic"});
    CHECK(pentatonic.setNum == 2708);
    CHECK(pentatonic.chroma == "101010010100");
    CHECK(pentatonic.normalized == "100101001010");

    // Test getting scale with tokens
    CHECK(scale::get(std::make_tuple("C4", "major")).notes == scale::get("C4 major").notes);
    
    // Test case insensitivity
    CHECK(scale::get("C4 Major").notes == scale::get("C4 major").notes);
}

TEST_CASE("scale::tokenize") {
    CHECK(scale::tokenize("c major") == std::make_tuple("C", "major"));
    CHECK(scale::tokenize("cb3 major") == std::make_tuple("Cb3", "major"));
    CHECK(scale::tokenize("melodic minor") == std::make_tuple("", "melodic minor"));
    CHECK(scale::tokenize("dorian") == std::make_tuple("", "dorian"));
    CHECK(scale::tokenize("c") == std::make_tuple("C", ""));
    CHECK(scale::tokenize("") == std::make_tuple("", ""));
}

TEST_CASE("scale::isKnown") {
    CHECK(scale::get("major").empty == false);
    CHECK(scale::get("Db major").empty == false);
    CHECK(scale::get("hello").empty == true);
    CHECK(scale::get("").empty == true);
    CHECK(scale::get("Maj7").empty == true);
}

TEST_CASE("scale::get with mixed cases") {
    CHECK(scale::get("C lydian #5P PENTATONIC").notes == scale::get("C lydian #5P pentatonic").notes);
    CHECK(scale::get("lydian #5P PENTATONIC").name == scale::get("lydian #5P pentatonic").name);
}

TEST_CASE("scale::get intervals") {
    CHECK(scale::get("major").intervals == split("1P 2M 3M 4P 5P 6M 7M"));
    CHECK(scale::get("C major").intervals == split("1P 2M 3M 4P 5P 6M 7M"));
    CHECK(scale::get("blah").intervals.empty());
}

TEST_CASE("scale::get notes") {
    CHECK(scale::get("C major").notes == split("C D E F G A B"));
    CHECK(scale::get("C lydian #9").notes == split("C D# E F# G A B"));
    CHECK(scale::get(std::make_tuple("C", "major")).notes == split("C D E F G A B"));
    CHECK(scale::get(std::make_tuple("C4", "major")).notes == split("C4 D4 E4 F4 G4 A4 B4"));
    CHECK(scale::get(std::make_tuple("eb", "bebop")).notes == split("Eb F G Ab Bb C Db D"));
    CHECK(scale::get(std::make_tuple("C", "no-scale")).notes.empty());
    CHECK(scale::get(std::make_tuple("no-note", "major")).notes.empty());
}

TEST_CASE("scale::detect") {
    SUBCASE("exact match") {
        CHECK(scale::detect(split("D E F# A B"), "D", "exact") == std::vector<std::string>{"D major pentatonic"});
        CHECK(scale::detect(split("D E F# A B"), "B", "exact") == std::vector<std::string>{"B minor pentatonic"});
        CHECK(scale::detect(split("D F# B C C#"), "D", "exact").empty());
        CHECK(scale::detect(split("c d e f g a b"), "C", "exact") == std::vector<std::string>{"C major"});
        CHECK(scale::detect(split("c2 d6 e3 f1 g7 a6 b5"), "d", "exact") == std::vector<std::string>{"D dorian"});
    }

    SUBCASE("fit match") {
        auto result = scale::detect(split("C D E F G A B"), "C", "fit");
        CHECK(result.size() >= 4); // Should include major, bebop, etc
        CHECK(result[0] == "C major"); // First match should be major
    }

    SUBCASE("tonic will be added") {
        CHECK(scale::detect(split("c d e f g b"), "C", "exact").empty());
        CHECK(scale::detect(split("c d e f g b"), "a", "exact") == std::vector<std::string>{"A minor"});
    }
}

TEST_CASE("Ukrainian Dorian scale") {
    // Source https://en.wikipedia.org/wiki/Ukrainian_Dorian_scale
    CHECK(scale::get("C romanian minor").notes == split("C D Eb F# G A Bb"));
    CHECK(scale::get("C ukrainian dorian").notes == split("C D Eb F# G A Bb"));
    CHECK(scale::get("B romanian minor").notes == split("B C# D E# F# G# A"));
    CHECK(scale::get("B dorian #4").notes == split("B C# D E# F# G# A"));
    CHECK(scale::get("B altered dorian").notes == split("B C# D E# F# G# A"));
}

TEST_CASE("scale::scaleChords") {
    auto chords = scale::scaleChords("pentatonic");
    CHECK(chords.size() >= 4); // Should have 5, M, 6, sus2, Madd9
    CHECK(scale::scaleChords("none").empty());
}

TEST_CASE("scale::extended") {
    auto extended = scale::extended("major");
    CHECK(extended.size() >= 3); // Should include bebop, ichikosucho, chromatic
    CHECK(scale::extended("none").empty());
}

TEST_CASE("scale::reduced") {
    auto reduced = scale::reduced("major");
    CHECK(reduced.size() >= 2); // Should include major pentatonic, ionian pentatonic, ritusen
    CHECK(scale::reduced("D major") == scale::reduced("major"));
    CHECK(scale::reduced("none").empty());
}

TEST_CASE("specific and problematic scales") {
    SUBCASE("whole note scale should use 6th") {
        std::string notes = "";
        for (const auto& note : scale::get("C whole tone").notes) {
            if (!notes.empty()) notes += " ";
            notes += note;
        }
        CHECK(notes == "C D E F# G# A#");

        notes = "";
        for (const auto& note : scale::get("Db whole tone").notes) {
            if (!notes.empty()) notes += " ";
            notes += note;
        }
        CHECK(notes == "Db Eb F G A B");
    }
}

TEST_CASE("scale::scaleNotes") {
    CHECK(scale::scaleNotes(split("C4 c3 C5 C4 c4")) == std::vector<std::string>{"C"});
    
    auto notes = scale::scaleNotes(split("C4 f3 c#10 b5 d4 cb4"));
    std::sort(notes.begin(), notes.end());
    auto expected = split("C C# D F B Cb");
    std::sort(expected.begin(), expected.end());
    CHECK(notes == expected);
    
    CHECK(scale::scaleNotes(split("D4 c#5 A5 F#6")) == std::vector<std::string>{"D", "F#", "A", "C#"});
}

TEST_CASE("scale::modeNames") {
    auto penta = scale::modeNames("pentatonic");
    CHECK(penta.size() == 5);
    CHECK(penta[0] == std::make_tuple("1P", "major pentatonic"));
    CHECK(penta[1] == std::make_tuple("2M", "egyptian"));
    CHECK(penta[2] == std::make_tuple("3M", "malkos raga"));
    CHECK(penta[3] == std::make_tuple("5P", "ritusen"));
    CHECK(penta[4] == std::make_tuple("6M", "minor pentatonic"));
    
    CHECK(scale::modeNames("whole tone pentatonic").size() == 1);
    CHECK(scale::modeNames("whole tone pentatonic")[0] == std::make_tuple("1P", "whole tone pentatonic"));
    
    auto cPenta = scale::modeNames("C pentatonic");
    CHECK(cPenta.size() == 5);
    CHECK(cPenta[0] == std::make_tuple("C", "major pentatonic"));
    CHECK(cPenta[1] == std::make_tuple("D", "egyptian"));
    CHECK(cPenta[2] == std::make_tuple("E", "malkos raga"));
    CHECK(cPenta[3] == std::make_tuple("G", "ritusen"));
    CHECK(cPenta[4] == std::make_tuple("A", "minor pentatonic"));
    
    CHECK(scale::modeNames("C whole tone pentatonic").size() == 1);
    CHECK(scale::modeNames("C whole tone pentatonic")[0] == std::make_tuple("C", "whole tone pentatonic"));
}

TEST_CASE("scale::rangeOf") {
    SUBCASE("range of a scale name") {
        auto range = split("C4 D4 E4 G4 A4 C5");
        auto l = scale::rangeOf("C pentatonic", "C4", "C5");

        CHECK(scale::rangeOf("C pentatonic", "C4", "C5") == range);
        
        auto descRange = split("C5 A4 G4 E4 D4 C4");
        CHECK(scale::rangeOf("C pentatonic", "C5", "C4") == descRange);
        
        auto gRange = split("G3 E3 D3 C3 A2");
        CHECK(scale::rangeOf("C pentatonic", "g3", "a2") == gRange);
    }
    
    SUBCASE("range of a scale name with flat") {
        auto range = split("Cb4 Db4 Eb4 Fb4 Gb4 Ab4 Bb4 Cb5");
        CHECK(scale::rangeOf("Cb major", "Cb4", "Cb5") == range);
    }
    
    SUBCASE("range of a scale name with sharp") {
        auto range = split("C#4 D#4 E#4 F#4 G#4 A#4 B#4 C#5");
        CHECK(scale::rangeOf("C# major", "C#4", "C#5") == range);
    }
    
    SUBCASE("range of a scale without tonic") {
        CHECK(scale::rangeOf("pentatonic", "C4", "C5").empty());
    }
    
    SUBCASE("range of a list of notes") {
        auto range = split("C4 Db4 G4 C5");
        CHECK(scale::rangeOf(split("c4 g4 db3 g"), "c4", "c5") == range);
    }
}

TEST_CASE("scale::degrees") {
    SUBCASE("positive scale degrees") {
        std::string result;
        for (int i = 1; i <= 10; i++) {
            if (!result.empty()) result += " ";
            result += scale::degrees("C major", i);
        }
        CHECK(result == "C D E F G A B C D E");
        
        result = "";
        for (int i = 1; i <= 10; i++) {
            if (!result.empty()) result += " ";
            result += scale::degrees("C4 major", i);
        }
        CHECK(result == "C4 D4 E4 F4 G4 A4 B4 C5 D5 E5");
        
        result = "";
        for (int i = 1; i <= 11; i++) {
            if (!result.empty()) result += " ";
            result += scale::degrees("C4 pentatonic", i);
        }
        CHECK(result == "C4 D4 E4 G4 A4 C5 D5 E5 G5 A5 C6");
    }
    
    SUBCASE("invalid inputs") {
        CHECK(scale::degrees("C major", 0) == "");
        CHECK(scale::degrees("C nonsense", 1) == "");
    }
    
    SUBCASE("negative scale degrees") {
        std::string result;
        for (int i = -1; i >= -10; i--) {
            if (!result.empty()) result += " ";
            result += scale::degrees("C major", i);
        }
        CHECK(result == "B A G F E D C B A G");
        
        result = "";
        for (int i = -1; i >= -10; i--) {
            if (!result.empty()) result += " ";
            result += scale::degrees("C4 major", i);
        }
        CHECK(result == "B3 A3 G3 F3 E3 D3 C3 B2 A2 G2");
        
        result = "";
        for (int i = -1; i >= -11; i--) {
            if (!result.empty()) result += " ";
            result += scale::degrees("C4 pentatonic", i);
        }
        CHECK(result == "A3 G3 E3 D3 C3 A2 G2 E2 D2 C2 A1");
    }
}

TEST_CASE("scale::steps") {
    std::vector<std::string> result;
    for (int i = -3; i <= 2; i++) {
        result.push_back(scale::steps("C4 major", i));
    }
    CHECK(result == split("G3 A3 B3 C4 D4 E4"));
}