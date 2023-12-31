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

using Tower = vector<vector<int>>;
using Graph = unordered_map<int, vector<int>>;

struct Brick {
  int id;
  int x1, y1, z1;
  int x2, y2, z2;
};

// Parse input
vector<Brick> parse(basic_ifstream<char> &stream) {
  std::regex pattern("(\\d+),(\\d+),(\\d+)~(\\d+),(\\d+),(\\d+)");
  std::smatch base_match;
  vector<Brick> bricks;

  for (string line; getline(stream, line);) {
    if (std::regex_match(line, base_match, pattern)) {
      vector<int> coords;
      transform(base_match.begin() + 1, base_match.end(), back_inserter(coords),
                [](string s) { return stoi(s); });
      bricks.push_back(Brick{
          .id = static_cast<int>(bricks.size()),
          .x1 = min(coords[0], coords[3]),
          .y1 = min(coords[1], coords[4]),
          .z1 = min(coords[2], coords[5]),
          .x2 = max(coords[0], coords[3]),
          .y2 = max(coords[1], coords[4]),
          .z2 = max(coords[2], coords[5]),
      });
    }
  };

  return bricks;
}

int getMaxCoord(const vector<Brick> &bricks,
                function<int(const Brick &)> accessor) {
  int maxCoordinate = 0;
  for (const auto &brick : bricks) {
    maxCoordinate = max(maxCoordinate, max(accessor(brick), accessor(brick)));
  }
  return maxCoordinate;
}

bool canLowerBrick(const Brick &brick, const Tower &tower, int dimX) {
  if (brick.z1 == 1)
    return false;

  for (int j = brick.y1; j <= brick.y2; j++)
    for (int i = brick.x1; i <= brick.x2; i++)
      if (tower[brick.z1 - 1][j * dimX + i] >= 0)
        return false;

  return true;
}

// Greedy approach from bottom to top
Graph getSupportGraph(vector<Brick> &bricks) {
  // Sort bricks
  sort(bricks.begin(), bricks.end(),
       [](Brick &a, Brick &b) { return a.z1 < b.z1; });

  // Create Tower
  int dimX = getMaxCoord(bricks, [](const Brick &b) { return b.x2; }) + 1;
  int dimY = getMaxCoord(bricks, [](const Brick &b) { return b.y2; }) + 1;
  int dimZ = getMaxCoord(bricks, [](const Brick &b) { return b.z2; }) + 1;
  Tower tower(dimZ, vector<int>(dimX * dimY, -1));

  Graph graph(bricks.size());
  for (const auto &b : bricks)
    graph[b.id] = {};

  for (auto &brick : bricks) {
    // Find brick position
    while (canLowerBrick(brick, tower, dimX)) {
      brick.z1--;
      brick.z2--;
    }

    // Add to stack
    for (int o = brick.z1; o <= brick.z2; o++)
      for (int j = brick.y1; j <= brick.y2; j++)
        for (int i = brick.x1; i <= brick.x2; i++)
          tower[o][j * dimX + i] = brick.id;

    // Add interactions
    int id;
    for (int j = brick.y1; j <= brick.y2; j++)
      for (int i = brick.x1; i <= brick.x2; i++)
        if ((id = tower[brick.z1 - 1][j * dimX + i]) >= 0)
          if (find(graph[id].begin(), graph[id].end(), brick.id) ==
              graph[id].end())
            graph[id].push_back(brick.id);
  }

  return graph;
}

// Find bricks that can be safely deleted without collapse
vector<int> findSafeBricks(const Graph &graph) {
  vector<int> safeBricks;

  // Iterate over each brick in the graph
  for (const auto &[brick, supports] : graph) {
    bool isSafe = true;

    // Every brick needs another support
    for (int supportedBrick : supports) {
      bool isSupported = false;
      // Check every other brick
      for (const auto &[otherBrick, otherSupports] : graph) {
        if (otherBrick != brick &&
            find(otherSupports.begin(), otherSupports.end(), supportedBrick) !=
                otherSupports.end())
          isSupported = true;
      }
      // No support ?
      if (!isSupported) {
        isSafe = false;
        break;
      }
    }

    if (isSafe)
      safeBricks.push_back(brick);
  }

  return safeBricks;
}

// Get all successors of a given mode
unordered_set<int> getAllSuccessors(int brick, const Graph &graph) {
  queue<int> q;
  q.push(brick);
  unordered_set<int> succ;

  while (!q.empty()) {
    int node = q.front();
    q.pop();
    for (int newNode : graph.at(node))
      if (!succ.contains(newNode)) {
        q.push(newNode);
        succ.insert(newNode);
      }
  }

  return succ;
}

// Count desintegration
vector<pair<int, int>> findDesintegrate(const Graph &graph) {
  vector<pair<int, int>> desint;

  // Iterate over each brick in the graph
  for (const auto &[brick, supports] : graph) {
    // Find every childs
    auto successors = getAllSuccessors(brick, graph);

    // Find every unsupported brick
    queue<int> q;
    unordered_set<int> others;
    // Add all nodes that are not successors of brick
    for (const auto &[otherBrick, _] : graph)
      if (otherBrick != brick && !successors.contains(otherBrick))
        q.push(otherBrick);

    while (!q.empty()) {
      int node = q.front();
      q.pop();
      for (int newNode : graph.at(node))
        if (newNode != brick && !others.contains(newNode)) {
          q.push(newNode);
          others.insert(newNode);
        }
    }

    int count = count_if(successors.begin(), successors.end(),
                         [&](int node) { return !others.contains(node); });
    desint.push_back({brick, count});
  }

  return desint;
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

  // Parse input
  auto bricks = parse(inputFile);

  // Create graph
  auto graph = getSupportGraph(bricks);

  // Supported
  cout << "Part a: " << findSafeBricks(graph).size() << endl;

  // Desintegrations
  auto desint = findDesintegrate(graph);
  cout << "Part b: "
       << accumulate(desint.begin(), desint.end(), 0,
                     [](int acc, pair<int, int> &p) { return acc + p.second; })
       << endl;

  // Close file
  inputFile.close();

  return 0;
}
