# TonalCpp - C++ Port of tonal.js

This is a C++ port of the [tonal.js](https://github.com/tonaljs/tonal) music theory library, with a focus on the chord detection functionality.

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

## Implementation Details

This is a port of the following packages from tonal.js:

1. Core packages:
   - pitch - Core pitch representation
   - collection - Utility functions for collections

2. Mid-level packages:
   - pitch-note - Note representation and operations 
   - pitch-interval - Interval representation and operations
   - pcset - Pitch class set operations

3. High-level packages:
   - chord-type - Chord type definitions
   - chord-detect - Chord detection algorithm

## License

Same as tonal.js (MIT)