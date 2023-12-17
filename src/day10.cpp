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

struct Point {
  int x, y;

  bool operator==(const Point &other) const {
    return x == other.x && y == other.y;
  }
};

template <> struct hash<Point> {
  size_t operator()(const Point &p) const {
    auto hash1 = hash<int>{}(p.x);
    auto hash2 = hash<int>{}(p.y);
    return hash1 ^ hash2; // Example: Combine the two hash values
  }
};

struct Transition {
  Point move;
  string source;
  string dest;
};

const string north = "|JLS";

const Transition transitions[] = {
    {{-1, 0}, "|LJ", "|7F"},
    {{1, 0}, "|7F", "|LJ"},
    {{0, -1}, "-7J", "-LF"},
    {{0, 1}, "-LF", "-7J"},
};

// DFS-like to find loop
unordered_set<Point> getLoop(const vector<vector<char>> &lines, Point start) {
  // Find cycle
  int length, ni, nj;
  int i = start.x, j = start.y;
  unordered_set<Point> loop;
  int height = lines.size(), width = lines[0].size();

  // Depth first search for loop
  for (length = 0; length == 0 || lines[i][j] != 'S'; length++) {
    loop.insert({i, j});

    // Find valid move
    for (const auto &m : transitions) {
      ni = i + m.move.x;
      nj = j + m.move.y;
      // Correct indexes
      if (ni >= 0 && nj >= 0 && ni < height && nj < width)
        // Valid source
        if (lines[i][j] == 'S' || m.source.find(lines[i][j]) != string::npos)
          // Finished circle or found destination
          if ((lines[ni][nj] == 'S' && length >= 4) ||
              (!loop.contains({ni, nj}) &&
               m.dest.find(lines[ni][nj]) != string::npos)) {
            i = ni;
            j = nj;
            break;
          }
    }
  }
  return loop;
}

// Flood all areas inside loop
void flood(vector<vector<char>> &lines, const unordered_set<Point> &loop) {
  for (int i = 0; i < lines.size(); i++) {
    int norths = 0;
    for (int j = 0; j < lines[0].size(); j++) {
      if (loop.contains({i, j})) {
        if (north.find(lines[i][j]) != string::npos)
          norths += 1;
      } else
        lines[i][j] = (norths % 2 != 0 ? 'I' : 'O');
    }
  }
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

  // Parse
  int index = 0;
  string line;
  vector<vector<char>> lines;
  Point start;
  for (; getline(inputFile, line); index++) {
    if (line.size() > 0)
      lines.push_back(vector<char>(line.begin(), line.end()));
    int found = line.find('S');
    if (found != string::npos)
      start = {index, found};
  }

  // Matrix to mark points in loop
  auto loop = getLoop(lines, start);

  cout << "Part a: " << loop.size() / 2 << endl;

  flood(lines, loop);

  int result =
      accumulate(lines.begin(), lines.end(), 0, [](int c, vector<char> &v) {
        return c + count(v.begin(), v.end(), 'I');
      });
  cout << "Part b: " << result << endl;

  // Close file
  inputFile.close();

  return 0;
}
