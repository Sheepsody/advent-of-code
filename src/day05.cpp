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
  vector<long> seeds;

  // Parse first line
  stringstream stream(lines[0]);
  stream.ignore(max_discard, ':');
  istream_iterator<long> it(stream), end;
  seeds = vector<long>(it, end);

  vector<long> new_seeds;
  for (long i = 2; i < lines.size(); i++) {
    const string &line = lines[i];

    if (isdigit(line[0])) {
      // Parse mappings
      stringstream stream(lines[i]);

      long dest, source, length;
      stream >> dest >> source >> length;

      vector<long> unmapped;
      for (const auto &s : seeds) {
        if ((s >= source) && (s < source + length)) {
          new_seeds.push_back(dest + s - source);
        } else {
          unmapped.push_back(s);
        }
      }
      seeds = std::move(unmapped);

    } else if (isalpha(line[0]) && !new_seeds.empty()) {
      // Change mapping
      new_seeds.insert(new_seeds.end(), seeds.begin(), seeds.end());
      seeds = std::move(new_seeds);
    }
  }

  new_seeds.insert(new_seeds.end(), seeds.begin(), seeds.end());

  return accumulate(new_seeds.begin(), new_seeds.end(), LONG_MAX,
                    [](auto a, auto b) { return min(a, b); });
}

long part_b(const vector<string> &lines) {
  vector<pair<long, long>> seeds;

  // Parse first line
  stringstream stream(lines[0]);
  stream.ignore(max_discard, ':');
  long start, length;
  while (stream >> start >> length)
    seeds.push_back(make_pair(start, start + length - 1));

  vector<pair<long, long>> new_seeds;
  for (long i = 2; i < lines.size(); i++) {
    const string &line = lines[i];

    if (isdigit(line[0])) {
      // Parse mappings
      stringstream stream(lines[i]);

      long dest, source, length;
      stream >> dest >> source >> length;

      int seedsLength = seeds.size();
      for (int j = 0; j < seedsLength; j++) {
        const auto &p = seeds[j];
        // Intersection
        if (!(p.second < source || p.first >= source + length)) {
          // New seed
          long destStart = dest + max(0L, p.first - source); // ok
          long destEnd = dest + min(length - 1, p.second - source);
          new_seeds.push_back(make_pair(destStart, destEnd));
          // Start
          if (p.first < source)
            seeds.push_back(make_pair(p.first, source - 1));
          // End
          if (p.second >= source + length)
            seeds.push_back(make_pair(source + length, p.second));
        } else {
          seeds.push_back(p);
        }
      }
      seeds.erase(seeds.begin(), seeds.begin() + seedsLength);

    } else if (isalpha(line[0]) && !new_seeds.empty()) {
      // Change mapping
      new_seeds.insert(new_seeds.end(), seeds.begin(), seeds.end());
      seeds = std::move(new_seeds);
    }
  }

  new_seeds.insert(new_seeds.end(), seeds.begin(), seeds.end());

  return accumulate(new_seeds.begin(), new_seeds.end(), LONG_MAX,
                    [](auto a, auto b) { return min(a, b.first); });
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
  cout << "Part b: " << part_b(lines) << endl;

  inputFile.close();

  return 0;
}
