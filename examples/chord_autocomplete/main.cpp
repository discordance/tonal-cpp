#include <iostream>
#include <string>
#include <iomanip>
#include "chord_autocomplete.h"

int main() {
    std::cout << "Chord Autocomplete Example\n";
    std::cout << "==========================\n\n";
    
    // Create the autocomplete instance
    tonalcpp::examples::ChordAutocomplete autocomplete;
    
    // Example 1: Autocomplete with no input (suggesting common chords)
    std::cout << "Example 1: Common chord suggestions\n";
    auto suggestions1 = autocomplete.getSuggestions("");
    for (const auto& suggestion : suggestions1) {
        std::cout << "  - " << suggestion << "\n";
    }
    std::cout << "\n";
    
    // Example 2: Autocomplete with tonic only
    std::cout << "Example 2: Suggestions for tonic 'D'\n";
    auto suggestions2 = autocomplete.getSuggestionsForTonic("D");
    for (const auto& suggestion : suggestions2) {
        std::cout << "  - " << suggestion << "\n";
    }
    std::cout << "\n";
    
    // Example 3: Autocomplete with partial chord type
    std::cout << "Example 3: Suggestions for 'Cmaj'\n";
    auto suggestions3 = autocomplete.getSuggestions("Cmaj");
    for (const auto& suggestion : suggestions3) {
        std::cout << "  - " << suggestion << "\n";
    }
    std::cout << "\n";
    
    // Example 4: Detailed suggestions with chord information
    std::cout << "Example 4: Detailed suggestions for 'G7'\n";
    auto suggestions4 = autocomplete.getDetailedSuggestions("G7", 20);
    for (const auto& [chord, description] : suggestions4) {
        std::cout << "  - " << std::setw(10) << std::left << chord 
                  << " : " << description << "\n";
    }
    std::cout << "\n";
    
    // Example 5: Interactive mode
    std::cout << "Example 5: Interactive mode (type 'exit' to quit)\n";
    std::string input;
    while (true) {
        std::cout << "\nEnter chord prefix: ";
        std::getline(std::cin, input);
        
        if (input == "exit" || input == "quit") {
            break;
        }
        
        auto detailed = autocomplete.getDetailedSuggestions(input, 20);
        if (detailed.empty()) {
            std::cout << "No suggestions found.\n";
        } else {
            std::cout << "Suggestions:\n";
            for (const auto& [chord, description] : detailed) {
                std::cout << "  - " << std::setw(10) << std::left << chord 
                          << " : " << description << "\n";
            }
        }
    }
    
    return 0;
}