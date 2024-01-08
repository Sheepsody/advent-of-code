#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

using namespace std;

constexpr auto max_size = numeric_limits<streamsize>::max();

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "Missing argument" << endl;
    throw;
  }

  ifstream inputFile(argv[1]);

  // Check if the file is open
  if (!inputFile.is_open()) {
    cerr << "Error opening the file!" << endl;
    return 1; // Return an error code
  }

  std::string line;
  int sum_a;
  int sum_b;

  while (std::getline(inputFile, line)) {
    if (line.size() < 5)
      break;

    std::istringstream iss(line);
    iss.ignore(max_size, ' ');

    // Extract game ID
    int gameId;
    iss >> gameId;

    // Extract subsets
    std::vector<std::string> subsets;
    std::string subset;

    iss.ignore(1, ' ');

    bool possible = true;
    int max_red = 0, max_green = 0, max_blue = 0;
    while (std::getline(iss, subset, ';')) {
      std::istringstream iss(subset);

      int current_red = 0, current_green = 0, current_blue = 0;

      int count;
      std::string color;

      while (iss >> count >> color) {
        if (color.starts_with("red"))
          current_red += count;
        if (color.starts_with("blue"))
          current_blue += count;
        if (color.starts_with("green"))
          current_green += count;
      }

      max_red = max(max_red, current_red);
      max_blue = max(max_blue, current_blue);
      max_green = max(max_green, current_green);

      if (current_blue > 14 || current_green > 13 || current_red > 12) {
        possible = false;
      }
    }

    if (possible)
      sum_a += gameId;
    sum_b += max_red * max_green * max_blue;
  }

  cout << "Part a: " << sum_a << endl;
  cout << "Part b: " << sum_b << endl;
  inputFile.close();

  return 0;
}
