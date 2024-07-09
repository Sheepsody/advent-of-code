#include <fstream>
#include <iostream>
#include <string>

#include "days/day01.cpp"
#include "days/day02.cpp"
#include "days/day03.cpp"
#include "days/day04.cpp"
#include "days/day05.cpp"
#include "days/day06.cpp"
#include "days/day07.cpp"
#include "days/day08.cpp"
#include "days/day09.cpp"
#include "days/day10.cpp"
#include "days/day11.cpp"
#include "days/day12.cpp"
#include "days/day13.cpp"
#include "days/day14.cpp"
#include "days/day15.cpp"
#include "days/day16.cpp"
#include "days/day17.cpp"
#include "days/day18.cpp"

#define HANDLE_DAY(dayNumber, DayClass)                                        \
  if (day == dayNumber) {                                                      \
    auto parsed = DayClass::parse(content);                                    \
    std::cout << "Day : " << dayNumber                                         \
              << " - Part 1: " << DayClass::part_one(parsed) << std::endl;     \
    std::cout << "Day : " << dayNumber                                         \
              << " - Part 2: " << DayClass::part_two(parsed) << std::endl;     \
    return 0;                                                                  \
  }

std::string readFileToString(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    throw;
  }

  // Read the entire file into a string
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " day_number input_file" << std::endl;
    return 1;
  }

  int day = std::stoi(argv[1]);

  // Needs to be formatted ith leading 0
  std::stringstream ssfilename;
  ssfilename << "data/" << argv[2];
  std::string filename = ssfilename.str();
  auto content = readFileToString(filename);

  HANDLE_DAY(1, Day01)
  HANDLE_DAY(2, Day02)
  HANDLE_DAY(3, Day03)
  HANDLE_DAY(4, Day04)
  HANDLE_DAY(5, Day05)
  HANDLE_DAY(6, Day06)
  HANDLE_DAY(7, Day07)
  HANDLE_DAY(8, Day08)
  HANDLE_DAY(9, Day09)
  HANDLE_DAY(10, Day10)
  HANDLE_DAY(11, Day11)
  HANDLE_DAY(12, Day12)
  HANDLE_DAY(13, Day13)
  HANDLE_DAY(14, Day14)
  HANDLE_DAY(15, Day15)
  HANDLE_DAY(16, Day16)
  HANDLE_DAY(17, Day17)
  HANDLE_DAY(18, Day18)

  cout << "Could not find day: " << argv[1] << endl;
  return 0;
}
