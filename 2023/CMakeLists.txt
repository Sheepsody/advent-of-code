cmake_minimum_required(VERSION 3.27)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm@16/bin/clang++")

# Set compiler flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Find packages
include(FetchContent)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3) # need main for benchmark::
FetchContent_MakeAvailable(benchmark)
set(BENCHMARK_ENABLE_LTO ON CACHE BOOL "Enable Link Time Optimization")

# Specify the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Project def
project(AdventOfCode)

SET(SOURCES
    src/main.cpp
)
add_executable(aoc ${SOURCES})

add_executable(bench src/bench.cpp)
target_link_libraries(bench benchmark::benchmark)

set_target_properties(bench PROPERTIES
    IMPORTED_LOCATION_RELEASE "${benchmark_BINARY_DIR}/src/libbenchmark.a"
    IMPORTED_LOCATION_DEBUG "${benchmark_BINARY_DIR}/src/libbenchmark.a"
)
