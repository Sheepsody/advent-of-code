#ifndef UTILS
#define UTILS

#include <iostream>
#include <istream>
#include <sstream>

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

#endif
