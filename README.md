# TonalCpp

This is a C++ port of the [tonal.js](https://github.com/tonaljs/tonal) music theory library, with a focus on the chord detection functionality.

## C++ Implementation Status

### Packages Ported to C++ (with tests)
- **pitch**: Core pitch representation and manipulation
- **pitch_note**: Note operations and conversions
- **pitch_interval**: Interval operations and conversions
- **pitch_distance**: Functions to calculate distances between pitches
- **note**: Note operations
- **pcset**: Pitch class sets functionality
- **collection**: Utility functions for collections
- **chord_type**: Dictionary of chord types and related operations
- **chord_detect**: Chord detection from notes
- **chord**: Chord operations
- **interval**: Interval operations
- **midi**: MIDI number conversions
- **scale_type**: Dictionary of scale types and related operations
- **scale**: Scale operations
- **helpers**: Utility functions used across the library

### Packages Not Yet Ported to C++
- **duration-value**: Note duration values
- **key**: Key functionality
- **mode**: Mode dictionary
- **progression**: Chord progressions
- **rhythm-pattern**: Rhythm pattern operations
- **time-signature**: Time signature parsing
- **voice-leading**: Voice leading operations


### Packages that won't be ported to C++ (yet)
- **abc-notation**: ABC notation parsing
- **array**: Array manipulation utilities
- **voicing**: Chord voicing operations
- **voicing-dictionary**: Dictionary of chord voicings
- **roman-numeral**: Roman numeral parsing
- **notation-scientific**: Scientific notation parsing
- **range**: Note range operations


## Project Structure

```
Cpp/
├── CMakeLists.txt         # Main build configuration
├── doctest/               # Doctest testing framework 
├── include/               # Header files 
│   └── tonalcpp/          # Library headers
├── src/                   # Source files
└── test/                  # Test files
```

## Dependencies

- C++17 compatible compiler
- CMake 3.14 or higher

## Using in Your Project

### Using with add_subdirectory

To use TonalCpp in your project, you can include it as a subdirectory:

1. Add the TonalCpp library to your project (as a git submodule or copy)

2. In your CMakeLists.txt:

```cmake
# Add the TonalCpp library
add_subdirectory(path/to/tonal/cpp)

# Link against the library
add_executable(your_app main.cpp)
target_link_libraries(your_app PRIVATE tonalcpp::tonalcpp)
```

### Example Usage

```cpp
#include <iostream>
#include <tonalcpp/note.h>
#include <tonalcpp/chord.h>

int main() {
    // Get note properties
    auto note = tonalcpp::note::get("C4");
    std::cout << "Note name: " << note.name << std::endl;
    std::cout << "MIDI: " << note.midi << std::endl;
    
    // Get notes in a chord
    auto chord = tonalcpp::chord::get("Cmaj7");
    std::cout << "Chord notes: ";
    for (const auto& n : chord.notes) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
```

## Building

### Quick Start (testing)

```bash
rm -rf build
cmake -S . -B build
cmake --build build
./build/test_tonalcpp
```

### CMake Options

- `TONAL_BUILD_TESTS` - Build the test suite (ON by default)