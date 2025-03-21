#include "doctest.h"
#include "tonalcpp/scale_type.h"
#include "tonalcpp/pcset.h"

using namespace tonalcpp;

TEST_CASE("Scale Type - dictionary") {
    SUBCASE("list names") {
        CHECK(scale_type::all().size() == 92);
        // Check if sorted (first item is "major pentatonic")
        CHECK(scale_type::all()[0].name == "major pentatonic");
    }

    SUBCASE("get scale") {
        scale_type::ScaleType major = scale_type::get("major");

        CHECK(major.empty == false);
        CHECK(major.setNum == 2773);
        CHECK(major.name == "major");
        CHECK(major.intervals == std::vector<std::string>{"1P", "2M", "3M", "4P", "5P", "6M", "7M"});
        CHECK(major.aliases == std::vector<std::string>{"ionian"});
        CHECK(major.chroma == "101011010101");
        CHECK(major.normalized == "101010110101");
    }

    SUBCASE("not valid get type") {
        scale_type::ScaleType unknown = scale_type::get("unknown");
        CHECK(unknown.empty == true);
        CHECK(unknown.name == "");
        CHECK(unknown.setNum == 0);
        CHECK(unknown.aliases.empty());
        CHECK(unknown.chroma == "000000000000");
        CHECK(unknown.intervals.empty());
        CHECK(unknown.normalized == "000000000000");
    }

    SUBCASE("add a scale type") {
        // First remove all existing scales
        scale_type::removeAll();
        
        // Add a new scale type
        scale_type::add({"1P", "5P"}, "quinta");
        CHECK(scale_type::get("quinta").chroma == "100000010000");
        
        // Add again with aliases
        scale_type::add({"1P", "5P"}, "quinta", {"q", "Q"});
        CHECK(scale_type::get("q").name == "quinta");
        CHECK(scale_type::get("Q").name == "quinta");
        
        // Re-initialize for other tests
        scale_type::initialize();
    }

    SUBCASE("major modes") {
        scale_type::ScaleType major = scale_type::get("major");
        std::vector<std::string> chromas = pcset::modes(major.intervals, true);
        
        std::vector<std::string> names;
        for (const auto& chroma : chromas) {
            names.push_back(scale_type::get(chroma).name);
        }
        
        std::vector<std::string> expected = {
            "major", "dorian", "phrygian", "lydian", "mixolydian", "minor", "locrian"
        };
        CHECK(names == expected);
    }

    SUBCASE("harmonic minor modes") {
        scale_type::ScaleType harmonicMinor = scale_type::get("harmonic minor");
        std::vector<std::string> chromas = pcset::modes(harmonicMinor.intervals, true);
        
        std::vector<std::string> names;
        for (const auto& chroma : chromas) {
            names.push_back(scale_type::get(chroma).name);
        }
        
        std::vector<std::string> expected = {
            "harmonic minor", "locrian 6", "major augmented", "dorian #4", 
            "phrygian dominant", "lydian #9", "ultralocrian"
        };
        CHECK(names == expected);
    }

    SUBCASE("melodic minor modes") {
        scale_type::ScaleType melodicMinor = scale_type::get("melodic minor");
        std::vector<std::string> chromas = pcset::modes(melodicMinor.intervals, true);
        
        std::vector<std::string> names;
        for (const auto& chroma : chromas) {
            names.push_back(scale_type::get(chroma).name);
        }
        
        std::vector<std::string> expected = {
            "melodic minor", "dorian b2", "lydian augmented", "lydian dominant",
            "mixolydian b6", "locrian #2", "altered"
        };
        CHECK(names == expected);
    }

    SUBCASE("clear dictionary") {
        scale_type::removeAll();
        CHECK(scale_type::all().empty());
        CHECK(scale_type::keys().empty());
        
        // Re-initialize for other tests
        scale_type::initialize();
    }
}