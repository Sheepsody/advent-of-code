#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <utility>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

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

  // Part a : tilt
  tiltGrid(grid, Direction::NORTH);
  cout << "Part a: " << getLoad(grid) << endl;
  tiltGrid(grid, Direction::SOUTH); // Reset

  // Part b : 1e9 cycles
  // Find cycle
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

  cout << "Part b: " << cycleLoads[loadIndex] << endl;

  // Close file
  inputFile.close();

  return 0;
}
