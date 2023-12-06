#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_set>

using namespace std;

constexpr auto max_discard = numeric_limits<streamsize>::max();

long part_a(const vector<string> &lines) {
  unordered_set<long> seeds;

  // Parse first line
  stringstream stream(lines[0]);
  stream.ignore(max_discard, ':');
  istream_iterator<long> it(stream), end;
  seeds = unordered_set<long>(it, end);

  unordered_set<long> new_seeds;

  for (long i = 2; i < lines.size(); i++) {
    const string &line = lines[i];

    if (isdigit(line[0])) {
      // Parse mappings
      stringstream stream(lines[i]);

      long dest, source, length;
      stream >> dest >> source >> length;

      vector<long> to_delete;
      for (const auto &s : seeds) {
        if ((s >= source) && (s < source + length)) {
          new_seeds.emplace(dest + s - source);
          to_delete.push_back(s);
        }
      }
      for (auto s : to_delete)
        seeds.erase(s);

    } else if (isalpha(line[0]) && !new_seeds.empty()) {
      // Change mapping
      new_seeds.insert(seeds.begin(), seeds.end());
      seeds = std::move(new_seeds);
    }
  }

  for (const auto &s : seeds)
    new_seeds.insert(s);

  return accumulate(new_seeds.begin(), new_seeds.end(), LONG_MAX,
                    [](auto a, auto b) { return min(a, b); });
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

  // Print results
  cout << "Part a: " << part_a(lines) << endl;

  inputFile.close();

  return 0;
}
