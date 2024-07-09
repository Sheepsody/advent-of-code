#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <ranges>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Day15 {
using namespace std;

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

vector<string> parse(const string_view &content) {
  vector<string> codes;
  for (auto vline : content | std::views::split(',') |
                        std::views::filter([](auto x) { return !x.empty(); })) {
    string line = string(vline.begin(), vline.end());
    ltrim(line);
    codes.push_back(line);
  }
  return codes;
}

int part_one(const vector<string> &codes) {
  return accumulate(codes.begin(), codes.end(), 0,
                    [](int acc, string s) { return acc + getStringCode(s); });
}

int part_two(const vector<string> &codes) {
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

  return count;
}
} // namespace Day15
