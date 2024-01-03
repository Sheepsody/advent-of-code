cmake_minimum_required(VERSION 3.12)
project(AdventOfCode)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Specify the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Define source files for each day
set(DAYS_SOURCE_FILES
    day01.cpp
    day02.cpp
    day03.cpp
    day04.cpp
    day05.cpp
    day06.cpp
    day07.cpp
    day08.cpp
    day09.cpp
    day10.cpp
    day11.cpp
    day12.cpp
    day13.cpp
    day14.cpp
    day15.cpp
    day16.cpp
    day17.cpp
    day18.cpp
    day19.cpp
    day20.cpp
    day21.cpp
    day22.cpp
    day23.cpp
    day24.cpp
    day25.cpp
)

# Add executable for each day
foreach(DAY_SOURCE ${DAYS_SOURCE_FILES})
    get_filename_component(DAY_NAME ${DAY_SOURCE} NAME_WE)
    add_executable(${DAY_NAME} src/${DAY_SOURCE})
endforeach()
