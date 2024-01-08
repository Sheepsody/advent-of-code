
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_set>

using namespace std;

constexpr auto max_discard = numeric_limits<streamsize>::max();
constexpr double Epsilon = 1E-200;

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

  vector<string> lines;
  string line;
  while (std::getline(inputFile, line))
    if (!line.empty())
      lines.push_back(line);

  vector<double> durations = parse_line(lines[0]);
  vector<double> records = parse_line(lines[1]);

  double result = 1;
  for (int i = 0; i < min(durations.size(), records.size()); i++) {
    auto roots = get_roots(durations[i], records[i]);
    result *= roots.second - roots.first + 1;
  }
  cout << "Part a: " << result << endl;

  double duration = parse_line_b(lines[0]);
  double record = parse_line_b(lines[1]);

  auto roots = get_roots(duration, record);
  cout << "Part b: " << (int)round(roots.second - roots.first + 1) << endl;

  inputFile.close();

  return 0;
}
