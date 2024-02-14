#include <iostream>
#include <string>
#include "days/day01.cpp"

std::string readFileToString(const std::string& filename) {
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

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " day_number" << std::endl;
        return 1;
    }

    std::string day(argv[1]);

    auto content = readFileToString("data/day-01.txt");

    if (day == "1") {
        std::cout << "Day 01 - Part 1: " << Day01::part_one(content) << std::endl;
        std::cout << "Day 01 - Part 2: " << Day01::part_two(content) << std::endl;
    }
    else {
        std::cerr << "Invalid day number" << std::endl;
        return 1;
    }

    return 0;
}
