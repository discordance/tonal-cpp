#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include "chord_autocomplete.h"

// Demo script that shows chord autocompletion for various inputs
int main() {
    std::cout << "Chord Autocomplete Demo\n";
    std::cout << "======================\n\n";
    
    // Create the autocomplete instance
    tonalcpp::examples::ChordAutocomplete autocomplete;
    
    // Define a list of test inputs
    std::vector<std::string> testInputs = {
        "", 
        "C", 
        "Dm", 
        "G7", 
        "Cmaj", 
        "Ami", 
        "Dsus", 
        "Eaug", 
        "F#", 
        "Bbm",
        "7",
        // Test cases for the M handling
        "CM",
        "Cm",
        "CMa",
        "CMaj"
    };
    
    // Test each input
    for (const auto& input : testInputs) {
        std::cout << "Suggestions for '" << (input.empty() ? "[empty]" : input) << "':\n";
        
        auto suggestions = autocomplete.getDetailedSuggestions(input, 5);
        if (suggestions.empty()) {
            std::cout << "  No suggestions found.\n";
        } else {
            for (const auto& [chord, description] : suggestions) {
                std::cout << "  - " << std::setw(10) << std::left << chord 
                        << " : " << description << "\n";
            }
        }
        std::cout << "\n";
    }
    
    // Demonstrate searching by tonic
    std::vector<std::pair<std::string, std::string>> tonicTests = {
        {"D", ""},
        {"G", "m"},
        {"F#", "maj"},
        {"Bb", "sus"}
    };
    
    for (const auto& [tonic, typePrefix] : tonicTests) {
        std::cout << "Suggestions for tonic '" << tonic << "' with type prefix '" 
                << (typePrefix.empty() ? "[empty]" : typePrefix) << "':\n";
        
        auto suggestions = autocomplete.getSuggestionsForTonic(tonic, typePrefix, 5);
        if (suggestions.empty()) {
            std::cout << "  No suggestions found.\n";
        } else {
            for (const auto& chord : suggestions) {
                std::cout << "  - " << chord << "\n";
            }
        }
        std::cout << "\n";
    }
    
    return 0;
}