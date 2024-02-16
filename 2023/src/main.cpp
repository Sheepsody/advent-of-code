#include <fstream>
#include <iostream>
#include <string>

#include "days/day01.cpp"
#include "days/day02.cpp"
#include "days/day03.cpp"
#include "days/day04.cpp"

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
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " day_number" << std::endl;
    return 1;
  }

  int day = std::stoi(argv[1]);

  // Needs to be formatted ith leading 0
  std::stringstream ssfilename;
  ssfilename << "data/day-" << std::setw(2) << std::setfill('0') << day
             << ".txt";
  std::string filename = ssfilename.str();
  auto content = readFileToString(filename);

  if (day == 1) {
    std::cout << "Day 01 - Part 1: " << Day01::part_one(content) << std::endl;
    std::cout << "Day 01 - Part 2: " << Day01::part_two(content) << std::endl;
  } else if (day == 2) {
    std::cout << "Day 02 - Part 1: " << Day02::part_one(content) << std::endl;
    std::cout << "Day 02 - Part 2: " << Day02::part_two(content) << std::endl;
  } else if (day == 3) {
    auto parsed = Day03::parse(content);
    std::cout << "Day 03 - Part 1: " << Day03::part_one(parsed) << std::endl;
    std::cout << "Day 03 - Part 2: " << Day03::part_two(parsed) << std::endl;
  } else if (day == 4) {
    auto parsed = Day04::parse(content);
    std::cout << "Day 04 - Part 1: " << Day04::part_one(parsed) << std::endl;
    std::cout << "Day 04 - Part 2: " << Day04::part_two(parsed) << std::endl;
  } else {
    std::cerr << "Day " << day << " not implemented" << std::endl;
    return 1;
  }

  return 0;
}
