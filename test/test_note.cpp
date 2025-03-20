#include "doctest.h"
#include "tonalcpp/note.h"
#include "test_helpers.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace tonalcpp;

TEST_CASE("note module") {
    SUBCASE("get") {
        pitch_note::Note c4 = note::get("C4");
        
        CHECK(c4.acc == "");
        CHECK(c4.alt == 0);
        CHECK(c4.chroma == 0);
        CHECK(c4.coord.size() == 2);
        CHECK(c4.coord[0] == 0);
        CHECK(c4.coord[1] == 4);
        CHECK(c4.empty == false);
        CHECK(doctest::Approx(c4.freq.value_or(0)) == 261.6255653005986);
        CHECK(c4.height == 60);
        CHECK(c4.letter == "C");
        CHECK(c4.midi.value_or(-1) == 60);
        CHECK(c4.name == "C4");
        CHECK(c4.oct.value_or(-1) == 4);
        CHECK(c4.pc == "C");
        CHECK(c4.step == 0);
        
        // Check that note::get(note::get("C4")) is the same as note::get("C4")
        pitch_note::Note c4Again = note::get(c4.name);
        CHECK(c4.name == c4Again.name);
        CHECK(c4.midi.value_or(-1) == c4Again.midi.value_or(-1));
        CHECK(c4.chroma == c4Again.chroma);
    }
    
    SUBCASE("property shorthands") {
        CHECK(note::name("db") == "Db");
        CHECK(note::pitchClass("Ax4") == "A##");
        CHECK(note::chroma("db4") == 1);
        CHECK(note::midi("db4").value_or(-1) == 61);
        CHECK(doctest::Approx(note::freq("A4").value_or(0)) == 440);
    }
    
    SUBCASE("simplify") {
        CHECK(note::simplify("C#") == "C#");
        CHECK(note::simplify("C##") == "D");
        CHECK(note::simplify("C###") == "D#");
        CHECK(note::simplify("B#4") == "C5");
        
        std::vector<std::string> notes = split("C## C### F##4 Gbbb5 B#4 Cbb4");
        std::vector<std::string> expected = split("D D# G4 E5 C5 Bb3");
        
        std::vector<std::string> simplified;
        simplified.reserve(notes.size());
        for (const auto& n : notes) {
            simplified.push_back(note::simplify(n));
        }
        
        CHECK(simplified == expected);
        CHECK(note::simplify("x") == "");
    }
    
    SUBCASE("from midi") {
        CHECK(note::fromMidi(70) == "Bb4");
        
        std::vector<int> midiNotes = {60, 61, 62};
        std::vector<std::string> expected = {"C4", "Db4", "D4"};
        std::vector<std::string> actual;
        
        for (int m : midiNotes) {
            actual.push_back(note::fromMidi(m));
        }
        
        CHECK(actual == expected);
        
        std::vector<std::string> expectedSharps = {"C4", "C#4", "D4"};
        std::vector<std::string> actualSharps;
        
        for (int m : midiNotes) {
            actualSharps.push_back(note::fromMidiSharps(m));
        }
        
        CHECK(actualSharps == expectedSharps);
    }
    
    SUBCASE("names") {
        std::vector<std::string> expectedNames = {"C", "D", "E", "F", "G", "A", "B"};
        CHECK(note::names() == expectedNames);
        
        std::vector<std::string> inputNotes = {"fx", "bb", "nothing"};
        std::vector<std::string> expectedFiltered = {"F##", "Bb"};
        CHECK(note::names(inputNotes) == expectedFiltered);
    }
    
    SUBCASE("sortedNames") {
        std::vector<std::string> notes = split("c f g a b h j");
        std::vector<std::string> expected = split("C F G A B");

        CHECK(note::sortedNames(notes) == expected);
        
        notes = split("c f g a b h j j h b a g f c");
        expected = split("C C F F G G A A B B");
        CHECK(note::sortedNames(notes) == expected);
        
        notes = split("c2 c5 c1 c0 c6 c");
        expected = split("C C0 C1 C2 C5 C6");
        CHECK(note::sortedNames(notes) == expected);
        
        expected = split("C6 C5 C2 C1 C0 C");
        CHECK(note::sortedNames(notes, note::descending) == expected);
    }
    
    SUBCASE("sortedUniq") {
        std::vector<std::string> notes = split("a b c2 p c2 b c c3");
        std::vector<std::string> expected = split("C A B C2 C3");
        CHECK(note::sortedUniqNames(notes) == expected);
    }
    
    SUBCASE("transpose") {
        CHECK(note::transpose("A4", "3M") == "C#5");
        CHECK(note::tr("A4", "3M") == "C#5");
    }
    
    SUBCASE("transposeFrom") {
        auto transposer = note::transposeFrom("C4");
        CHECK(transposer("5P") == "G4");
        
        std::vector<std::string> intervals = {"1P", "3M", "5P"};
        std::vector<std::string> expected = {"C", "E", "G"};
        std::vector<std::string> result;
        
        auto fromC = note::transposeFrom("C");
        for (const auto& interval : intervals) {
            result.push_back(fromC(interval));
        }
        
        CHECK(result == expected);
    }
    
    SUBCASE("transposeBy") {
        auto transposer = note::transposeBy("5P");
        CHECK(transposer("C4") == "G4");
        
        std::vector<std::string> notes = {"C", "D", "E"};
        std::vector<std::string> expected = {"G", "A", "B"};
        std::vector<std::string> result;
        
        auto by5P = note::transposeBy("5P");
        for (const auto& note : notes) {
            result.push_back(by5P(note));
        }
        
        CHECK(result == expected);
    }
    
    SUBCASE("enharmonic") {
        CHECK(note::enharmonic("C#") == "Db");
        CHECK(note::enharmonic("C##") == "D");
        CHECK(note::enharmonic("C###") == "Eb");
        CHECK(note::enharmonic("B#4") == "C5");
        
        std::vector<std::string> notes = split("C## C### F##4 Gbbb5 B#4 Cbb4");
        std::vector<std::string> expected = split("D Eb G4 E5 C5 A#3");
        std::vector<std::string> result;
        
        for (const auto& n : notes) {
            result.push_back(note::enharmonic(n));
        }
        
        CHECK(result == expected);
        CHECK(note::enharmonic("x") == "");
        CHECK(note::enharmonic("F2", "E#") == "E#2");
        CHECK(note::enharmonic("B2", "Cb") == "Cb3");
        CHECK(note::enharmonic("C2", "B#") == "B#1");
        CHECK(note::enharmonic("F2", "Eb") == "");
    }
    
    SUBCASE("transposeFifths") {
        CHECK(note::transposeFifths("G4", 3) == "E6");
        CHECK(note::transposeFifths("G", 3) == "E");
        
        std::vector<int> fifths = {0, 1, 2, 3, 4, 5};
        std::vector<std::string> expectedNotes;
        
        for (int n : fifths) {
            expectedNotes.push_back(note::transposeFifths("C2", n));
        }
        
        std::vector<std::string> expected = {"C2", "G2", "D3", "A3", "E4", "B4"};
        CHECK(expectedNotes == expected);
        
        std::vector<int> sharpFifths = {0, 1, 2, 3, 4, 5, 6};
        std::vector<std::string> sharps;
        
        for (int n : sharpFifths) {
            sharps.push_back(note::transposeFifths("F#", n));
        }
        
        std::vector<std::string> expectedSharps = {"F#", "C#", "G#", "D#", "A#", "E#", "B#"};
        CHECK(sharps == expectedSharps);
        
        std::vector<int> flatFifths = {0, -1, -2, -3, -4, -5, -6};
        std::vector<std::string> flats;
        
        for (int n : flatFifths) {
            flats.push_back(note::transposeFifths("Bb", n));
        }
        
        std::vector<std::string> expectedFlats = {"Bb", "Eb", "Ab", "Db", "Gb", "Cb", "Fb"};
        CHECK(flats == expectedFlats);
    }
    
    SUBCASE("transposeOctaves") {
        std::vector<int> octs = {0, 1, 2, 3, 4};
        std::vector<std::string> octaves;
        
        for (int oct : octs) {
            octaves.push_back(note::transposeOctaves("C4", oct));
        }
        
        std::vector<std::string> expected = {"C4", "C5", "C6", "C7", "C8"};
        CHECK(octaves == expected);
        
        std::vector<int> negOcts = {-1, -2, -3, -4, -5};
        std::vector<std::string> negOctaves;
        
        for (int oct : negOcts) {
            negOctaves.push_back(note::transposeOctaves("C4", oct));
        }
        
        std::vector<std::string> expectedNeg = {"C3", "C2", "C1", "C0", "C-1"};
        CHECK(negOctaves == expectedNeg);
    }
    
    SUBCASE("fromFreq") {
        CHECK(note::fromFreq(440) == "A4");
        CHECK(note::fromFreq(444) == "A4");
        CHECK(note::fromFreq(470) == "Bb4");
        CHECK(note::fromFreqSharps(470) == "A#4");
        CHECK(note::fromFreq(0) == "");
        // We can't directly check NaN, but any invalid value should return ""
        CHECK(note::fromFreq(-1) == "");
    }
}