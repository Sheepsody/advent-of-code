#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <unordered_set>

namespace Day11 {
using namespace std;

using Point = pair<int, int>;

struct Input {
  vector<Point> positions;
  vector<int> rowsExpansion;
  vector<int> colsExpansion;
};

Input parse(const string_view &content) {
  vector<string> lines;
  for (auto line : content | std::views::split('\n') |
                       std::views::filter([](auto x) { return !x.empty(); })) {
    lines.push_back(string(line.begin(), line.end()));
  }

  // Parse grid
  vector<vector<int>> matrix(lines.size(), vector<int>(lines[0].size(), 0));
  int height = matrix.size(), width = matrix[0].size();
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      if (lines[i][j] == '#')
        matrix[i][j] = 1;

  // Add rows
  vector<int> rowsExpansion(height, 0);
  for (int i = 0; i < height; i++)
    // Check if the entire row is zero
    if (all_of(matrix[i].begin(), matrix[i].end(),
               [](int x) { return x == 0; }))
      // Insert a new row with zeros
      rowsExpansion[i] = 1;

  // Add columns
  vector<int> colsExpansion(width, 0);
  for (int j = 0; j < width; j++)
    // Check if the entire column is zero
    if (all_of(matrix.begin(), matrix.end(),
               [j](const vector<int> &row) { return row[j] == 0; }))
      // Insert a new column with zeros
      colsExpansion[j] = 1;

  // Print
  vector<Point> positions;
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      if (matrix[i][j] == 1)
        positions.emplace_back(i, j);

  return {
      positions,
      rowsExpansion,
      colsExpansion,
  };
}

int calculateDistance(Point a, Point b, vector<int> rows, vector<int> cols,
                      int expansionFactor) {
  int distance = 0;

  for (int i = min(a.first, b.first); i < max(a.first, b.first); ++i)
    distance += rows[i] * expansionFactor + 1;

  for (int i = min(a.second, b.second); i < max(a.second, b.second); ++i)
    distance += cols[i] * expansionFactor + 1;

  return distance;
}

int part_one(const Input &parsed) {
  int distance = 0;
  int length = parsed.positions.size();

  for (auto i = 0; i < length; ++i)
    for (auto j = i + 1; j < length; ++j)
      distance +=
          calculateDistance(parsed.positions[i], parsed.positions[j],
                            parsed.rowsExpansion, parsed.colsExpansion, 1);

  return distance;
}

long part_two(const Input &parsed) {
  long distance = 0;
  int length = parsed.positions.size();

  for (auto i = 0; i < length; ++i)
    for (auto j = i + 1; j < length; ++j)
      distance +=
          calculateDistance(parsed.positions[i], parsed.positions[j],
                            parsed.rowsExpansion, parsed.colsExpansion, 999999);

  return distance;
}
} // namespace Day11
