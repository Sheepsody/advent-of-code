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

namespace Day06 {
using namespace std;

constexpr auto max_discard = numeric_limits<streamsize>::max();
constexpr double Epsilon = 1E-200;

using Input = vector<vector<double>>;

// Solve 2nd order
pair<double, double> get_roots(double duration, double record) {
  auto delta = sqrt(pow(duration, 2) - 4 * record);

  auto r1 = (duration - delta) / 2;
  auto r2 = (duration + delta) / 2;

  r1 = (abs(ceil(r1) - r1) < Epsilon) ? r1 + 1 : ceil(r1);
  r2 = (abs(trunc(r2) - r2) < Epsilon) ? r2 - 1 : trunc(r2);

  return make_pair(r1, r2);
}

vector<double> parse_line(const string &line) {
  vector<double> parsed;
  double next;
  stringstream stream(line);

  stream.ignore(max_discard, ':');
  while (stream >> next)
    parsed.push_back(next);

  return parsed;
}

double parse_line_b(const string &line) {
  double result = 0;
  double next;
  stringstream stream(line);

  stream.ignore(max_discard, ':');
  while (stream >> next)
    result = pow(10, ceil(log10(next))) * result + next;

  return result;
}

Input parse(string_view content) {
  vector<string> lines;
  for (auto line : content | std::views::split('\n'))
    lines.push_back(string(line.begin(), line.end()));

  return {
      parse_line(lines[0]),
      parse_line(lines[1]),
      {parse_line_b(lines[0]), parse_line_b(lines[1])},
  };
}

double part_one(const Input &parsed) {
  double result = 1;
  int length = min(parsed[0].size(), parsed[1].size());
  for (int i = 0; i < length; i++) {
    auto roots = get_roots(parsed[0][i], parsed[1][i]);
    result *= roots.second - roots.first + 1;
  }
  return (int)result;
}

double part_two(const Input &parsed) {
  double duration = parsed[2][0];
  double record = parsed[2][1];
  auto roots = get_roots(duration, record);
  return (int)round(roots.second - roots.first + 1);
}

} // namespace Day06
