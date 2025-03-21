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
- **helpers**: Utility functions used across the library

### Packages Not Yet Ported to C++
- **abc-notation**: ABC notation parsing
- **array**: Array manipulation utilities
- **duration-value**: Note duration values
- **key**: Key functionality
- **mode**: Mode dictionary
- **notation-scientific**: Scientific notation parsing
- **progression**: Chord progressions
- **range**: Note range operations
- **rhythm-pattern**: Rhythm pattern operations
- **roman-numeral**: Roman numeral parsing

- **scale-dictionary**: Dictionary of scales
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