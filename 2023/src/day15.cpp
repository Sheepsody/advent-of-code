#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

int getCharCode(char c, int n) { return ((n + (c - '\0')) * 17) % 256; }

int getStringCode(const string &s) {
  return accumulate(s.begin(), s.end(), 0,
                    [](int acc, char c) { return getCharCode(c, acc); });
}

enum class Operation { DASH, EQUAL };

struct Step {
  string label;
  Operation ops;
  optional<int> focal;
};

// Trims whitespace from the start of a string
void ltrim(string &s) {
  auto it =
      remove_if(s.begin(), s.end(), [](auto &&ch) { return isspace(ch); });
  s.erase(it, s.end());
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
  vector<string> codes;
  for (string line; getline(inputFile, line, ',');) {
    ltrim(line);
    if (!line.empty())
      codes.push_back(line);
  }

  // Step a: encode all lines
  cout << "Step a: "
       << accumulate(codes.begin(), codes.end(), 0,
                     [](int acc, string s) { return acc + getStringCode(s); })
       << endl;

  // Lines to operations
  vector<Step> steps;
  for (const auto &s : codes) {
    if (s.find('-') != string::npos) {
      // Dash
      auto dash = s.find('-');
      string label = s.substr(0, dash);
      steps.push_back({std::move(label), Operation::DASH});
    } else {
      // Equals
      auto dash = s.find('=');
      string label = s.substr(0, dash);
      int focal = stoi(s.substr(dash + 1));
      steps.push_back({std::move(label), Operation::EQUAL, focal});
    }
  }

  // Apply operations
  unordered_map<int, vector<pair<string, int>>> boxes;
  for (const auto &step : steps) {

    int box = getStringCode(step.label);
    // Add box if not existing
    if (!boxes.contains(box))
      boxes[box] = vector<pair<string, int>>();

    // Operation
    if (step.ops == Operation::DASH) {
      auto &box_content = boxes[box];
      auto new_end =
          std::remove_if(box_content.begin(), box_content.end(),
                         [&](const auto &p) { return p.first == step.label; });
      box_content.erase(new_end, box_content.end());
    } else {
      auto &lenses = boxes[box];
      auto it = find_if(lenses.begin(), lenses.end(),
                        [&](const auto &p) { return p.first == step.label; });
      if (it == lenses.end())
        boxes[box].emplace_back(step.label, step.focal.value_or(0));
      else
        it->second = step.focal.value_or(0);
    }
  }

  int count = 0;
  for (auto &[k, v] : boxes)
    for (int i = 0; i < v.size(); i++)
      count += (k + 1) * (i + 1) * v[i].second;

  cout << "Part b: " << count << endl;

  // Close file
  inputFile.close();

  return 0;
}
