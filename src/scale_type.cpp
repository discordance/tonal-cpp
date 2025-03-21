#include "tonalcpp/scale_type.h"
#include <algorithm>

namespace tonalcpp {
namespace scale_type {

// Empty ScaleType constant
const ScaleType NoScaleType = {
    {
        "", // name
        true, // empty
        0, // setNum
        "000000000000", // chroma
        "000000000000", // normalized
        {} // intervals
    },
    {} // aliases
};

// Internal storage for the scale type dictionary
namespace {
    std::vector<ScaleType> dictionary;
    std::unordered_map<std::string, ScaleType*> index;
    std::unordered_map<int, ScaleType*> numIndex;
}

// SCALES data formatted as ["intervals", "name", "alias1", "alias2", ...]
// This replicates the data from the TypeScript version
const std::vector<std::vector<std::string>> SCALES = {
    // Basic scales
    {"1P 2M 3M 5P 6M", "major pentatonic", "pentatonic"},
    {"1P 2M 3M 4P 5P 6M 7M", "major", "ionian"},
    {"1P 2M 3m 4P 5P 6m 7m", "minor", "aeolian"},

    // Jazz common scales
    {"1P 2M 3m 3M 5P 6M", "major blues"},
    {"1P 3m 4P 5d 5P 7m", "minor blues", "blues"},
    {"1P 2M 3m 4P 5P 6M 7M", "melodic minor"},
    {"1P 2M 3m 4P 5P 6m 7M", "harmonic minor"},
    {"1P 2M 3M 4P 5P 6M 7m 7M", "bebop"},
    {"1P 2M 3m 4P 5d 6m 6M 7M", "diminished", "whole-half diminished"},

    // Modes
    {"1P 2M 3m 4P 5P 6M 7m", "dorian"},
    {"1P 2M 3M 4A 5P 6M 7M", "lydian"},
    {"1P 2M 3M 4P 5P 6M 7m", "mixolydian", "dominant"},
    {"1P 2m 3m 4P 5P 6m 7m", "phrygian"},
    {"1P 2m 3m 4P 5d 6m 7m", "locrian"},

    // 5-note scales
    {"1P 3M 4P 5P 7M", "ionian pentatonic"},
    {"1P 3M 4P 5P 7m", "mixolydian pentatonic", "indian"},
    {"1P 2M 4P 5P 6M", "ritusen"},
    {"1P 2M 4P 5P 7m", "egyptian"},
    {"1P 3M 4P 5d 7m", "neopolitan major pentatonic"},
    {"1P 3m 4P 5P 6m", "vietnamese 1"},
    {"1P 2m 3m 5P 6m", "pelog"},
    {"1P 2m 4P 5P 6m", "kumoijoshi"},
    {"1P 2M 3m 5P 6m", "hirajoshi"},
    {"1P 2m 4P 5d 7m", "iwato"},
    {"1P 2m 4P 5P 7m", "in-sen"},
    {"1P 3M 4A 5P 7M", "lydian pentatonic", "chinese"},
    {"1P 3m 4P 6m 7m", "malkos raga"},
    {"1P 3m 4P 5d 7m", "locrian pentatonic", "minor seven flat five pentatonic"},
    {"1P 3m 4P 5P 7m", "minor pentatonic", "vietnamese 2"},
    {"1P 3m 4P 5P 6M", "minor six pentatonic"},
    {"1P 2M 3m 5P 6M", "flat three pentatonic", "kumoi"},
    {"1P 2M 3M 5P 6m", "flat six pentatonic"},
    {"1P 2m 3M 5P 6M", "scriabin"},
    {"1P 3M 5d 6m 7m", "whole tone pentatonic"},
    {"1P 3M 4A 5A 7M", "lydian #5P pentatonic"},
    {"1P 3M 4A 5P 7m", "lydian dominant pentatonic"},
    {"1P 3m 4P 5P 7M", "minor #7M pentatonic"},
    {"1P 3m 4d 5d 7m", "super locrian pentatonic"},

    // 6-note scales
    {"1P 2M 3m 4P 5P 7M", "minor hexatonic"},
    {"1P 2A 3M 5P 5A 7M", "augmented"},
    {"1P 2M 4P 5P 6M 7m", "piongio"},
    {"1P 2m 3M 4A 6M 7m", "prometheus neopolitan"},
    {"1P 2M 3M 4A 6M 7m", "prometheus"},
    {"1P 2m 3M 5d 6m 7m", "mystery #1"},
    {"1P 2m 3M 4P 5A 6M", "six tone symmetric"},
    {"1P 2M 3M 4A 5A 6A", "whole tone", "messiaen's mode #1"},
    {"1P 2m 4P 4A 5P 7M", "messiaen's mode #5"},

    // 7-note scales
    {"1P 2M 3M 4P 5d 6m 7m", "locrian major", "arabian"},
    {"1P 2m 3M 4A 5P 6m 7M", "double harmonic lydian"},
    {"1P 2m 2A 3M 4A 6m 7m", "altered", "super locrian", "diminished whole tone", "pomeroy"},
    {"1P 2M 3m 4P 5d 6m 7m", "locrian #2", "half-diminished", "aeolian b5"},
    {"1P 2M 3M 4P 5P 6m 7m", "mixolydian b6", "melodic minor fifth mode", "hindu"},
    {"1P 2M 3M 4A 5P 6M 7m", "lydian dominant", "lydian b7", "overtone"},
    {"1P 2M 3M 4A 5A 6M 7M", "lydian augmented"},
    {"1P 2m 3m 4P 5P 6M 7m", "dorian b2", "phrygian #6", "melodic minor second mode"},
    {"1P 2m 3m 4d 5d 6m 7d", "ultralocrian", "superlocrian bb7", "superlocrian diminished"},
    {"1P 2m 3m 4P 5d 6M 7m", "locrian 6", "locrian natural 6", "locrian sharp 6"},
    {"1P 2A 3M 4P 5P 5A 7M", "augmented heptatonic"},
    {"1P 2M 3m 4A 5P 6M 7m", "dorian #4", "ukrainian dorian", "romanian minor", "altered dorian"},
    {"1P 2M 3m 4A 5P 6M 7M", "lydian diminished"},
    {"1P 2M 3M 4A 5A 7m 7M", "leading whole tone"},
    {"1P 2M 3M 4A 5P 6m 7m", "lydian minor"},
    {"1P 2m 3M 4P 5P 6m 7m", "phrygian dominant", "spanish", "phrygian major"},
    {"1P 2m 3m 4P 5P 6m 7M", "balinese"},
    {"1P 2m 3m 4P 5P 6M 7M", "neopolitan major"},
    {"1P 2M 3M 4P 5P 6m 7M", "harmonic major"},
    {"1P 2m 3M 4P 5P 6m 7M", "double harmonic major", "gypsy"},
    {"1P 2M 3m 4A 5P 6m 7M", "hungarian minor"},
    {"1P 2A 3M 4A 5P 6M 7m", "hungarian major"},
    {"1P 2m 3M 4P 5d 6M 7m", "oriental"},
    {"1P 2m 3m 3M 4A 5P 7m", "flamenco"},
    {"1P 2m 3m 4A 5P 6m 7M", "todi raga"},
    {"1P 2m 3M 4P 5d 6m 7M", "persian"},
    {"1P 2m 3M 5d 6m 7m 7M", "enigmatic"},
    {"1P 2M 3M 4P 5A 6M 7M", "major augmented", "major #5", "ionian augmented", "ionian #5"},
    {"1P 2A 3M 4A 5P 6M 7M", "lydian #9"},

    // 8-note scales
    {"1P 2m 2M 4P 4A 5P 6m 7M", "messiaen's mode #4"},
    {"1P 2m 3M 4P 4A 5P 6m 7M", "purvi raga"},
    {"1P 2m 3m 3M 4P 5P 6m 7m", "spanish heptatonic"},
    {"1P 2M 3m 3M 4P 5P 6M 7m", "bebop minor"},
    {"1P 2M 3M 4P 5P 5A 6M 7M", "bebop major"},
    {"1P 2m 3m 4P 5d 5P 6m 7m", "bebop locrian"},
    {"1P 2M 3m 4P 5P 6m 7m 7M", "minor bebop"},
    {"1P 2M 3M 4P 5d 5P 6M 7M", "ichikosucho"},
    {"1P 2M 3m 4P 5P 6m 6M 7M", "minor six diminished"},
    {"1P 2m 3m 3M 4A 5P 6M 7m", "half-whole diminished", "dominant diminished", "messiaen's mode #2"},
    {"1P 3m 3M 4P 5P 6M 7m 7M", "kafi raga"},
    {"1P 2M 3M 4P 4A 5A 6A 7M", "messiaen's mode #6"},

    // 9-note scales
    {"1P 2M 3m 3M 4P 5d 5P 6M 7m", "composite blues"},
    {"1P 2M 3m 3M 4A 5P 6m 7m 7M", "messiaen's mode #3"},

    // 10-note scales
    {"1P 2m 2M 3m 4P 4A 5P 6m 6M 7M", "messiaen's mode #7"},

    // 12-note scales
    {"1P 2m 2M 3m 3M 4P 5d 5P 6m 6M 7m 7M", "chromatic"},
};

// Forward declaration of ensureInitialized
bool ensureInitialized();

std::vector<std::string> names() {
    ensureInitialized();
    std::vector<std::string> result;
    result.reserve(dictionary.size());
    for (const auto& scale : dictionary) {
        result.push_back(scale.name);
    }
    return result;
}

ScaleType get(const ScaleTypeName& type) {
    ensureInitialized();
    auto it = index.find(type);
    if (it != index.end()) {
        return *(it->second);
    }
    return NoScaleType;
}

ScaleType get(int setNum) {
    ensureInitialized();
    auto it = numIndex.find(setNum);
    if (it != numIndex.end()) {
        return *(it->second);
    }
    return NoScaleType;
}

std::vector<ScaleType> all() {
    ensureInitialized();
    return dictionary;
}

std::vector<std::string> keys() {
    ensureInitialized();
    std::vector<std::string> result;
    result.reserve(index.size());
    for (const auto& entry : index) {
        result.push_back(entry.first);
    }
    return result;
}

void removeAll() {
    dictionary.clear();
    index.clear();
    numIndex.clear();
}

ScaleType add(const std::vector<std::string>& intervals, 
              const std::string& name, 
              const std::vector<std::string>& aliases) {
    
    // Create scale type from intervals
    pcset::Pcset pcsetBase = pcset::getPcset(intervals);
    
    // Pre-allocate space to avoid reallocation during additions
    if (dictionary.capacity() == dictionary.size()) {
        dictionary.reserve(dictionary.size() + 100);
    }
    
    // Create new scale
    ScaleType scale;
    
    // Copy properties from pcset
    scale.name = name;
    scale.empty = pcsetBase.empty;
    scale.setNum = pcsetBase.setNum;
    scale.chroma = pcsetBase.chroma;
    scale.normalized = pcsetBase.normalized;
    scale.intervals = intervals;
    scale.aliases = aliases;
    
    // Add to dictionary
    dictionary.push_back(scale);
    
    // Get a reference to the actual scale in the dictionary
    ScaleType* storedScale = &dictionary.back();
    
    // Add all indexes
    index[storedScale->name] = storedScale;
    numIndex[storedScale->setNum] = storedScale;
    index[storedScale->chroma] = storedScale;
    
    // Add all aliases
    for (const auto& alias : storedScale->aliases) {
        index[alias] = storedScale;
    }
    
    return *storedScale;
}

void addAlias(const ScaleType& scale, const std::string& alias) {
    // Find the scale in the dictionary by name (safer than comparing by reference)
    for (auto& entry : dictionary) {
        if (entry.name == scale.name) {
            // Add the alias to the entry's aliases
            entry.aliases.push_back(alias);
            // Add to index - use pointer to the element in the dictionary
            index[alias] = &entry;
            break;
        }
    }
}

// Split a space-separated string into a vector of strings
std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> result;
    std::string::size_type start = 0;
    std::string::size_type end = str.find(' ');
    
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(' ', start);
    }
    
    if (start < str.length()) {
        result.push_back(str.substr(start));
    }
    
    return result;
}

void initialize() {
    // Clear any existing data
    removeAll();
    
    // Pre-allocate space to avoid reallocations
    dictionary.reserve(SCALES.size());
    
    // Add all scales from data
    for (const auto& scale : SCALES) {
        if (scale.size() >= 2) {
            std::vector<std::string> intervals = splitString(scale[0]);
            std::string name = scale[1];
            std::vector<std::string> aliases;
            
            // Add all aliases (if any)
            for (size_t i = 2; i < scale.size(); ++i) {
                aliases.push_back(scale[i]);
            }
            
            add(intervals, name, aliases);
        }
    }
}

// Call initialize explicitly in the first function access to ensure it's properly initialized
// This is another safeguard in case static initialization didn't work as expected
bool ensureInitialized() {
    static bool initialized = false;
    if (!initialized) {
        initialize();
        initialized = true;
    }
    return initialized;
}

namespace {
    // Force initialization
    const bool initialized = ensureInitialized();
}

} // namespace scale_type
} // namespace tonalcpp