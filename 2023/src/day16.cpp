#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <utility>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

enum class Direction { NORTH, SOUTH, EAST, WEST };
const vector<Direction> directions{Direction::NORTH, Direction::SOUTH,
                                   Direction::EAST, Direction::WEST};

struct Point {
  int x;
  int y;
  Direction d;

  friend bool operator==(const Point &lhs, const Point &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && rhs.d == lhs.d;
  }
};

// FIXME Version that works for all tuples
template <> struct hash<Point> {
  std::size_t operator()(const Point &p) const {

    std::size_t h1 = std::hash<int>{}(p.x);
    std::size_t h2 = std::hash<int>{}(p.y);
    std::size_t h3 = std::hash<int>{}(static_cast<int>(p.d));

    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};

// Direction to increments
const unordered_map<Direction, pair<int, int>> increments = {
    {Direction::NORTH, {-1, 0}},
    {Direction::SOUTH, {1, 0}},
    {Direction::EAST, {0, 1}},
    {Direction::WEST, {0, -1}}};

// Map to match directions
const unordered_map<char, unordered_map<Direction, vector<Direction>>>
    directionMap = {
        // Mirror
        {'/',
         {{Direction::NORTH, {Direction::EAST}},
          {Direction::SOUTH, {Direction::WEST}},
          {Direction::EAST, {Direction::NORTH}},
          {Direction::WEST, {Direction::SOUTH}}}},
        // Mirror
        {'\\',
         {{Direction::NORTH, {Direction::WEST}},
          {Direction::SOUTH, {Direction::EAST}},
          {Direction::EAST, {Direction::SOUTH}},
          {Direction::WEST, {Direction::NORTH}}}},
        // Splitter
        {'|',
         {{Direction::EAST, {Direction::NORTH, Direction::SOUTH}},
          {Direction::WEST, {Direction::NORTH, Direction::SOUTH}}}},
        {'-',
         {{Direction::NORTH, {Direction::EAST, Direction::WEST}},
          {Direction::SOUTH, {Direction::EAST, Direction::WEST}}}}};

std::vector<Direction> getDirections(char c, Direction old_dir) {
  // Mirror or splitter
  auto it = directionMap.find(c);
  if (it != directionMap.end()) {
    auto dirIt = it->second.find(old_dir);
    if (dirIt != it->second.end()) {
      return dirIt->second;
    }
  }
  // Empty space
  return {old_dir};
}

int getEnergizedCount(Point initP, const vector<vector<char>> &grid) {
  int counter = 0;
  queue<Point> beams;
  unordered_set<Point> visited;
  beams.push(initP);

  while (!beams.empty()) {
    auto beam = beams.front();
    beams.pop();

    if (beam.x < 0 || beam.y < 0 || beam.x >= grid.size() ||
        beam.y >= grid[0].size())
      continue;

    if (visited.contains(beam))
      continue;
    else if (!any_of(directions.begin(), directions.end(), [&](Direction d) {
               return visited.contains({beam.x, beam.y, d});
             }))
      counter += 1;

    for (Direction d : getDirections(grid[beam.x][beam.y], beam.d)) {
      auto [ii, ij] = increments.at(d);
      int x = beam.x + ii, y = beam.y + ij;
      beams.push({x, y, d});
    }

    visited.insert(beam);
  }

  return counter;
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

  // Parsing
  vector<vector<char>> grid;
  for (string line; getline(inputFile, line);)
    if (!line.empty())
      grid.push_back(vector<char>(line.begin(), line.end()));

  // Part a
  cout << "Part a: " << getEnergizedCount({0, 0, Direction::EAST}, grid)
       << endl;

  // Part b
  int maxCounter = 0;
  int height = grid.size(), width = grid[0].size();
  for (int i = 0; i < height; i++) {
    maxCounter =
        max(maxCounter, getEnergizedCount({i, 0, Direction::EAST}, grid));
    maxCounter =
        max(maxCounter,
            getEnergizedCount({height - i - 1, 0, Direction::WEST}, grid));
  }
  for (int i = 0; i < width; i++) {
    maxCounter =
        max(maxCounter, getEnergizedCount({0, i, Direction::SOUTH}, grid));
    maxCounter =
        max(maxCounter,
            getEnergizedCount({0, width - i - 1, Direction::NORTH}, grid));
  }

  cout << "Part b: " << maxCounter << endl;

  // Close file
  inputFile.close();

  return 0;
}
