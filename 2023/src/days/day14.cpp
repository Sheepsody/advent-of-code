#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <utility>

namespace Day14 {
using namespace std;

using Grid = vector<vector<char>>;

enum class Direction { NORTH, SOUTH, EAST, WEST };
const Direction DIRECTIONS[4] = {Direction::NORTH, Direction::WEST,
                                 Direction::SOUTH, Direction::EAST};

size_t hashGrid(const vector<vector<char>> &grid) {
  size_t combinedHash = 0;

  // Hashes the content of a string
  hash<string> hasher;

  for (const auto &row : grid)
    // 0x9e3779b9 -> magic number often used in hash combination
    combinedHash ^= hasher(string(row.begin(), row.end())) + 0x9e3779b9 +
                    (combinedHash << 1);

  return combinedHash;
}

// FIXME Could be cleaner
void tiltGrid(vector<vector<char>> &grid, Direction direction) {
  int height = grid.size();
  int width = grid[0].size();

  switch (direction) {
  case Direction::NORTH:
    for (int j = 0; j < width; j++) {
      int emptyRow = -1;
      for (int i = 0; i < height; i++) {
        if (grid[i][j] == 'O' && emptyRow != -1) {
          grid[emptyRow][j] = 'O';
          grid[i][j] = '.';
          emptyRow++;
        } else if (grid[i][j] == '.') {
          if (emptyRow == -1)
            emptyRow = i;
        } else if (grid[i][j] == '#') {
          emptyRow = -1;
        }
      }
    }
    break;

  case Direction::SOUTH:
    for (int j = 0; j < width; j++) {
      int emptyRow = -1;
      for (int i = height - 1; i >= 0; i--) {
        if (grid[i][j] == 'O' && emptyRow != -1) {
          grid[emptyRow][j] = 'O';
          grid[i][j] = '.';
          emptyRow--;
        } else if (grid[i][j] == '.') {
          if (emptyRow == -1)
            emptyRow = i;
        } else if (grid[i][j] == '#') {
          emptyRow = -1;
        }
      }
    }
    break;

  case Direction::WEST:
    for (int i = 0; i < height; i++) {
      int emptyCol = -1;
      for (int j = 0; j < width; j++) {
        if (grid[i][j] == 'O' && emptyCol != -1) {
          grid[i][emptyCol] = 'O';
          grid[i][j] = '.';
          emptyCol++;
        } else if (grid[i][j] == '.') {
          if (emptyCol == -1)
            emptyCol = j;
        } else if (grid[i][j] == '#') {
          emptyCol = -1;
        }
      }
    }
    break;

  case Direction::EAST:
    for (int i = 0; i < height; i++) {
      int emptyCol = -1;
      for (int j = width - 1; j >= 0; j--) {
        if (grid[i][j] == 'O' && emptyCol != -1) {
          grid[i][emptyCol] = 'O';
          grid[i][j] = '.';
          emptyCol--;
        } else if (grid[i][j] == '.') {
          if (emptyCol == -1)
            emptyCol = j;
        } else if (grid[i][j] == '#') {
          emptyCol = -1;
        }
      }
    }
    break;

  default:
    break;
  }
}

int getLoad(const vector<vector<char>> &grid) {
  int load = 0, height = grid.size();
  for (int i = 0; i < height; i++)
    load += (height - i) * count_if(grid[i].begin(), grid[i].end(),
                                    [](char c) { return c == 'O'; });
  return load;
}

Grid parse(const string_view &content) {
  Grid grid;
  for (auto line : content | std::views::split('\n') |
                       std::views::filter([](auto x) { return !x.empty(); })) {
    grid.push_back(vector<char>(line.begin(), line.end()));
  }
  return grid;
}

int part_one(const Grid &grid) {
  Grid nGrid = grid;
  tiltGrid(nGrid, Direction::NORTH);
  return getLoad(grid);
}

int part_two(const Grid &cgrid) {

  Grid grid = cgrid;

  int loadIndex;
  vector<int> cycleLoads;
  unordered_map<size_t, int> cycleIndexes;

  for (int ncycle = 0; ncycle < 1E9; ncycle++) {

    // One cycle
    for (int i = 0; i < 4; i++)
      tiltGrid(grid, DIRECTIONS[i % 4]);

    // Save to dict
    size_t hash = hashGrid(grid);
    int load = getLoad(grid);

    // Cycle ? (repeated hash)
    if (cycleIndexes.contains(hash)) {
      int baseIndex = cycleIndexes[hash];
      loadIndex =
          baseIndex + ((1000000000 - baseIndex - 1) % (ncycle - baseIndex));
      break;
    }

    cycleIndexes[hash] = ncycle;
    cycleLoads.push_back(load);
  }

  return cycleLoads[loadIndex];
}
} // namespace Day14
