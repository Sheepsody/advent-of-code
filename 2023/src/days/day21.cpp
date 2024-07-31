#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

namespace std {
struct Point21 {
  int x;
  int y;

  bool operator<(const Point21 &p) const {
    return x < p.x || (x == p.x && y < p.y);
  }

  bool operator==(const Point21 &p) const { return x == p.x && y == p.y; }
};

template <> struct hash<Point21> {
  std::size_t operator()(const Point21 &p) const {

    std::size_t h1 = std::hash<int>{}(p.x);
    std::size_t h2 = std::hash<int>{}(p.y);

    return h1 ^ (h2 << 1);
  }
};
} // namespace std

namespace Day21 {
using namespace std;

constexpr int DIRECTIONS[4][2] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

using Grid = vector<vector<bool>>;

// Get reachable garden plots one step
unordered_set<Point21> getReach(const Grid &grid,
                              const unordered_set<Point21> &points) {
  unordered_set<Point21> newPoint21s;
  int height = grid.size(), width = grid[0].size();

  for (const auto &p : points)
    for (auto &[xi, yi] : DIRECTIONS) {
      Point21 newP = {p.x + xi, p.y + yi};

      // Checks
      if (newP.x < 0 || newP.y < 0 || newP.x >= height || newP.y >= width)
        continue;
      if (!grid[newP.x][newP.y])
        continue;

      // Insert
      newPoint21s.insert(newP);
    }

  return newPoint21s;
}

tuple<Grid, Point21> parse(const string_view &content) {

  Grid grid;
  Point21 start;
  for (auto line : content | std::views::split('\n') |
                       std::views::filter([](auto x) { return !x.empty(); })) {
    vector<bool> row;
    transform(line.begin(), line.end(), back_inserter(row),
              [](char c) { return (c == '#') ? false : true; });
    grid.push_back(row);

    string sline = string(line.begin(), line.end());

    auto startPos = sline.find('S');
    if (startPos != string::npos) {
      start.x = startPos;
      start.y = sline.find('S');
    }
  }

  return make_tuple(grid, start);
}

auto part_one(const tuple<Grid, Point21> &input) {
  const auto &[grid, start] = input;

  unordered_set<Point21> s{start};
  for (int i = 0; i < 64; i++) {
    s = getReach(grid, s);
  }

  return s.size();
}

auto part_two(const tuple<Grid, Point21> &input) {
  const auto &[grid, start] = input;

  unordered_set<Point21> s{start};

  for (int i = 0; i < 64; i++) {
    s = getReach(grid, s);
  }

  long n = 202300; // 26501365 = 65 + (202300 * 131)

  for (int i = 0; i < 100; i++) // Odd
    s = getReach(grid, s);

  long oddFull = s.size();
  long oddCorners = accumulate(s.begin(), s.end(), 0, [](int acc, Point21 p) {
    return acc + ((abs(p.x - 65) + abs(p.y - 65) > 65) ? 1 : 0);
  });

  s = getReach(grid, s); // Even

  long evenFull = s.size();
  long evenCorners = accumulate(s.begin(), s.end(), 0, [](int acc, Point21 p) {
    return acc + ((abs(p.x - 65) + abs(p.y - 65) > 65) ? 1 : 0);
  });

  long result = ((n + 1) * (n + 1)) * oddFull + (n * n) * evenFull -
                (n + 1) * oddCorners + n * evenCorners;

  return result;
}
} // namespace Day21
