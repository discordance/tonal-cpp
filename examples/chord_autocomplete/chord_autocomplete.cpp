#include "chord_autocomplete.h"
#include <iostream>

namespace tonalcpp {
namespace examples {

ChordAutocomplete::ChordAutocomplete() {
    // Initialize with all available chord types and symbols
    chordTypeNames = chord_type::names();
    chordSymbols = chord_type::symbols();
    
    // Initialize with common tonic notes (we'll use a standard set)
    allTonicNotes = {
        "C", "C#", "Db", "D", "D#", "Eb", "E", "F", 
        "F#", "Gb", "G", "G#", "Ab", "A", "A#", "Bb", "B"
    };
    
    // Initialize chord type cache
    initChordTypeCache();
}

std::vector<std::string> ChordAutocomplete::getSuggestions(const std::string& prefix, int limit) {
    std::vector<std::string> results;
    
    if (prefix.empty()) {
        // If no prefix, return common chords sorted by complexity
        results = {"C", "Cm", "F", "G", "Am", "C7", "G7", "Cmaj7"};
        return results;
    }
    
    // First, try to tokenize the input (in case it's a complete chord)
    auto tokens = chord::tokenize(prefix);
    std::string tonic = tokens[0];
    std::string chordType = tokens[1];
    
    // If we have a valid tonic, try to autocomplete the chord type
    if (!tonic.empty() && note::octave(tonic).has_value()) {
        auto suggestions = getSuggestionsForTonic(tonic, chordType, limit);
        // Sort by complexity
        if (!suggestions.empty()) {
            std::sort(suggestions.begin(), suggestions.end(), 
                [this](const std::string& a, const std::string& b) {
                    return getChordComplexity(a) < getChordComplexity(b);
                });
            return suggestions;
        }
    }
    
    // Otherwise, treat as a general chord search
    std::vector<std::string> unsortedResults;
    
    // Check for matches in complete chord names
    for (const auto& note : allTonicNotes) {
        if (unsortedResults.size() >= limit * 2) break; // Collect more than we need for sorting
        if (startsWith(note, prefix)) {
            unsortedResults.push_back(note);
            unsortedResults.push_back(note + "m");
            unsortedResults.push_back(note + "7");
            if (unsortedResults.size() >= limit * 2) break;
        }
    }
    
    // Check for matches with a chord type (e.g., "Cmaj")
    for (const auto& note : allTonicNotes) {
        for (const auto& symbol : chordSymbols) {
            if (unsortedResults.size() >= limit * 2) break;
            
            std::string fullChord = note + symbol;
            if (startsWith(fullChord, prefix)) {
                unsortedResults.push_back(fullChord);
            }
        }
        
        for (const auto& name : chordTypeNames) {
            if (unsortedResults.size() >= limit * 2) break;
            if (name.empty()) continue;
            
            std::string fullChord = note + " " + name;
            if (startsWith(fullChord, prefix)) {
                unsortedResults.push_back(fullChord);
            }
        }
    }
    
    // Sort results by complexity
    std::sort(unsortedResults.begin(), unsortedResults.end(), 
        [this](const std::string& a, const std::string& b) {
            return getChordComplexity(a) < getChordComplexity(b);
        });
    
    // Take only up to limit results
    for (const auto& result : unsortedResults) {
        if (results.size() >= limit) break;
        results.push_back(result);
    }
    
    return results;
}

std::vector<std::pair<std::string, std::string>> ChordAutocomplete::getDetailedSuggestions(
    const std::string& prefix, int limit) {
    
    std::vector<std::pair<std::string, std::string>> results;
    
    // Get basic suggestions first
    auto suggestions = getSuggestions(prefix, limit);
    
    // Add descriptions to each suggestion
    for (const auto& suggestion : suggestions) {
        results.push_back({suggestion, getChordDescription(suggestion)});
    }
    
    return results;
}

std::vector<std::string> ChordAutocomplete::getSuggestionsForTonic(
    const std::string& tonic, const std::string& typePrefix, int limit) {
    
    std::vector<std::string> unsortedResults;
    auto validNote = note::get(tonic);
    
    if (validNote.empty) {
        return unsortedResults; // Not a valid tonic
    }
    
    std::string validTonic = validNote.name;
    
    // If no type prefix, suggest common chord types for this tonic
    if (typePrefix.empty()) {
        // Add simple chords first (the sorting will ensure they stay at the top)
        unsortedResults.push_back(validTonic);
        unsortedResults.push_back(validTonic + "m");
        unsortedResults.push_back(validTonic + "7");
        unsortedResults.push_back(validTonic + "maj7");
        unsortedResults.push_back(validTonic + "m7");
        unsortedResults.push_back(validTonic + "6");
        unsortedResults.push_back(validTonic + "9");
        unsortedResults.push_back(validTonic + "sus4");
        
        // Add more complex chords to have a variety of options
        unsortedResults.push_back(validTonic + "dim");
        unsortedResults.push_back(validTonic + "aug");
        unsortedResults.push_back(validTonic + "13");
        unsortedResults.push_back(validTonic + "m9");
        
        // Sort by complexity
        std::sort(unsortedResults.begin(), unsortedResults.end(), 
            [this](const std::string& a, const std::string& b) {
                return getChordComplexity(a) < getChordComplexity(b);
            });
        
        // Return only up to the limit
        std::vector<std::string> results;
        for (size_t i = 0; i < unsortedResults.size() && i < limit; ++i) {
            results.push_back(unsortedResults[i]);
        }
        
        return results;
    }
    
    // With type prefix, find matching chord types
    for (const auto& symbol : chordSymbols) {
        if (startsWith(symbol, typePrefix)) {
            unsortedResults.push_back(validTonic + symbol);
        }
    }
    
    // Also check formal names
    for (const auto& name : chordTypeNames) {
        if (startsWith(name, typePrefix)) {
            unsortedResults.push_back(validTonic + " " + name);
        }
    }
    
    // Sort by complexity
    std::sort(unsortedResults.begin(), unsortedResults.end(), 
        [this](const std::string& a, const std::string& b) {
            return getChordComplexity(a) < getChordComplexity(b);
        });
    
    // Return only up to the limit
    std::vector<std::string> results;
    for (size_t i = 0; i < unsortedResults.size() && i < limit; ++i) {
        results.push_back(unsortedResults[i]);
    }
    
    return results;
}

bool ChordAutocomplete::startsWith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    
    // Case-sensitive check for 'M' for Major chords, but only when 'M' is a single character
    // after the tonic (i.e., "CM" for C major, but not "CMa" or "CMaj")
    for (size_t i = 0; i < prefix.length(); i++) {
        if (prefix[i] == 'M' && 
            // Only enforce case sensitivity for standalone 'M' (not part of "Ma", "Maj", etc.)
            (i == prefix.length() - 1 || !std::isalpha(prefix[i+1]))) {
            // For standalone M, check exact case match
            if (str[i] != 'M') {
                return false;
            }
        } else {
            // For other characters, stay case-insensitive
            if (std::tolower(str[i]) != std::tolower(prefix[i])) {
                return false;
            }
        }
    }
    
