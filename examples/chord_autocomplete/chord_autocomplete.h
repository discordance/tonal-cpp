#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "tonalcpp/chord.h"
#include "tonalcpp/chord_type.h"
#include "tonalcpp/note.h"

namespace tonalcpp {
namespace examples {

/**
 * A chord autocomplete utility that provides suggestions based on prefix inputs
 */
class ChordAutocomplete {
public:
    ChordAutocomplete();

    /**
     * Get chord suggestions based on a prefix
     * 
     * @param prefix The user input to get suggestions for
     * @param limit Maximum number of suggestions to return
     * @return Vector of chord suggestions
     */
    std::vector<std::string> getSuggestions(const std::string& prefix, int limit = 20);

    /**
     * Get chord suggestions including information about the chord
     * 
     * @param prefix The user input to get suggestions for
     * @param limit Maximum number of suggestions to return
     * @return Vector of pairs (chord name, description)
     */
    std::vector<std::pair<std::string, std::string>> getDetailedSuggestions(
        const std::string& prefix, int limit = 20);

    /**
     * Get chord suggestions for a specific tonic/root note
     * 
     * @param tonic The tonic note
     * @param typePrefix The chord type prefix
     * @param limit Maximum number of suggestions to return
     * @return Vector of chord suggestions
     */
    std::vector<std::string> getSuggestionsForTonic(
        const std::string& tonic, const std::string& typePrefix = "", int limit = 20);

private:
    // Cache of chord types and symbols
    std::vector<std::string> chordTypeNames;
    std::vector<std::string> chordSymbols;
    std::vector<std::string> allTonicNotes;
    
    // Cached chord types for faster lookup
    std::unordered_map<std::string, chord_type::ChordType> chordTypeCache;

    // Helper function to check if a string starts with a prefix
    bool startsWith(const std::string& str, const std::string& prefix);
    
    // Helper function to generate chord description
    std::string getChordDescription(const std::string& chordName);
    
    // Initialize the chord type cache
    void initChordTypeCache();
    
    // Calculate chord complexity for sorting (lower is simpler)
    int getChordComplexity(const std::string& chordName);
};

} // namespace examples
} // namespace tonalcpp