#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

struct Point {
  int x;
  int y;

  bool operator<(const Point &p) const {
    return x < p.x || (x == p.x && y < p.y);
  }

  bool operator==(const Point &p) const { return x == p.x && y == p.y; }
};

const int DIRECTIONS[4][2] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

template <> struct hash<Point> {
  std::size_t operator()(const Point &p) const {

    std::size_t h1 = std::hash<int>{}(p.x);
    std::size_t h2 = std::hash<int>{}(p.y);

    return h1 ^ (h2 << 1);
  }
};

using Grid = vector<vector<bool>>;

// Get reachable garden plots one step
unordered_set<Point> getReach(const Grid &grid,
                              const unordered_set<Point> &points) {
  unordered_set<Point> newPoints;
  int height = grid.size(), width = grid[0].size();

  for (const auto &p : points)
    for (auto &[xi, yi] : DIRECTIONS) {
      Point newP = {p.x + xi, p.y + yi};

      // Checks
      if (newP.x < 0 || newP.y < 0 || newP.x >= height || newP.y >= width)
        continue;
      if (!grid[newP.x][newP.y])
        continue;

      // Insert
      newPoints.insert(newP);
    }

  return newPoints;
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

  // Parse lines
  string line;
  Grid grid;
  Point start;
  for (int i = 0; getline(inputFile, line); i++)
    if (!line.empty()) {
      vector<bool> row;
      transform(line.begin(), line.end(), back_inserter(row),
                [](char c) { return (c == '#') ? false : true; });
      grid.push_back(row);

      auto startPos = line.find('S');
      if (startPos != string::npos) {
        start.x = startPos;
        start.y = line.find('S');
      }
    }

  // Find paths
  unordered_set<Point> s{start};
  for (int i = 0; i < 64; i++)
    s = getReach(grid, s);

  cout << "Part a: " << s.size() << endl;

  // Part B
  // https://github.com/villuna/aoc23/wiki/A-Geometric-solution-to-advent-of-code-2023,-day-21

  long n = 202300; // 26501365 = 65 + (202300 * 131)

  for (int i = 0; i < 100; i++) // Odd
    s = getReach(grid, s);

  long oddFull = s.size();
  long oddCorners = accumulate(s.begin(), s.end(), 0, [](int acc, Point p) {
    return acc + ((abs(p.x - 65) + abs(p.y - 65) > 65) ? 1 : 0);
  });

  s = getReach(grid, s); // Even

  long evenFull = s.size();
  long evenCorners = accumulate(s.begin(), s.end(), 0, [](int acc, Point p) {
    return acc + ((abs(p.x - 65) + abs(p.y - 65) > 65) ? 1 : 0);
  });

  long result = ((n + 1) * (n + 1)) * oddFull + (n * n) * evenFull -
                (n + 1) * oddCorners + n * evenCorners;

  cout << "Part b: " << result << endl;

  // Close file
  inputFile.close();

  return 0;
}
