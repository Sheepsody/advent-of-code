#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>

using namespace std;

int part_one(string line) {
  // Char to int
  constexpr auto decimal = [](auto &&c) { return c - '0'; };

  auto first = find_if(line.begin(), line.end(),
                       [](unsigned char c) { return isdigit(c); });

  auto last = find_if(line.rbegin(), line.rend(),
                      [](unsigned char c) { return isdigit(c); });

  // Check characters not found
  if (first == line.end() || last.base() == line.begin())
    return 0;

  return decimal(*first) * 10 + decimal(*last);
}

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

  int sum = 0;
  for (string line; getline(inputFile, line);) {
    sum += part_one(line);
  }
  cout << sum << endl;

  inputFile.close();

  return 0;
}
