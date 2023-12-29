#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_set>
#include <utility>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

using Grid = vector<vector<char>>;

const int MAX_GRID_SIZE = 142 * 142;
using BitSet = bitset<MAX_GRID_SIZE>;

static const std::pair<int, int> DIRS[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

static const vector<tuple<int, int, char>> DIRECTIONS = {
    {1, 0, '^'},  // South
    {-1, 0, 'v'}, // North
    {0, 1, '<'},  // Right
    {0, -1, '>'}, // Left
};

static const vector<tuple<char, int, int>> SLIDES = {
    {'v', 1, 0},  // South
    {'^', -1, 0}, // North
    {'>', 0, 1},  // Right
    {'<', 0, -1}, // Left
};

struct Point {
  int x;
  int y;

  bool operator==(const Point &other) const {
    return x == other.x && y == other.y;
  }
};

template <> struct hash<Point> {
  std::size_t operator()(const Point &p) const {

    std::size_t h1 = std::hash<int>{}(p.x);
    std::size_t h2 = std::hash<int>{}(p.y);

    return h1 ^ (h2 << 1);
  }
};

// Depth-first search
// Take into account the slides
int dfsPartA(const Grid &grid, unordered_set<Point> &visited, Point p) {
  int height = grid.size(), width = grid[0].size();

  // Termination condition
  if (p.x >= height - 1) // Single path tile bottom
    return visited.size() - 1;

  // Sliding
  char c = grid[p.x][p.y];
  auto it = find_if(SLIDES.begin(), SLIDES.end(),
                    [c](auto &s) { return get<0>(s) == c; });
  if (it != SLIDES.end()) {
    Point newP = {p.x + get<1>(*it), p.y + get<2>(*it)};
    visited.insert(newP);
    int result = dfsPartA(grid, visited, newP);
    visited.erase(newP);
    return result;
  }

  // Advance
  int result = -1;
  for (auto &[i, j, a] : DIRECTIONS) {
    Point newP{p.x + i, p.y + j};
    if (newP.x < 0 || newP.y < 0 || newP.x >= height || newP.y >= width ||
        grid[newP.x][newP.y] == '#')
      continue;

    // Cannot slide reverse
    if (grid[newP.x][newP.y] == a)
      continue;

    // Cannot go on same tile twice
    if (visited.contains(newP))
      continue;

    visited.insert(newP);
    result = max(result, dfsPartA(grid, visited, newP));
    visited.erase(newP);
  }

  return result;
}

// Converts 2D coordinates to a 1D index
inline int getIndex(int x, int y, int width) { return x * width + y; }

// Depth-first search
// Ignore slides
int dfsPartB(const BitSet &grid, BitSet &visited, int x, int y, int height,
             int width, int length) {
  // Termination condition
  if (x >= height - 1)
    return length;

  // Advance
  int result = -1;
  __attribute__((opencl_unroll_hint)) for (const auto &[dx, dy] : DIRS) {
    int newX = x + dx, newY = y + dy;
    int index = getIndex(newX, newY, width);

    if (newX < 0 || newY < 0 || newX >= height || newY >= width ||
        grid[index] || visited[index])
      continue;

    visited.set(index);
    result = std::max(
        result, dfsPartB(grid, visited, newX, newY, height, width, length + 1));
    visited.reset(index);
  }

  return result;
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
  Grid grid;
  for (string line; getline(inputFile, line);)
    if (!line.empty()) {
      vector<char> row;
      row.reserve(line.size());
      row.insert(row.end(), line.begin(), line.end());
      grid.push_back(std::move(row));
    }

  // DFS
  unordered_set<Point> visited{{0, 1}};
  cout << "Part a: " << dfsPartA(grid, visited, {0, 1}) << endl;

  // Optimization
  // Build vectors of bools that are smaller
  BitSet bitGrid;
  BitSet bitVisited;
  int height = grid.size(), width = grid[0].size();
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      if (grid[i][j] == '#')
        bitGrid.set(getIndex(i, j, width));

  cout << "Part b: " << dfsPartB(bitGrid, bitVisited, 0, 1, height, width, 0)
       << endl;

  // Close file
  inputFile.close();

  return 0;
}
