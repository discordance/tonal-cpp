cmake_minimum_required(VERSION 3.14)
project(TonalCpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(TONAL_BUILD_TESTS "Build test programs" ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Add compile flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
  add_compile_options(/W4)
endif()

# Define source files for the library
set(LIB_SOURCES
    src/pitch.cpp
    src/collection.cpp
    src/pitch_note.cpp
    src/pitch_interval.cpp
    src/pitch_distance.cpp
    src/pcset.cpp
    src/chord_type.cpp
    src/chord_detect.cpp
    src/chord.cpp
    src/interval.cpp
    src/midi.cpp
    src/note.cpp
    src/helpers.cpp
    src/scale_type.cpp
    src/scale.cpp
    src/range.cpp
    src/voice_leading.cpp
    src/voicing_dictionary.cpp
    src/voicing.cpp
)

# Create static library
add_library(tonalcpp STATIC ${LIB_SOURCES})
add_library(tonalcpp::tonalcpp ALIAS tonalcpp)

# Set include directories properly for both local and parent project use
target_include_directories(tonalcpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Make directories if needed
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/tonalcpp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)

# Setup tests if enabled
if(TONAL_BUILD_TESTS)
  enable_testing()
  
  # Test executable
  set(TEST_SOURCES
    test/test_main.cpp
    test/test_helpers.cpp
    test/test_pitch.cpp
    test/test_pitch_note.cpp
    test/test_pitch_interval.cpp
    test/test_pcset.cpp
    test/test_collection.cpp
    test/test_chord_type.cpp
    test/test_chord_detect.cpp
    test/test_chord.cpp
    test/test_interval.cpp
    test/test_midi.cpp
    test/test_note.cpp
    test/test_scale_type.cpp
    test/test_scale.cpp
    test/test_range.cpp
    test/test_voice_leading.cpp
    test/test_voicing_dictionary.cpp
    test/test_voicing.cpp
  )
  
  add_executable(test_tonalcpp ${TEST_SOURCES})
  target_link_libraries(test_tonalcpp PRIVATE tonalcpp)
  target_include_directories(test_tonalcpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/doctest)
  
  # Register test
  add_test(NAME tonalcpp_tests COMMAND test_tonalcpp)
endif()

# Export targets to make them available to parent projects
if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  # We're being included via add_subdirectory, export targets automatically
  set_property(GLOBAL APPEND PROPERTY tonalcpp_TARGETS tonalcpp)
endif()

