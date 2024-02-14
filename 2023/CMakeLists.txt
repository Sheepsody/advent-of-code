cmake_minimum_required(VERSION 3.27)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find packages
include(FetchContent)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3) # need main for benchmark::
FetchContent_MakeAvailable(benchmark)

# Specify the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Project def
project(AdventOfCode)

# Define source files for each day
set(DAYS_SOURCE_FILES
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

SET(SOURCES
    src/main.cpp
)
add_executable(aoc ${SOURCES})

add_executable(bench src/bench.cpp)
target_link_libraries(bench benchmark::benchmark)
