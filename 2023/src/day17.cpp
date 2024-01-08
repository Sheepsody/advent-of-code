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
#include <vector>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

enum class Direction { NORTH, SOUTH, EAST, WEST };

const vector<tuple<Direction, Direction, int, int>> DIRECTIONS = {
    {Direction::SOUTH, Direction::NORTH, 1, 0},
    {Direction::NORTH, Direction::SOUTH, -1, 0},
    {Direction::EAST, Direction::WEST, 0, 1},
    {Direction::WEST, Direction::EAST, 0, -1},
};

struct Node {
  int x;
  int y;
  Direction d;
  int heat;
  int priority;
  int straight;

  inline bool operator>(const Node &other) const {
    return priority > other.priority;
  }
};

// Use A* algorithm
int getMinLength(const vector<vector<int>> &grid,
                 int minStraight, // Min same direction
                 int maxStraigh   // Max same direction
) {
  int height = grid.size(), width = grid[0].size();
  priority_queue<Node, vector<Node>, greater<Node>> pq;
  int dist = height + width - 2;
  pq.push({0, 0, Direction::EAST, 0, dist, 0});
  pq.push({0, 0, Direction::SOUTH, 0, dist, 0});

  // Store past distances with straight info & direction
  vector<vector<vector<int>>> distances(
      height, vector<vector<int>>(width, vector<int>(maxStraigh * 4, INT_MAX)));

  while (!pq.empty()) {
    Node p = pq.top();
    pq.pop();

    int straight = static_cast<int>(p.d) * maxStraigh + p.straight - 1;
    if (p.straight != -1 && distances[p.x][p.y][straight] < p.priority)
      continue;
    distances[p.x][p.y][straight] = p.priority;

    if (p.x == height - 1 && p.y == width - 1 && p.straight >= minStraight)
      return p.heat;

    for (auto [di, dr, xi, yi] : DIRECTIONS) {
      int newX = p.x + xi, newY = p.y + yi;
      if (newX < 0 || newX >= height || newY < 0 || newY >= width)
        continue;

      if ((p.d == dr)                                 // Cannot go reverse
          || (p.d == di && p.straight >= maxStraigh)  // Cannot exceed
          || (p.d != di && p.straight < minStraight)) // Cannot be less
        continue;

      // Add new point
      int newH = p.heat + grid[newX][newY];
      int newS = (p.d == di) ? p.straight + 1 : 1;
      int newP = newH + height - newX - 1 + width - newY - 1; // No need for abs

      int dInt = static_cast<int>(di) * maxStraigh + newS - 1;

      if (newP < distances[newX][newY][dInt]) {
        distances[newX][newY][dInt] = newP;
        pq.push({newX, newY, di, newH, newP, newS});
      }
    }
  }

  return -1;
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
  vector<vector<int>> grid;
  for (string line; getline(inputFile, line);)
    if (!line.empty()) {
      vector<int> parsedLine;
      transform(line.begin(), line.end(), back_inserter(parsedLine),
                [](char c) { return c - '0'; });
      grid.push_back(std::move(parsedLine));
    };

  cout << "Part a: " << getMinLength(grid, -1, 3) << endl;
  cout << "Part b: " << getMinLength(grid, 4, 10) << endl;

  // Close file
  inputFile.close();

  return 0;
}
