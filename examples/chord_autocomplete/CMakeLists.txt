cmake_minimum_required(VERSION 3.14)
project(chord_autocomplete_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the source files
set(COMMON_SOURCES
    chord_autocomplete.cpp
)

# Create the interactive executable
add_executable(chord_autocomplete 
    main.cpp
    ${COMMON_SOURCES}
)

# Create the demo executable
add_executable(chord_autocomplete_demo
    demo.cpp
    ${COMMON_SOURCES}
)

# Find and link the tonalcpp library
# Assuming the library is built at the top level
find_library(TONALCPP_LIB tonalcpp PATHS ../../build REQUIRED)

# Include directories
target_include_directories(chord_autocomplete PRIVATE
    ../../include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(chord_autocomplete_demo PRIVATE
    ../../include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against the tonalcpp library
target_link_libraries(chord_autocomplete ${TONALCPP_LIB})
target_link_libraries(chord_autocomplete_demo ${TONALCPP_LIB})