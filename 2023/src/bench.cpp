#include "utils.cpp"
#include <benchmark/benchmark.h>

#include "days/day01.cpp"
#include "days/day02.cpp"
#include "days/day03.cpp"
#include "days/day04.cpp"
#include "days/day05.cpp"

#define BENCHMARK_FUNCTION(name, day, part)                                    \
  static void name(benchmark::State &state) {                                  \
    auto content = readFileToString("data/day-" #day ".txt");                  \
    auto parsed = Day##day::parse(content);                                    \
    for (auto _ : state) {                                                     \
      Day##day::part(parsed);                                                  \
    }                                                                          \
  }                                                                            \
  BENCHMARK(name);

BENCHMARK_FUNCTION(DAY_01_PART_1, 01, part_one)
BENCHMARK_FUNCTION(DAY_01_PART_2, 01, part_two)

BENCHMARK_FUNCTION(DAY_02_PART_1, 02, part_one)
BENCHMARK_FUNCTION(DAY_02_PART_2, 02, part_two)

BENCHMARK_FUNCTION(DAY_03_PART_1, 03, part_one)
BENCHMARK_FUNCTION(DAY_03_PART_2, 03, part_two)

BENCHMARK_FUNCTION(DAY_04_PART_1, 04, part_one)
BENCHMARK_FUNCTION(DAY_04_PART_2, 04, part_two)

BENCHMARK_FUNCTION(DAY_05_PART_1, 05, part_one)
BENCHMARK_FUNCTION(DAY_05_PART_2, 05, part_two)

BENCHMARK_MAIN();
