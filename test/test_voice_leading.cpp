#include "doctest.h"
#include "tonalcpp/voice_leading.h"
#include <vector>
#include <string>

using namespace tonalcpp::voice_leading;

TEST_CASE("VoiceLeading topNoteDiff") {
    std::vector<std::vector<std::string>> voicings = {
        {"F3", "A3", "C4", "E4"},
        {"C4", "E4", "F4", "A4"}
    };
    
    std::vector<std::string> lastVoicing = {"C4", "E4", "G4", "B4"};
    
    std::vector<std::string> expected = {"C4", "E4", "F4", "A4"};
    
    CHECK(topNoteDiff(voicings, lastVoicing) == expected);
}