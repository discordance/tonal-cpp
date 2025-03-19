# Tonal.js Pitch Package Overview

The `pitch` package is the foundational module in the Tonal.js library, providing a notation-agnostic representation of musical pitches. This document describes how it works and how it should be implemented in C++.

## Conceptual Overview

The pitch package uses a simple but powerful representation for pitches that can be used to model:

1. **Pitch Classes** (like C, D#, Bb) - notes without octave information
2. **Notes** (like C4, F#3, Gb5) - notes with octave information 
3. **Intervals** (like P5, m3, M7) - musical distances between notes

All of these musical concepts are modeled using a common `Pitch` structure that contains the following key properties:

## Core Data Structure: `Pitch`

```typescript
interface Pitch {
  readonly step: number;   // The scale step (0 = C, 1 = D, ... 6 = B)
  readonly alt: number;    // The alteration (-1 = flat, 0 = natural, 1 = sharp, etc.)
  readonly oct?: number;   // The octave (undefined for pitch classes)
  readonly dir?: Direction; // Interval direction (undefined for notes)
}
```

This compact representation allows for efficient manipulation of musical pitches while preserving their essential properties.

## Coordinate System

One of the most powerful aspects of the pitch system is its use of a coordinate system based on the circle of fifths. This allows for:

1. Consistent calculations across different musical entities
2. Simple transposition and interval operations
3. Enharmonic equivalence where needed
4. Preservation of note spelling when required

Coordinates are represented as arrays of numbers:
- `PitchClassCoordinates = [fifths]` - for pitch classes
- `NoteCoordinates = [fifths, octaves]` - for notes
- `IntervalCoordinates = [fifths, octaves, direction]` - for intervals

### Conversion Between Pitches and Coordinates

The `coordinates()` function converts a pitch to its coordinate representation, while the `pitch()` function converts coordinates back into a pitch.

### Key Constants

The pitch system uses several key constant arrays:

1. **SIZES**: `[0, 2, 4, 5, 7, 9, 11]`
   - Represents the number of semitones from C for each natural note (C, D, E, F, G, A, B)
   - Used for calculating chroma and heights

2. **FIFTHS**: `[0, 2, 4, -1, 1, 3, 5]`
   - Represents the position in the circle of fifths for each natural note
   - C is at position 0, G is at position 1, D is at position 2, etc.

3. **FIFTHS_TO_STEPS**: `[3, 0, 4, 1, 5, 2, 6]`
   - Used to convert from a position in the circle of fifths back to a step value
   - For example, fifths position 1 (G) maps to step 4

## Core Functions

### Type Guards

1. `isPitch(obj)`: Determines if an object conforms to the Pitch interface
2. `isNamedPitch(obj)`: Determines if an object has a 'name' property

### Pitch Properties

1. `chroma({ step, alt })`: Calculates the pitch class/chroma (0-11) of a pitch
2. `height({ step, alt, oct, dir })`: Calculates the absolute height (MIDI note number + octave shifts)
3. `midi(pitch)`: Returns the MIDI note number (if in the valid MIDI range) or null

### Coordinate Conversions

1. `coordinates(pitch)`: Converts a pitch to its coordinate representation
2. `pitch(coordinates)`: Converts coordinates back to a pitch

## Implementation Notes

1. The design uses optional properties (`oct?`, `dir?`) to create a flexible yet type-safe representation
2. The module uses pure functions without side effects
3. The core calculations involve modular arithmetic and various offset calculations

## Applications

This pitch representation is the foundation for:

1. Note parsing and manipulation
2. Interval calculations
3. Chord and scale representation
4. Transposition and inversion operations
5. Musical set theory operations

## Example Usage

```typescript
// Create pitches
const C = { step: 0, alt: 0 };             // C pitch class
const Fs = { step: 3, alt: 1 };            // F# pitch class
const Bb3 = { step: 6, alt: -1, oct: 3 };  // Bb3 note
const M3 = { step: 2, alt: 0, oct: 0, dir: 1 }; // Major third interval

// Calculate chroma (pitch class number 0-11)
chroma(C);    // => 0
chroma(Fs);   // => 6
chroma(Bb3);  // => 10

// Calculate height (absolute position)
height(C);     // => -1200 (pitch class reference point)
height(Bb3);   // => 46 (semitones from C-1)

// Get MIDI note number
midi(Bb3);    // => 58

// Convert to coordinates
coordinates(C);     // => [0]
coordinates(Fs);    // => [-1]
coordinates(Bb3);   // => [-5, 5]
coordinates(M3);    // => [4, -2]

// Convert from coordinates back to pitch
pitch([0]);        // => { step: 0, alt: 0 }
pitch([-5, 5]);    // => { step: 6, alt: -1, oct: 3 }
```

## C++ Implementation Considerations

When implementing this package in C++:

1. Use strong types for Direction, PitchCoordinates, etc.
2. Consider using std::optional for optional properties
3. Use references or const references for function parameters to avoid copying
4. For maximum compatibility, maintain the same function and property names
5. Use proper unit tests to validate all edge cases

This foundation is essential for implementing the rest of the library correctly.