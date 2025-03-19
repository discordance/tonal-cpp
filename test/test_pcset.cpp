#include "doctest.h"
#include "tonalcpp/pcset.h"
#include "test_helpers.h"
#include <vector>

using namespace tonalcpp;
using namespace tonalcpp::pcset;
using namespace tonalcpp::test;

TEST_CASE("Pcset basic functionality") {
    SUBCASE("from note list") {
        auto pcset = getPcset(std::vector<std::string>{"c", "d", "e"});
        CHECK(pcset.empty == false);
        CHECK(pcset.name == "");
        
        // Now we can test these since pitch_note is implemented
        CHECK(pcset.setNum == 2688);
        CHECK(pcset.chroma == "101010000000");
        CHECK(pcset.normalized == "100000001010");
        CHECK(pcset.intervals == std::vector<std::string>{"1P", "2M", "3M"});
        
        CHECK(getPcset(std::vector<std::string>{"d", "e", "c"}).setNum == getPcset(std::vector<std::string>{"c", "d", "e"}).setNum);
        CHECK(getPcset(std::vector<std::string>{"not a note or interval"}).empty == true);
        CHECK(getPcset(std::vector<std::string>{}).empty == true);
    }
    
    SUBCASE("from pcset number") {
        CHECK(getPcset(2048).chroma == getPcset(std::vector<std::string>{"C"}).chroma);
        
        auto set = getPcset(2048);
        CHECK(getPcset(set.setNum).setNum == set.setNum);
    }
    
    SUBCASE("num") {
        CHECK(num("000000000001") == 1);
        CHECK(num(std::vector<std::string>{"B"}) == 1);
        CHECK(num(std::vector<std::string>{"Cb"}) == 1);
        CHECK(num(std::vector<std::string>{"C", "E", "G"}) == 2192);
        CHECK(num(std::vector<std::string>{"C"}) == 2048);
        CHECK(num("100000000000") == 2048);
        CHECK(num("111111111111") == 4095);
    }
    
    SUBCASE("normalized") {
        const std::string likeC = getPcset(std::vector<std::string>{"C"}).chroma;
        std::vector<std::string> pcs = {"c", "d", "e", "f", "g", "a", "b"};
        for (const auto& pc : pcs) {
            CHECK(getPcset(std::vector<std::string>{pc}).normalized == likeC);
        }
        
        CHECK(getPcset(std::vector<std::string>{"E", "F#"}).normalized == getPcset(std::vector<std::string>{"C", "D"}).normalized);
    }
}

TEST_CASE("Pcset chroma operations") {
    SUBCASE("chroma") {
        CHECK(chroma(std::vector<std::string>{"C"}) == "100000000000");
        CHECK(chroma(std::vector<std::string>{"D"}) == "001000000000");
        CHECK(chroma(split("c d e")) == "101010000000");
        CHECK(chroma(split("g g#4 a bb5")) == "000000011110");
        CHECK(chroma(split("P1 M2 M3 P4 P5 M6 M7")) == chroma(split("c d e f g a b")));
        CHECK(chroma("101010101010") == "101010101010");
        CHECK(chroma(std::vector<std::string>{"one", "two"}) == "000000000000");
        CHECK(chroma("A B C") == "000000000000");
    }
    
    SUBCASE("chromas") {
        auto allChromas = chromas();
        CHECK(allChromas.size() == 2048);
        CHECK(allChromas[0] == "100000000000");
        CHECK(allChromas[2047] == "111111111111");
    }
    
    SUBCASE("intervals") {
        CHECK(intervals("101010101010") == std::vector<std::string>{"1P", "2M", "3M", "5d", "6m", "7m"});
        CHECK(intervals("1010").empty());
        CHECK(intervals(std::vector<std::string>{"C", "G", "B"}) == std::vector<std::string>{"1P", "5P", "7M"});
        CHECK(intervals(std::vector<std::string>{"D", "F", "A"}) == std::vector<std::string>{"2M", "4P", "6M"});
    }
    
    SUBCASE("isChroma") {
        CHECK(getPcset("101010101010").chroma == "101010101010");
        CHECK(getPcset("1010101").chroma == "000000000000");
        CHECK(getPcset("blah").chroma == "000000000000");
        CHECK(getPcset("c d e").chroma == "000000000000");
    }
}

