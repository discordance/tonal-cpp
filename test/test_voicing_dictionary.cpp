#include "doctest.h"
#include "tonalcpp/voicing_dictionary.h"
#include <vector>
#include <string>

using namespace tonalcpp::voicing_dictionary;

TEST_CASE("VoicingDictionary lookup") {
    SUBCASE("lookup major triad") {
        std::vector<std::string> expected = {"1P 3M 5P", "3M 5P 8P", "5P 8P 10M"};
        auto result = lookup("M", triads);
        REQUIRE(result.has_value());
        CHECK(result.value() == expected);
    }
    
    SUBCASE("lookup empty string") {
        std::vector<std::string> expected = {"1P 3M 5P", "3M 5P 8P", "5P 8P 10M"};
        auto result = lookup("", triads);
        REQUIRE(result.has_value());
        CHECK(result.value() == expected);
    }
    
    SUBCASE("lookup custom dictionary") {
        VoicingDictionary custom = {{"minor", {"1P 3m 5P"}}};
        std::vector<std::string> expected = {"1P 3m 5P"};
        auto result = lookup("minor", custom);
        REQUIRE(result.has_value());
        CHECK(result.value() == expected);
    }
    
    SUBCASE("lookup non-existent") {
        auto result = lookup("nonexistent", triads);
        CHECK_FALSE(result.has_value());
    }
}