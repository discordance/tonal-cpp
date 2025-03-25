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
    std::unordered_set<std::string> resultSet; // Use a set to avoid duplicates
    std::vector<std::string> unsortedResults;
    
    // Check for matches in complete chord names - exact match for notes to avoid C# when searching for C
    for (const auto& note : allTonicNotes) {
        if (resultSet.size() >= limit * 2) break; // Collect more than we need for sorting
        
        // For note prefixes, we want exact matches to avoid C# when searching for C
        bool noteMatches = false;
        
        // Single character prefixes (like "C")
        if (prefix.length() == 1) {
            noteMatches = (note == prefix);
        } 
        // Two character prefixes that could be complete notes (like "C#" or "Db")
        else if (prefix.length() == 2 && (prefix[1] == '#' || prefix[1] == 'b')) {
            noteMatches = (note == prefix);
        }
        // Other prefixes - use regular prefix matching
        else if (startsWith(note, prefix)) {
            noteMatches = true;
        }
        
        if (noteMatches) {
            if (resultSet.insert(note).second) unsortedResults.push_back(note);
            if (resultSet.insert(note + "m").second) unsortedResults.push_back(note + "m");
            if (resultSet.insert(note + "7").second) unsortedResults.push_back(note + "7");
            if (resultSet.size() >= limit * 2) break;
        }
    }
    
    // Check for matches with a chord type (e.g., "Cmaj")
    for (const auto& note : allTonicNotes) {
        bool noteMatches = false;
        
        // Single character prefixes (like "C") - exact match only
        if (prefix.length() == 1) {
            noteMatches = (note == prefix);
        } 
        // Two character prefixes that could be complete notes (like "C#" or "Db") - exact match only
        else if (prefix.length() == 2 && (prefix[1] == '#' || prefix[1] == 'b')) {
            noteMatches = (note == prefix);
        }
        // Other prefixes (like "Cmaj") - use startsWith for chord name matches
        else {
            // If the prefix starts with this note, it might be looking for a chord type
            // Example: "Cm" should match C minor chords but not C# chords
            if (prefix.length() >= note.length() && prefix.substr(0, note.length()) == note) {
                noteMatches = true;
            }
        }
        
        if (noteMatches) {
            // If prefix is exactly the note name or if we're looking for chord types
            if (prefix == note || prefix.length() > note.length()) {
                for (const auto& symbol : chordSymbols) {
                    if (resultSet.size() >= limit * 2) break;
                    
                    std::string fullChord = note + symbol;
                    // If prefix is just the note, add all symbols
                    // If prefix is longer, check if the whole chord matches the prefix
                    if ((prefix.length() <= note.length() && prefix == note) ||
                        (prefix.length() > note.length() && startsWith(fullChord, prefix))) {
                        if (resultSet.insert(fullChord).second) {
                            unsortedResults.push_back(fullChord);
                        }
                    }
                }
                
                for (const auto& name : chordTypeNames) {
                    if (resultSet.size() >= limit * 2) break;
                    if (name.empty()) continue;
                    
                    std::string fullChord = note + " " + name;
                    // If prefix is just the note, add all names
                    // If prefix is longer, check if the chord matches the prefix
                    if ((prefix.length() <= note.length() && prefix == note) ||
                        (prefix.length() > note.length() && startsWith(fullChord, prefix))) {
                        if (resultSet.insert(fullChord).second) {
                            unsortedResults.push_back(fullChord);
                        }
                    }
                }
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
    std::unordered_set<std::string> resultSet; // Use a set to avoid duplicates
    auto validNote = note::get(tonic);
    
    if (validNote.empty) {
        return unsortedResults; // Not a valid tonic
    }
    
    std::string validTonic = validNote.name;
    
    // If no type prefix, suggest common chord types for this tonic
    if (typePrefix.empty()) {
        // Add simple chords first (the sorting will ensure they stay at the top)
        if (resultSet.insert(validTonic).second) unsortedResults.push_back(validTonic);
        if (resultSet.insert(validTonic + "m").second) unsortedResults.push_back(validTonic + "m");
        if (resultSet.insert(validTonic + "7").second) unsortedResults.push_back(validTonic + "7");
        if (resultSet.insert(validTonic + "maj7").second) unsortedResults.push_back(validTonic + "maj7");
        if (resultSet.insert(validTonic + "m7").second) unsortedResults.push_back(validTonic + "m7");
        if (resultSet.insert(validTonic + "6").second) unsortedResults.push_back(validTonic + "6");
        if (resultSet.insert(validTonic + "9").second) unsortedResults.push_back(validTonic + "9");
        if (resultSet.insert(validTonic + "sus4").second) unsortedResults.push_back(validTonic + "sus4");
        
        // Add more complex chords to have a variety of options
        if (resultSet.insert(validTonic + "dim").second) unsortedResults.push_back(validTonic + "dim");
        if (resultSet.insert(validTonic + "aug").second) unsortedResults.push_back(validTonic + "aug");
        if (resultSet.insert(validTonic + "13").second) unsortedResults.push_back(validTonic + "13");
        if (resultSet.insert(validTonic + "m9").second) unsortedResults.push_back(validTonic + "m9");
    } else if (typePrefix == "m" || typePrefix == "min") {
        // Special case for minor chord search - prioritize minor variations
        if (resultSet.insert(validTonic + "m").second) unsortedResults.push_back(validTonic + "m");
        if (resultSet.insert(validTonic + "m7").second) unsortedResults.push_back(validTonic + "m7");
        if (resultSet.insert(validTonic + "m9").second) unsortedResults.push_back(validTonic + "m9");
        if (resultSet.insert(validTonic + "m11").second) unsortedResults.push_back(validTonic + "m11");
        if (resultSet.insert(validTonic + "m13").second) unsortedResults.push_back(validTonic + "m13");
        if (resultSet.insert(validTonic + "m6").second) unsortedResults.push_back(validTonic + "m6");
        if (resultSet.insert(validTonic + "m6/9").second) unsortedResults.push_back(validTonic + "m6/9");
        if (resultSet.insert(validTonic + "dim").second) unsortedResults.push_back(validTonic + "dim");
    } else if (typePrefix == "M" || typePrefix == "maj") {
        // Special case for major chord search - prioritize major variations
        if (resultSet.insert(validTonic + "M").second) unsortedResults.push_back(validTonic + "M");
        if (resultSet.insert(validTonic + "maj7").second) unsortedResults.push_back(validTonic + "maj7");
        if (resultSet.insert(validTonic + "maj9").second) unsortedResults.push_back(validTonic + "maj9");
        if (resultSet.insert(validTonic + "maj13").second) unsortedResults.push_back(validTonic + "maj13");
        if (resultSet.insert(validTonic + "6").second) unsortedResults.push_back(validTonic + "6");
        if (resultSet.insert(validTonic + "6/9").second) unsortedResults.push_back(validTonic + "6/9");
        if (resultSet.insert(validTonic + "aug").second) unsortedResults.push_back(validTonic + "aug");
        
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
            std::string fullChord = validTonic + symbol;
            if (resultSet.insert(fullChord).second) {
                unsortedResults.push_back(fullChord);
            }
        }
    }
    
    // Also check formal names
    for (const auto& name : chordTypeNames) {
        if (startsWith(name, typePrefix)) {
            std::string fullChord = validTonic + " " + name;
            if (resultSet.insert(fullChord).second) {
                unsortedResults.push_back(fullChord);
            }
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
    
    // Case-sensitive check for 'M' for Major and 'm' for minor chords
    // This is critical for distinguishing between "CM" (C major) and "Cm" (C minor)
    for (size_t i = 0; i < prefix.length(); i++) {
        // Handle 'M' and 'm' with exact case matching when they appear right after the tonic
        if ((prefix[i] == 'M' || prefix[i] == 'm') && 
            // Make sure it's not part of "Maj", "min", etc.
            (i == prefix.length() - 1 || !std::isalpha(prefix[i+1]))) {
            // For standalone M or m, check exact case match
            if (str[i] != prefix[i]) {
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
    
    // Check for simple chord patterns first
    // Root notes, major and minor triads are the simplest
    
    // Just the root note (C)
    if (chordName.length() == 1 || 
        (chordName.length() == 2 && (chordName[1] == '#' || chordName[1] == 'b'))) {
        return 1; // Root note or root with accidental
    }
    
    // Check for minor triad (Cm) - ensure this is prioritized for 'm' search
    if (chordName.length() == 2 && chordName[1] == 'm') {
        return 2; // Minor triad
    }
    
    // Check for major triad (CM)
    if (chordName.length() == 2 && chordName[1] == 'M') {
        return 2; // Major triad (same complexity level as minor, but will be sorted differently)
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
    
    return complexity;
}

} // namespace examples
} // namespace tonalcpp