#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>

using namespace std;

constexpr auto decimal = [](auto &&c) { return c - '0'; };
constexpr auto is_symbol = [](auto &&c) { return !(isdigit(c) or c == '.'); };

const int directions[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                             {0, 1},   {1, -1}, {1, 0},  {1, 1}};

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
  int sum_a, sum_b;

  vector<vector<char>> lines;

  while (std::getline(inputFile, line)) {
    if (!line.empty())
      lines.emplace_back(std::vector<char>(line.begin(), line.end()));
  }

  int width = lines[0].size(), height = lines.size();

  // Store all part numbers adjacent to a gear
  unordered_map<int, vector<int>> gears;

  for (int i = 0; i < lines.size(); i++) {
    bool is_adjacent = false;
    int sum = 0;
    set<int> gear_positions;

    for (int j = 0; j <= lines[0].size(); j++) {
      char c = (j != lines[i].size()) ? lines[i][j] : ' ';

      if (isdigit(c)) {
        sum = sum * 10 + decimal(c);

        for (auto const &d : directions) {
          int ni = i + d[0];
          int nj = j + d[1];
          if (ni >= 0 && nj >= 0 && ni < height && nj < width &&
              is_symbol(lines[ni][nj])) {

            is_adjacent = true;

            if (lines[ni][nj] == '*')
              gear_positions.insert(ni * width + nj);
          }
        }
      } else {
        if (is_adjacent) {
          sum_a += sum;

          for (const auto &pos : gear_positions)
            gears[pos].emplace_back(sum);
        }

        gear_positions.clear();
        sum = 0;
        is_adjacent = false;
      }
    }
  }

  for (const auto &g : gears)
    sum_b += (g.second.size() == 2) ? g.second[0] * g.second[1] : 0;

  cout << "Part a: " << sum_a << endl;
  cout << "Part b: " << sum_b << endl;
  inputFile.close();

  return 0;
}
