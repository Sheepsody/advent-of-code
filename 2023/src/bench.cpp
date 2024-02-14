#include "days/day01.cpp"
#include "utils.cpp"
#include <benchmark/benchmark.h>

static void DAY_01_PART_1(benchmark::State &state) {
  auto content = readFileToString("data/day-01.txt");
  for (auto _ : state) {
    Day01::part_one(content);
  }
}
BENCHMARK(DAY_01_PART_1);

static void DAY_01_PART_2(benchmark::State &state) {
  auto content = readFileToString("data/day-01.txt");
  for (auto _ : state) {
    Day01::part_two(content);
  }
}
BENCHMARK(DAY_01_PART_2);

BENCHMARK_MAIN();
