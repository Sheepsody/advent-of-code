#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

struct Hail {
  double px, py, pz;
  double vx, vy, vz;
};

// Parse input with regex
// 19, 13, 30 @ -2,  1, -2
vector<Hail> parse(basic_ifstream<char> &stream) {
  vector<Hail> hails;
  std::regex pattern("([-]*\\d+), ([-]*\\d+), ([-]*\\d+) @ ([-]*\\d+), "
                     "([-]*\\d+), ([-]*\\d+)");
  std::smatch matches;

  for (string line; getline(stream, line);) {
    if (std::regex_search(line, matches, pattern)) {
      vector<double> coords;
      transform(matches.begin() + 1, matches.end(), back_inserter(coords),
                [](auto &&i) { return stod(i.str()); });

      hails.push_back(Hail{
          .px = coords[0],
          .py = coords[1],
          .pz = coords[2],
          .vx = coords[3],
          .vy = coords[4],
          .vz = coords[5],
      });
    }
  };

  return hails;
}

struct Intersection {
  double x;
  double y;
};

std::optional<Intersection> getIntersection(const Hail &a, const Hail &b) {

  double denom = a.vx * b.vy - a.vy * b.vx;
  if (denom == 0)
    return std::nullopt;

  double x = (a.px * (a.py - a.vy) - a.py * (a.px - a.vx)) * b.vx -
             a.vx * (b.px * (b.py - b.vy) - b.py * (b.px - b.vx));
  double y = (a.px * (a.py - a.vy) - a.py * (a.px - a.vx)) * b.vy -
             a.vy * (b.px * (b.py - b.vy) - b.py * (b.px - b.vx));

  return Intersection{x / denom, y / denom};
}

double timeToReach(const Hail &a, const Intersection &i) {
  return (i.x - a.px) / a.vx;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "Missing argument" << endl;
    throw;
  }

  ifstream inputFile(argv[1]);

  double pMin = 200000000000000, pMax = 400000000000000;
  // double pMin = 7, pMax = 27;

  // Check if the file is open
  if (!inputFile.is_open()) {
    cerr << "Error opening the file!" << endl;
    return 1; // Return an error code
  }

  // Parse input
  auto hails = parse(inputFile);
  cout << hails.size() << endl;

  for (auto &h : hails) {
    h.px -= pMin;
    h.py -= pMin;
    h.pz -= pMin;
  }

  int count = 0;

  // Create graph
  for (int i = 0; i < hails.size(); i++)
    for (int j = 0; j < i; j++) {
      auto r = getIntersection(hails[i], hails[j]);

      if (!r.has_value())
        continue;

      if (timeToReach(hails[i], r.value()) < 0 ||
          timeToReach(hails[j], r.value()) < 0)
        continue;

      if (r.value().x < 0 || r.value().x > pMax - pMin)
        continue;

      if (r.value().y < 0 || r.value().y > pMax - pMin)
        continue;

      count++;
    }

  cout << "Part a: " << count << endl;
  //
  // Close file
  inputFile.close();

  return 0;
}
