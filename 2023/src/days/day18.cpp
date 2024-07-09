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
#include <utility>

namespace Day18 {
using namespace std;

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

using Plan = vector<tuple<char, int, string>>;

Plan parse(const string_view &content) {
  vector<tuple<char, int, string>> plan;
  for (auto line : content | std::views::split('\n') |
                       std::views::filter([](auto x) { return !x.empty(); })) {
    string_view sv(line.begin(), line.end());
    size_t pos = sv.find(' ', 2);
    if (pos != string_view::npos)
      plan.push_back({sv[0], stoi(string(sv.substr(2, pos - 2))),
                      string(sv.substr(pos + 3, 6))});
  }
  return plan;
}

long part_one(const Plan &plan) { return getNbPoints(plan); }

long part_two(const Plan &plan) {
  vector<tuple<char, int, string>> swapPlan;
  for (const auto &step : plan) {
    string s = get<2>(step);
    char d = getD(s[5]);
    int c = stoi(s.substr(0, 5), nullptr, 16);
    swapPlan.push_back({d, c, ""});
  }
  return getNbPoints(swapPlan);
}
} // namespace Day18
