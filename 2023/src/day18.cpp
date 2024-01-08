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

// FIXME Faster then dict ?
pair<int, int> getDirection(char c) {
  switch (c) {
  case 'R':
    return {0, 1};
  case 'L':
    return {0, -1};
  case 'U':
    return {-1, 0};
  case 'D':
    return {1, 0};
  default:
    return {0, 0};
  }
}

// Gauss's area formula
// Determine the area of a simple polygon from vertices
// Note: use Shoelace formmula, returns 2*a
long getArea(const vector<pair<long, long>> &vertices) {
  long area = 0;
  int length = vertices.size();
  for (int i = 0; i < length; i++) {
    // Determinant of 2x2 matrix
    area += vertices[i].first * vertices[(i + 1) % length].second;
    area -= vertices[i].second * vertices[(i + 1) % length].first;
  }
  return abs(area);
}

long getNbPoints(const vector<tuple<char, int, string>> &plan) {
  // Get all vertices
  vector<pair<long, long>> vertices;
  vertices.push_back({0, 0});
  int x = 0, y = 0;
  long nbBnd = 0;
  for (const auto &[d, q, _] : plan) {
    auto m = getDirection(d);
    x += m.first * q;
    y += m.second * q;
    vertices.push_back({x, y});
    nbBnd += q;
  }
  // Pick's theorem : i = a - b/2 + 1
  auto area = getArea(vertices);
  return static_cast<long>(1 + (area + nbBnd) / 2);
}

char getD(char c) {
  switch (c) {
  case '0':
    return 'R';
  case '1':
    return 'D';
  case '2':
    return 'L';
  default:
    return 'U';
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

  // Parse lines
  int pos;
  vector<tuple<char, int, string>> plan;
  for (string line; getline(inputFile, line);)
    if (!line.empty())
      if ((pos = line.find(' ', 2)) != string::npos)
        plan.push_back(
            {line[0], stoi(line.substr(2, pos - 2)), line.substr(pos + 3, 6)});

  // Part a
  cout << "Part a: " << getNbPoints(plan) << endl;

  // Part b
  vector<tuple<char, int, string>> swapPlan;
  for (const auto &step : plan) {
    string s = get<2>(step);
    char d = getD(s[5]);
    int c = stoi(s.substr(0, 5), nullptr, 16);
    swapPlan.push_back({d, c, ""});
  }
  cout << "Part b: " << getNbPoints(swapPlan) << endl;

  // Close file
  inputFile.close();

  return 0;
}
