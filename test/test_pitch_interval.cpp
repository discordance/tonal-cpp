#include "../doctest/doctest.h"
#include "tonalcpp/pitch_interval.h"
#include "test_helpers.h"
#include <vector>
#include <string>

using namespace tonalcpp;
using namespace tonalcpp::pitch_interval;
using namespace tonalcpp::pitch;

TEST_CASE("@tonaljs/pitch-interval") {
    SUBCASE("tokenize") {
        auto tokens1 = tokenizeInterval("-2M");
        CHECK(tokens1.first == "-2");
        CHECK(tokens1.second == "M");

        auto tokens2 = tokenizeInterval("M-3");
        CHECK(tokens2.first == "-3");
        CHECK(tokens2.second == "M");
    }

    SUBCASE("interval from string") {
        SUBCASE("has all properties") {
            Interval ivl = interval("4d");
            CHECK_FALSE(ivl.empty);
            CHECK(ivl.name == "4d");
            CHECK(ivl.num == 4);
            CHECK(ivl.q == "d");
            CHECK(ivl.type == IntervalType::Perfectable);
            CHECK(ivl.alt == -1);
            CHECK(ivl.chroma == 4);
            CHECK(ivl.dir.value() == Direction::Ascending);
            CHECK(ivl.coord.size() == 3);
            CHECK(ivl.coord[0] == -8);
            CHECK(ivl.coord[1] == 5);
            CHECK(ivl.oct == 0);
            CHECK(ivl.semitones == 4);
            CHECK(ivl.simple == 4);
            CHECK(ivl.step == 3);
        }

        SUBCASE("accepts interval name as parameter") {
            Interval ivl1 = interval("5P");
            Interval ivl2 = interval(ivl1.name);
            CHECK(ivl1.name == ivl2.name);
            CHECK(ivl1.semitones == ivl2.semitones);
            CHECK(ivl1.chroma == ivl2.chroma);
        }

        SUBCASE("name") {
            auto names = [](const std::string& src) {
                std::vector<std::string> result;
                for (const auto& s : split(src)) {
                    result.push_back(interval(s).name);
                }
                std::string joined;
                for (size_t i = 0; i < result.size(); ++i) {
                    if (i > 0) joined += " ";
                    joined += result[i];
                }
                return joined;
            };

            CHECK(names("1P 2M 3M 4P 5P 6M 7M") == "1P 2M 3M 4P 5P 6M 7M");
            CHECK(names("P1 M2 M3 P4 P5 M6 M7") == "1P 2M 3M 4P 5P 6M 7M");
            CHECK(names("-1P -2M -3M -4P -5P -6M -7M") == "-1P -2M -3M -4P -5P -6M -7M");
            CHECK(names("P-1 M-2 M-3 P-4 P-5 M-6 M-7") == "-1P -2M -3M -4P -5P -6M -7M");
            
            CHECK(interval("not-an-interval").empty);
            CHECK(interval("2P").empty);
        }

        SUBCASE("q") {
            auto q = [](const std::string& str) {
                std::vector<std::string> result;
                for (const auto& i : split(str)) {
                    result.push_back(interval(i).q);
                }
                return result;
            };

            CHECK(q("1dd 1d 1P 1A 1AA") == std::vector<std::string>{"dd", "d", "P", "A", "AA"});
            CHECK(q("2dd 2d 2m 2M 2A 2AA") == std::vector<std::string>{"dd", "d", "m", "M", "A", "AA"});
        }

        SUBCASE("alt") {
            auto alt = [](const std::string& str) {
                std::vector<int> result;
                for (const auto& i : split(str)) {
                    result.push_back(interval(i).alt);
                }
                return result;
            };

            CHECK(alt("1dd 2dd 3dd 4dd") == std::vector<int>{-2, -3, -3, -2});
        }

        SUBCASE("simple") {
            auto simple = [](const std::string& str) {
                std::vector<int> result;
                for (const auto& i : split(str)) {
                    result.push_back(interval(i).simple);
                }
                return result;
            };

            CHECK(simple("1P 2M 3M 4P") == std::vector<int>{1, 2, 3, 4});
            CHECK(simple("8P 9M 10M 11P") == std::vector<int>{8, 2, 3, 4});
            CHECK(simple("-8P -9M -10M -11P") == std::vector<int>{-8, -2, -3, -4});
        }
    }

    SUBCASE("interval from pitch props") {
        SUBCASE("requires step, alt and dir") {
            // Test creating intervals from pitch properties using intervalPitchName
            Pitch p1{0, 0, std::nullopt, Direction::Ascending};
            CHECK(interval(intervalPitchName(p1)).name == "1P");
            
            Pitch p2{0, -2, std::nullopt, Direction::Ascending};
            CHECK(interval(intervalPitchName(p2)).name == "1dd");
            
            Pitch p3{1, 1, std::nullopt, Direction::Ascending};
            CHECK(interval(intervalPitchName(p3)).name == "2A");
            
            Pitch p4{2, -2, std::nullopt, Direction::Ascending};
            CHECK(interval(intervalPitchName(p4)).name == "3d");
            
            Pitch p5{1, 1, std::nullopt, Direction::Descending};
            CHECK(interval(intervalPitchName(p5)).name == "-2A");
            
            // Test invalid input
            CHECK(interval("invalid").empty);
            
            // Test invalid step value
            Pitch invalid_step{1000, 0, std::nullopt, std::nullopt};
            CHECK(interval(intervalPitchName(invalid_step)).empty);
        }

        SUBCASE("accepts octave") {
            // Test with intervalPitchName and then interval for octave-based intervals
            Pitch p6{0, 0, 0, Direction::Ascending};
            CHECK(interval(intervalPitchName(p6)).name == "1P");
            
            Pitch p7{0, -1, 1, Direction::Descending};
            CHECK(interval(intervalPitchName(p7)).name == "-8d");
            
            Pitch p8{0, 1, 2, Direction::Descending};
            CHECK(interval(intervalPitchName(p8)).name == "-15A");
            
            Pitch p9{1, -1, 1, Direction::Descending};
            CHECK(interval(intervalPitchName(p9)).name == "-9m");
            
            Pitch p10{0, 0, 0, Direction::Ascending};
            CHECK(interval(intervalPitchName(p10)).name == "1P");
        }
    }
}