TEST_CASE("Pcset set operations") {
    SUBCASE("isSubsetOf") {
        auto isInCMajor = [](const std::vector<std::string>& notes) {
            return isSubsetOf(split("c4 e6 g"), notes);
        };
        CHECK(isInCMajor(split("c2 g7")) == true);
        CHECK(isInCMajor(split("c2 e")) == true);
        CHECK(isInCMajor(split("c2 e3 g4")) == false);
        CHECK(isInCMajor(split("c2 e3 b5")) == false);
        CHECK(isSubsetOf(split("c d e"), std::vector<std::string>{"C", "D"}) == true);
        
        CHECK(isSubsetOf("101010101010", "101000000000") == true);
        CHECK(isSubsetOf("101010101010", "111000000000") == false);
    }
    
    SUBCASE("isSupersetOf") {
        auto extendsCMajor = [](const std::vector<std::string>& notes) {
            return isSupersetOf(std::vector<std::string>{"c", "e", "g"}, notes);
        };
        CHECK(extendsCMajor(split("c2 g3 e4 f5")) == true);
        CHECK(extendsCMajor(split("e c g")) == false);
        CHECK(extendsCMajor(split("c e f")) == false);
        CHECK(isSupersetOf(std::vector<std::string>{"c", "d"}, std::vector<std::string>{"c", "d", "e"}) == true);
        
        CHECK(isSupersetOf("101000000000", "101010101010") == true);
        CHECK(isSupersetOf("101000000000", "110010101010") == false);
    }
    
    SUBCASE("isEqual") {
        CHECK(isEqual(split("c2 d3 e7 f5"), split("c4 c d5 e6 f1")) == true);
        CHECK(isEqual(split("c f"), split("c4 c f1")) == true);
        
        CHECK(isEqual("101010101010", "101010101010") == true);
        CHECK(isEqual("101010101010", "101010001010") == false);
    }
}

TEST_CASE("Pcset note operations") {
    SUBCASE("isNoteIncludedIn") {
        auto isIncludedInC = [](const std::string& note) {
            return isNoteIncludedIn(std::vector<std::string>{"c", "d", "e"}, note);
        };
        CHECK(isIncludedInC("C4") == true);
        CHECK(isIncludedInC("C#4") == false);
    }
    
    SUBCASE("filter") {
        auto inCMajor = [](const std::vector<std::string>& notes) {
            return filter(split("c d e"), notes);
        };
        CHECK(inCMajor(split("c2 c#2 d2 c3 c#3 d3")) == split("c2 d2 c3 d3"));
        CHECK(filter(split("c"), split("c2 c#2 d2 c3 c#3 d3")) == split("c2 c3"));
    }
    
    SUBCASE("notes") {
        CHECK(notes(split("c d e f g a b")) == split("C D E F G A B"));
        CHECK(notes(split("b a g f e d c")) == split("C D E F G A B"));
        CHECK(notes(split("D3 A3 Bb3 C4 D4 E4 F4 G4 A4")) == split("C D E F G A Bb"));
        CHECK(notes("101011010110") == split("C D E F G A Bb"));
        CHECK(notes(std::vector<std::string>{"blah", "x"}).empty());
    }
}

TEST_CASE("Pcset modes") {
    SUBCASE("modes") {
        auto majorModes = modes(split("c d e f g a b"));
        CHECK(majorModes.size() == 7);
        CHECK(majorModes[0] == "101011010101");
        CHECK(majorModes[1] == "101101010110");
        CHECK(majorModes[2] == "110101011010");
        
        auto allModes = modes(split("c d e f g a b"), false);
        CHECK(allModes.size() == 12);
        CHECK(allModes[0] == "101011010101");
        
        CHECK(modes(std::vector<std::string>{"blah", "bleh"}).empty());
        
        auto chromaModes = modes("101010101010", true);
        CHECK(chromaModes.size() > 0);
    }
}