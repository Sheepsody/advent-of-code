#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>

using namespace std;

constexpr auto decimal = [](auto &&c) { return c - '0'; };
constexpr auto is_digit = [](auto &&c) { return isdigit(c); };

int part_one(string line) {
  auto first = find_if(line.begin(), line.end(), is_digit);
  auto last = find_if(line.rbegin(), line.rend(), is_digit);

  // Check characters not found
  if (first == line.end() || last.base() == line.begin())
    return 0;

  return decimal(*first) * 10 + decimal(*last);
}

vector<std::string> digits{"one", "two", "three", "four", "five",
                           "six", "seven", "eight", "nine"};

int get_first_char(string line) {
  for (auto pos = line.begin(); pos != line.end(); pos++) {
    auto view = string_view(pos, line.end());

    if (isdigit(view[0]))
      return decimal(view[0]);

    for (auto i = 0; i < digits.size(); i++) {
      if (view.starts_with(digits[i]))
        return i + 1;
    }
  }

  return -1;
}

int get_last_char(string line) {
  for (auto pos = line.rbegin(); pos != line.rend(); pos++) {
    auto view = string_view(&(*pos));

    if (isdigit(view[0]))
      return decimal(view[0]);

    for (auto i = 0; i < digits.size(); i++) {
      if (view.starts_with(digits[i]))
        return i + 1;
    }
  }

  return -1;
}

int part_two(string line) {
  auto first = get_first_char(line);
  auto last = get_last_char(line);
  if (first == -1 || last == -1)
    return 0;
  return first * 10 + last;
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

  pair<int, int> sum = make_pair(0, 0);
  for (string line; getline(inputFile, line);) {
    sum.first += part_one(line);
    sum.second += part_two(line);
  }
  cout << "Part 1: " << sum.first << endl;
  cout << "Part 2: " << sum.second << endl;

  inputFile.close();

  return 0;
}
