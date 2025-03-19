#include "doctest.h"
#include "tonalcpp/pitch_note.h"
#include "test_helpers.h"
#include <vector>
#include <string>

using namespace tonalcpp;
using namespace tonalcpp::test;

// Helper for extracting note properties from a vector of strings
std::vector<std::string> getNames(const std::vector<std::string>& notes) {
    std::vector<std::string> result;
    for (const auto& n : notes) {
        result.push_back(note(n).name);
    }
    return result;
}

std::vector<int> getHeights(const std::vector<std::string>& notes) {
    std::vector<int> result;
    for (const auto& n : notes) {
        result.push_back(note(n).height);
    }
    return result;
}

std::vector<std::optional<int>> getMidis(const std::vector<std::string>& notes) {
    std::vector<std::optional<int>> result;
    for (const auto& n : notes) {
        result.push_back(note(n).midi);
    }
    return result;
}

TEST_CASE("Pitch Note") {
    SUBCASE("tokenizeNote") {
        auto [letter, acc, oct, remainder] = tokenizeNote("Cbb5 major");
        CHECK(letter == "C");
        CHECK(acc == "bb");
        CHECK(oct == "5");
        CHECK(remainder == "major");
        
        auto tokens1 = tokenizeNote("Ax");
        CHECK(std::get<0>(tokens1) == "A");
        CHECK(std::get<1>(tokens1) == "##");
        CHECK(std::get<2>(tokens1) == "");
        CHECK(std::get<3>(tokens1) == "");
        
        auto tokens2 = tokenizeNote("CM");
        CHECK(std::get<0>(tokens2) == "C");
        CHECK(std::get<1>(tokens2) == "");
        CHECK(std::get<2>(tokens2) == "");
        CHECK(std::get<3>(tokens2) == "M");
        
        auto tokens3 = tokenizeNote("maj7");
        CHECK(std::get<0>(tokens3) == "");
        CHECK(std::get<1>(tokens3) == "");
        CHECK(std::get<2>(tokens3) == "");
        CHECK(std::get<3>(tokens3) == "maj7");
        
        auto tokens4 = tokenizeNote("");
        CHECK(std::get<0>(tokens4) == "");
        CHECK(std::get<1>(tokens4) == "");
        CHECK(std::get<2>(tokens4) == "");
        CHECK(std::get<3>(tokens4) == "");
        
        auto tokens5 = tokenizeNote("bb");
        CHECK(std::get<0>(tokens5) == "B");
        CHECK(std::get<1>(tokens5) == "b");
        CHECK(std::get<2>(tokens5) == "");
        CHECK(std::get<3>(tokens5) == "");
        
        auto tokens6 = tokenizeNote("##");
        CHECK(std::get<0>(tokens6) == "");
        CHECK(std::get<1>(tokens6) == "##");
        CHECK(std::get<2>(tokens6) == "");
        CHECK(std::get<3>(tokens6) == "");
        
        auto tokens7 = tokenizeNote(" |\n");
        CHECK(std::get<0>(tokens7) == "");
        CHECK(std::get<1>(tokens7) == "");
        CHECK(std::get<2>(tokens7) == "");
        CHECK(std::get<3>(tokens7) == "");
    }
    
    SUBCASE("note properties from string") {
        Note a4 = note("A4");
        CHECK(a4.empty == false);
        CHECK(a4.name == "A4");
        CHECK(a4.letter == "A");
        CHECK(a4.acc == "");
        CHECK(a4.pc == "A");
        CHECK(a4.step == 5);
        CHECK(a4.alt == 0);
        CHECK(a4.oct.value() == 4);
        CHECK(a4.coord.size() == 2);
        CHECK(a4.coord[0] == 3);
        CHECK(a4.coord[1] == 3);
        CHECK(a4.height == 69);
        CHECK(a4.chroma == 9);
        CHECK(a4.midi.value() == 69);
        CHECK(approxEqual(a4.freq.value(), 440.0));
        
        // Test that Note accepts a Note as parameter
        CHECK(note(note("C4")).name == note("C4").name);
    }
    
    SUBCASE("height") {
        auto heights = getHeights(split("C4 D4 E4 F4 G4"));
        CHECK(heights == std::vector<int>{60, 62, 64, 65, 67});
        
        auto heights2 = getHeights(split("B-2 C-1 D-1"));
        CHECK(heights2 == std::vector<int>{-1, 0, 2});
        
        auto heights3 = getHeights(split("F9 G9 A9"));
        CHECK(heights3 == std::vector<int>{125, 127, 129});
        
        auto heights4 = getHeights(split("C-4 D-4 E-4 F-4 G-4"));
        CHECK(heights4 == std::vector<int>{-36, -34, -32, -31, -29});
        
        auto heights5 = getHeights(split("C D E F G"));
        CHECK(heights5 == std::vector<int>{
            -1188, -1186, -1184, -1183, -1181
        });
        
        auto heights6 = getHeights(split("Cb4 Cbb4 Cbbb4 B#4 B##4 B###4"));
        auto heights7 = getHeights(split("B3 Bb3 Bbb3 C5 C#5 C##5"));
        CHECK(heights6 == heights7);
        
        auto heights8 = getHeights(split("Cb Cbb Cbbb B# B## B###"));
        auto heights9 = getHeights(split("B Bb Bbb C C# C##"));
        CHECK(heights8 == heights9);
    }
    
    SUBCASE("midi") {
        auto midis = getMidis(split("C4 D4 E4 F4 G4"));
        std::vector<std::optional<int>> expected1 = {60, 62, 64, 65, 67};
        CHECK(midis == expected1);
        
        auto midis2 = getMidis(split("B-2 C-1 D-1"));
        std::vector<std::optional<int>> expected2 = {std::nullopt, 0, 2};
        for (size_t i = 0; i < expected2.size(); ++i) {
            CHECK(midis2[i] == expected2[i]);
        }
        
        auto midis3 = getMidis(split("F9 G9 A9"));
        std::vector<std::optional<int>> expected3 = {125, 127, std::nullopt};
        for (size_t i = 0; i < expected3.size(); ++i) {
            CHECK(midis3[i] == expected3[i]);
        }
        
        auto midis4 = getMidis(split("C-4 D-4 E-4 F-4"));
        std::vector<std::optional<int>> expected4 = {
            std::nullopt, std::nullopt, std::nullopt, std::nullopt
        };
        for (size_t i = 0; i < expected4.size(); ++i) {
            CHECK(midis4[i] == expected4[i]);
        }
        
        auto midis5 = getMidis(split("C D E F"));
        std::vector<std::optional<int>> expected5 = {
            std::nullopt, std::nullopt, std::nullopt, std::nullopt
        };
        for (size_t i = 0; i < expected5.size(); ++i) {
            CHECK(midis5[i] == expected5[i]);
        }
    }
    
    SUBCASE("freq") {
        CHECK(approxEqual(note("C4").freq.value(), 261.6255653005986));
        CHECK(approxEqual(note("B-2").freq.value(), 7.716926582126941));
        CHECK(approxEqual(note("F9").freq.value(), 11175.303405856126));
        CHECK(approxEqual(note("C-4").freq.value(), 1.0219748644554634));
        CHECK(note("C").freq == std::nullopt);
        CHECK(note("x").freq == std::nullopt);
    }
    
    SUBCASE("note properties from pitch properties") {
        CHECK(note(Pitch{1, -1, std::nullopt, std::nullopt}).name == "Db");
        CHECK(note(Pitch{2, 1, std::nullopt, std::nullopt}).name == "E#");
        CHECK(note(Pitch{2, 1, 4, std::nullopt}).name == "E#4");
        CHECK(note(Pitch{5, 0, std::nullopt, std::nullopt}).name == "A");
        CHECK(note(Pitch{-1, 0, std::nullopt, std::nullopt}).name == "");
        CHECK(note(Pitch{8, 0, std::nullopt, std::nullopt}).name == "");
    }
}