#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest/doctest.h"

// This file serves as the main entry point for all tests
// Individual test cases are in separate files and will
// be automatically included in the test suite.

// We keep one basic test here to verify the test framework
TEST_CASE("Test framework") {
    CHECK(1 == 1);
}

// Individual test files
#include "test_pitch.cpp"
#include "test_pitch_note.cpp"
#include "test_pcset.cpp"
#include "test_collection.cpp"