    return true;
}

std::string ChordAutocomplete::getChordDescription(const std::string& chordName) {
    chord::Chord chordObj = chord::get(chordName);
    
    if (!chordObj.tonic.has_value() && chordObj.intervals.empty()) {
        return "Unknown chord";
    }
    
    std::string description;
    
    // Add tonic if available
    if (chordObj.tonic.has_value()) {
        description += chordObj.tonic.value() + " ";
    }
    
    // Add full chord type name if available
    if (!chordObj.name.empty()) {
        description += chordObj.name;
    } else if (!chordObj.aliases.empty()) {
        description += chordObj.aliases[0];
    } else {
        description += "chord";
    }
    
    // Add notes
    description += " [";
    for (size_t i = 0; i < chordObj.notes.size(); i++) {
        description += chordObj.notes[i];
        if (i < chordObj.notes.size() - 1) {
            description += ", ";
        }
    }
    description += "]";
    
    return description;
}

void ChordAutocomplete::initChordTypeCache() {
    auto types = chord_type::all();
    for (const auto& type : types) {
        // Cache by name
        if (!type.name.empty()) {
            chordTypeCache[type.name] = type;
        }
        
        // Cache by aliases
        for (const auto& alias : type.aliases) {
            chordTypeCache[alias] = type;
        }
    }
}

int ChordAutocomplete::getChordComplexity(const std::string& chordName) {
    chord::Chord chordObj = chord::get(chordName);
    
    // If it's an invalid chord, give it high complexity
    if (!chordObj.tonic.has_value() && chordObj.intervals.empty()) {
        return 100;
    }
    
    // Base complexity on number of notes in the chord
    int complexity = chordObj.notes.size();
    
    // More complexity for extended, altered, and complex chords
    if (chordName.find("maj") != std::string::npos) complexity += 1;
    if (chordName.find("aug") != std::string::npos) complexity += 2;
    if (chordName.find("dim") != std::string::npos) complexity += 2;
    if (chordName.find("sus") != std::string::npos) complexity += 1;
    
    // Number-based complexity (7, 9, 11, 13 chords are progressively more complex)
    if (chordName.find("7") != std::string::npos) complexity += 2;
    if (chordName.find("9") != std::string::npos) complexity += 3;
    if (chordName.find("11") != std::string::npos) complexity += 4;
    if (chordName.find("13") != std::string::npos) complexity += 5;
    
    // Altered notes add complexity
    if (chordName.find("#") != std::string::npos || 
        chordName.find("b") != std::string::npos) complexity += 2;
    
    // Simple major and minor chords are the simplest
    if (chordName.length() == 1) return 1; // Just the root note
    if (chordName.length() == 2 && chordName[1] == 'm') return 2; // Minor chord
    if (chordName.length() == 1 || 
        (chordName.length() == 2 && (chordName[1] == '#' || chordName[1] == 'b'))) return 2; // Major chord
    
    return complexity;
}

} // namespace examples
} // namespace tonalcpp