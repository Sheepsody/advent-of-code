#include "days/day01.cpp"
#include "utils.cpp"
#include <benchmark/benchmark.h>

#define BENCHMARK_FUNCTION(name, day, part)                                    \
  static void name(benchmark::State &state) {                                  \
    auto content = readFileToString("data/day-" #day ".txt");                  \
    for (auto _ : state) {                                                     \
      Day##day::part(content);                                                 \
    }                                                                          \
  }                                                                            \
  BENCHMARK(name);

BENCHMARK_FUNCTION(DAY_01_PART_1, 01, part_one)
BENCHMARK_FUNCTION(DAY_01_PART_2, 01, part_two)

BENCHMARK_MAIN();
