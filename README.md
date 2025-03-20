# TonalCpp - C++ Port of tonal.js

This is a C++ port of the [tonal.js](https://github.com/tonaljs/tonal) music theory library, with a focus on the chord detection functionality.

## C++ Implementation Status

### Packages Ported to C++ (with tests)
- **pitch**: Core pitch representation and manipulation
- **pitch_note**: Note operations and conversions
- **pitch_interval**: Interval operations and conversions
- **pitch_distance**: Functions to calculate distances between pitches
- **pcset**: Pitch class sets functionality
- **collection**: Utility functions for collections
- **chord_type**: Dictionary of chord types and related operations
- **chord_detect**: Chord detection from notes

### Packages Not Yet Ported to C++
- **chord**: Chord operations
- **duration-value**: Note duration values
- **interval**: Interval operations
- **key**: Key functionality
- **midi**: MIDI number conversions
- **mode**: Mode dictionary
- **note**: Note operations
- **progression**: Chord progressions
- **range**: Note range operations
- **rhythm-pattern**: Rhythm pattern operations
- **roman-numeral**: Roman numeral parsing
- **scale**: Scale operations
- **scale-type**: Scale type dictionary
- **time-signature**: Time signature parsing
- **voice-leading**: Voice leading operations
- **voicing**: Chord voicing operations
- **voicing-dictionary**: Dictionary of chord voicings

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

## Building

### Quick Start (testing)

```bash
rm -rf build
cmake -S . -B build
cmake --build build
./build/test_tonalcpp
```