#include "tonalcpp/pitch.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace tonalcpp;

// Helper function to print pitch information
void printPitchInfo(const std::string& name, const Pitch& pitch) {
    std::cout << "Pitch: " << name << std::endl;
    std::cout << "  step: " << pitch.step << ", alt: " << pitch.alt;
    
    if (pitch.oct.has_value()) {
        std::cout << ", oct: " << *pitch.oct;
    }
    
    if (pitch.dir.has_value()) {
        std::cout << ", dir: " << ((*pitch.dir == Direction::Ascending) ? "ascending" : "descending");
    }
    
    std::cout << std::endl;
    std::cout << "  chroma: " << chroma(pitch) << std::endl;
    std::cout << "  height: " << height(pitch) << std::endl;
    
    if (auto m = midi(pitch)) {
        std::cout << "  midi: " << *m << std::endl;
    } else {
        std::cout << "  midi: N/A" << std::endl;
    }
    
    auto coords = coordinates(pitch);
    std::cout << "  coordinates: [";
    for (size_t i = 0; i < coords.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << coords[i];
    }
    std::cout << "]" << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "===== TonalCpp Pitch Module Example =====" << std::endl << std::endl;
    
    // Create some pitch objects
    
    // Pitch classes (notes without octave)
    Pitch C = {0, 0, std::nullopt, std::nullopt};     // C
    Pitch Csharp = {0, 1, std::nullopt, std::nullopt}; // C#
    Pitch Bb = {6, -1, std::nullopt, std::nullopt};   // Bb
    
    // Notes with octave
    Pitch C4 = {0, 0, 4, std::nullopt};               // C4
    Pitch Fsharp3 = {3, 1, 3, std::nullopt};          // F#3
    Pitch Eb5 = {2, -1, 5, std::nullopt};             // Eb5
    
    // Intervals
    Pitch M3 = {2, 0, 0, Direction::Ascending};       // Major 3rd
    Pitch P5 = {4, 0, 0, Direction::Ascending};       // Perfect 5th
    Pitch m7desc = {6, -1, 0, Direction::Descending}; // Descending minor 7th
    
    // Print information about each pitch
    printPitchInfo("C", C);
    printPitchInfo("C#", Csharp);
    printPitchInfo("Bb", Bb);
    
    printPitchInfo("C4", C4);
    printPitchInfo("F#3", Fsharp3);
    printPitchInfo("Eb5", Eb5);
    
    printPitchInfo("M3", M3);
    printPitchInfo("P5", P5);
    printPitchInfo("m7 (descending)", m7desc);
    
    // Demonstrate conversion between coordinates and pitches
    std::cout << "===== Coordinate to Pitch Conversion =====" << std::endl << std::endl;
    
    std::vector<PitchCoordinates> coordExamples = {
        {0},           // C (pitch class)
        {7},           // C# (pitch class)
        {0, 4},        // C4 (note)
        {-2, 3},       // Bb3 (note)
        {4, -2, 1},    // M3 (interval)
        {1, 0, -1}     // P5 (descending interval)
    };
    
    for (const auto& coords : coordExamples) {
        std::cout << "Coordinates: [";
        for (size_t i = 0; i < coords.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << coords[i];
        }
        std::cout << "]" << std::endl;
        
        Pitch p = pitchFromCoordinates(coords);
        std::cout << "  step: " << p.step << ", alt: " << p.alt;
        
        if (p.oct.has_value()) {
            std::cout << ", oct: " << *p.oct;
        }
        
        if (p.dir.has_value()) {
            std::cout << ", dir: " << ((*p.dir == Direction::Ascending) ? "ascending" : "descending");
        }
        
        std::cout << std::endl;
        std::cout << "  chroma: " << chroma(p) << std::endl;
        std::cout << "  height: " << height(p) << std::endl << std::endl;
    }
    
    return 0;
}