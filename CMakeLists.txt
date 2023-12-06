cmake_minimum_required(VERSION 3.12)
project(AdventOfCode)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Specify the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add executable for each day
add_executable(day01 src/day01.cpp)
add_executable(day02 src/day02.cpp)
add_executable(day03 src/day03.cpp)
add_executable(day04 src/day04.cpp)
