cmake_minimum_required(VERSION 3.14)
project(TonalCpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(TONAL_BUILD_TESTS "Build test programs" ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Add compile flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
  add_compile_options(/W4)
endif()

# Library includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define source files for the library
set(LIB_SOURCES
    src/pitch.cpp
    src/collection.cpp
    src/pitch_note.cpp
    src/pitch_interval.cpp
    src/pcset.cpp
    src/chord_type.cpp
    src/chord_detect.cpp
)

# Create static library
add_library(tonalcpp STATIC ${LIB_SOURCES})

# Create directories for library structure
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/tonalcpp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples)

# Setup tests if enabled
if(TONAL_BUILD_TESTS)
  enable_testing()
  # Use the doctest framework
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/doctest)
  
  # Test executable
  set(TEST_SOURCES
    test/test_main.cpp
    test/test_helpers.cpp
  )
  
  add_executable(test_tonalcpp ${TEST_SOURCES})
  target_link_libraries(test_tonalcpp PRIVATE tonalcpp)
  
  # Register test
  add_test(NAME tonalcpp_tests COMMAND test_tonalcpp)
endif()

# Install targets
install(TARGETS tonalcpp
        EXPORT tonalcpp-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

install(DIRECTORY include/ DESTINATION include)

# Example applications
add_executable(chord_detector examples/chord_detector.cpp)
target_link_libraries(chord_detector PRIVATE tonalcpp)

add_executable(pitch_example examples/pitch_example.cpp)
target_link_libraries(pitch_example PRIVATE tonalcpp)