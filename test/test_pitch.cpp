#include "../doctest/doctest.h"
#include "tonalcpp/pitch.h"
#include <vector>
#include <limits>

using namespace tonalcpp;

// Pitch classes
const Pitch C = {0, 0, std::nullopt, std::nullopt};
const Pitch Cs = {0, 1, std::nullopt, std::nullopt};
const Pitch Cb = {0, -1, std::nullopt, std::nullopt};
const Pitch A = {5, 0, std::nullopt, std::nullopt};

// Notes
const Pitch C4 = {0, 0, 4, std::nullopt};
const Pitch A4 = {5, 0, 4, std::nullopt};
const Pitch Gs6 = {4, 1, 6, std::nullopt};

// Intervals
const Pitch P5 = {4, 0, 0, Direction::Ascending};
const Pitch P_5 = {4, 0, 0, Direction::Descending};

// Named pitch for testing - use our NamedPitch type
class TestNamedPitch : public NamedPitch {
public:
    TestNamedPitch(const std::string& n) {
        name = n;
    }
};

TEST_CASE("@tonaljs/pitch") {
    SUBCASE("isNamedPitch") {
        TestNamedPitch namedPitch {"C"};
        CHECK(isNamedPitch(&namedPitch) == true);
    }
    
    SUBCASE("height") {
        std::vector<Pitch> pitchClasses = {C, Cs, Cb, A};
        std::vector<int> pitchClassHeights;
        for (const auto& p : pitchClasses) {
            pitchClassHeights.push_back(height(p));
        }
        CHECK(pitchClassHeights == std::vector<int>{-1200, -1199, -1201, -1191});
        
        std::vector<Pitch> notes = {C4, A4, Gs6};
        std::vector<int> noteHeights;
        for (const auto& n : notes) {
            noteHeights.push_back(height(n));
        }
        CHECK(noteHeights == std::vector<int>{48, 57, 80});
        
        std::vector<Pitch> intervals = {P5, P_5};
        std::vector<int> intervalHeights;
        for (const auto& i : intervals) {
            intervalHeights.push_back(height(i));
        }
        CHECK(intervalHeights == std::vector<int>{7, -7});
    }
    
    SUBCASE("midi") {
        std::vector<Pitch> pitchClasses = {C, Cs, Cb, A};
        std::vector<std::optional<int>> pitchClassMidis;
        for (const auto& p : pitchClasses) {
            pitchClassMidis.push_back(midi(p));
        }
        
        for (const auto& m : pitchClassMidis) {
            CHECK_FALSE(m.has_value());
        }
        
        std::vector<Pitch> notes = {C4, A4, Gs6};
        std::vector<int> noteMidis;
        for (const auto& n : notes) {
            noteMidis.push_back(midi(n).value());
        }
        CHECK(noteMidis == std::vector<int>{60, 69, 92});
    }
    
    SUBCASE("chroma") {
        std::vector<Pitch> pitchClasses = {C, Cs, Cb, A};
        std::vector<int> pitchClassChromas;
        for (const auto& p : pitchClasses) {
            pitchClassChromas.push_back(chroma(p));
        }
        CHECK(pitchClassChromas == std::vector<int>{0, 1, 11, 9});
        
        std::vector<Pitch> notes = {C4, A4, Gs6};
        std::vector<int> noteChromas;
        for (const auto& n : notes) {
            noteChromas.push_back(chroma(n));
        }
        CHECK(noteChromas == std::vector<int>{0, 9, 8});
        
        std::vector<Pitch> intervals = {P5, P_5};
        std::vector<int> intervalChromas;
        for (const auto& i : intervals) {
            intervalChromas.push_back(chroma(i));
        }
        CHECK(intervalChromas == std::vector<int>{7, 7});
    }
    
    SUBCASE("coordinates") {
        // Pitch classes
        CHECK(coordinates(C) == PitchCoordinates{0});
        CHECK(coordinates(A) == PitchCoordinates{3});
        CHECK(coordinates(Cs) == PitchCoordinates{7});
        CHECK(coordinates(Cb) == PitchCoordinates{-7});
        
        // Notes
        CHECK(coordinates(C4) == PitchCoordinates{0, 4});
        CHECK(coordinates(A4) == PitchCoordinates{3, 3});
        
        // Intervals
        CHECK(coordinates(P5) == PitchCoordinates{1, 0});
        CHECK(coordinates(P_5) == PitchCoordinates{-1, -0});
    }
    
    SUBCASE("pitch") {
        // Test using the function name in the TS test (pitch) which should map to pitchFromCoordinates
        Pitch fromCoord = pitchFromCoordinates({0});
        CHECK(fromCoord.step == C.step);
        CHECK(fromCoord.alt == C.alt);
        CHECK_FALSE(fromCoord.oct.has_value());
        CHECK_FALSE(fromCoord.dir.has_value());
        
        fromCoord = pitchFromCoordinates({7});
        CHECK(fromCoord.step == Cs.step);
        CHECK(fromCoord.alt == Cs.alt);
        CHECK_FALSE(fromCoord.oct.has_value());
        CHECK_FALSE(fromCoord.dir.has_value());
        
        // Explicit tests matching TS version
        Pitch p1 = pitchFromCoordinates({0});
        CHECK(p1.step == C.step);
        CHECK(p1.alt == C.alt);
        
        Pitch p2 = pitchFromCoordinates({7});
        CHECK(p2.step == Cs.step);
        CHECK(p2.alt == Cs.alt);
    }
    
    SUBCASE("isPitch") {
        // Valid pitch with various common values
        Pitch validPitch1 = {0, 0, std::nullopt, std::nullopt};
        CHECK(isPitch(&validPitch1) == true);
        
        Pitch validPitch2 = {2, -1, std::nullopt, std::nullopt};
        CHECK(isPitch(&validPitch2) == true); 
        
        Pitch validPitch3 = {4, 1, 4, std::nullopt};
        CHECK(isPitch(&validPitch3) == true);
        
        Pitch validPitch4 = {4, 0, 0, Direction::Ascending};
        CHECK(isPitch(&validPitch4) == true);
        
        // Invalid step value
        Pitch invalidStepPitch = {std::numeric_limits<int>::min(), 0, std::nullopt, std::nullopt};
        CHECK(isPitch(&invalidStepPitch) == false);
        
        // Invalid alt value
        Pitch invalidAltPitch = {0, std::numeric_limits<int>::min(), std::nullopt, std::nullopt};
        CHECK(isPitch(&invalidAltPitch) == false);
        
        // Both step and alt invalid
        Pitch bothInvalidPitch = {-1, -100, std::nullopt, std::nullopt};
        CHECK(isPitch(&bothInvalidPitch) == false);
        
        // Null pointer test
        CHECK(isPitch(nullptr) == false);
    }
}