#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

namespace Day19 {
using namespace std;

struct Rules {
  char var;
  char op;
  int val;
  string dest;
};

struct Workflow {
  vector<Rules> rules;
  string fallback;
};

template <typename T> struct Input {
  T x;
  T m;
  T a;
  T s;
  string rule = "";
};

// Avoid memory access of dict
template <typename T> T &getValue(Input<T> &input, char c) {
  switch (c) {
  case 'x':
    return input.x;
  case 'm':
    return input.m;
  case 'a':
    return input.a;
  default:
    return input.s;
  };
}

// Parse first part - Workflows
unordered_map<string, Workflow> getWorkflows(string input) {
  stringstream stream(input);
  unordered_map<string, Workflow> workfows;

  std::regex pattern("(\\w+)([<>]{1})(\\d+):(\\w+),");

  for (string line; getline(stream, line);) {
    if (!line.empty()) {
      auto startPos = line.find('{') + 1;
      string key = line.substr(0, startPos - 1);

      auto words_begin =
          std::sregex_iterator(line.begin(), line.end(), pattern);
      auto words_end = std::sregex_iterator();

      vector<Rules> rules;
      for (auto i = words_begin; i != words_end; ++i) {
        const auto &match = *i;
        rules.push_back({.var = match[1].str()[0],
                         .op = match[2].str()[0],
                         .val = stoi(match[3].str()),
                         .dest = match[4].str()});
      }

      auto endPos = line.rfind('}');
      auto lastCommaPos = line.rfind(',') + 1;
      string fallback = line.substr(lastCommaPos, endPos - lastCommaPos);

      workfows[key] = {std::move(rules), std::move(fallback)};
    }
  }

  return workfows;
}

// Parse second part - Inputs
vector<Input<int>> getInputs(string input) {
  stringstream stream(input);
  std::regex patternVars("(\\w+)=(\\d+)");
  vector<Input<int>> inputs;

  for (string line; getline(stream, line);)
    if (!line.empty()) {
      auto words_begin =
          std::sregex_iterator(line.begin(), line.end(), patternVars);
      auto words_end = std::sregex_iterator();

      Input<int> input;
      for (auto i = words_begin; i != words_end; ++i) {
        const auto &match = *i;
        char c = match[1].str()[0];
        int value = stoi(match[2].str());
        getValue(input, c) = value;
      }

      inputs.push_back(input);
    }

  return inputs;
}

// Send input through workflows
char execute(Input<int> &input,
             const unordered_map<string, Workflow> &workflows) {
  string rule = "in";
  while (rule != "A" && rule != "R") {
    bool matched = false;
    const auto &workflow = workflows.at(rule);

    for (auto &r : workflow.rules) {
      if (r.op == '<' && getValue(input, r.var) < r.val) {
        rule = r.dest;
        matched = true;
        break;
      }
      if (r.op == '>' && getValue(input, r.var) > r.val) {
        rule = r.dest;
        matched = true;
        break;
      }
    }
    if (!matched)
      rule = workflow.fallback;
  }

  return rule[0];
}

// Get all possible combinations
long long getAllComb(const unordered_map<string, Workflow> &workflows) {
  long long count = 0;
  queue<Input<pair<int, int>>> q;
  q.push({
      .x = {1, 4000},
      .m = {1, 4000},
      .a = {1, 4000},
      .s = {1, 4000},
      .rule = "in",
  });

  while (!q.empty()) {
    // Get top element
    auto range = q.front();
    q.pop();

    // Termination
    if (range.rule == "R") // Rejected
      continue;
    if (range.rule == "A") { // Accepted
      long long add = 1;
      for (auto c : {'x', 'm', 'a', 's'})
        add *= getValue(range, c).second - getValue(range, c).first + 1;
      count += add;
      continue;
    }

    // Exclude bad sizes
    bool exclude = false;
    for (auto c : {'x', 'm', 'a', 's'})
      if (getValue(range, c).second < getValue(range, c).first)
        exclude = true;
    if (exclude)
      continue;

    // Apply filters
    bool matched = false;
    const auto &workflow = workflows.at(range.rule);

    for (const auto &r : workflow.rules) {
      if (r.op == '<' && getValue(range, r.var).first < r.val) {
        // New item
        Input<pair<int, int>> newRange = range; // Copy all
        getValue(newRange, r.var).second =
            min(r.val - 1, getValue(newRange, r.var).second);
        newRange.rule = r.dest;
        q.push(newRange);
        // Old item
        getValue(range, r.var).first = max(r.val, getValue(range, r.var).first);
        q.push(range);
        matched = true;
        break;
      }
      if (r.op == '>' && getValue(range, r.var).second > r.val) {
        // New item
        Input<pair<int, int>> newRange = range; // Copy all
        getValue(newRange, r.var).first =
            max(r.val + 1, getValue(newRange, r.var).first);
        newRange.rule = r.dest;
        q.push(newRange);
        // Old item
        getValue(range, r.var).second =
            min(r.val, getValue(range, r.var).second);
        q.push(range);
        matched = true;
        break;
      }
    }

    // No filters matched
    // Fallback
    if (!matched) {
      range.rule = workflow.fallback;
      q.push(range);
    }
  }

  return count;
}

auto parse(const string_view &content) {
  return string(content.begin(), content.end());
}

long long part_one(const string &content) {
  auto workflows = getWorkflows(content.substr(0, content.find("\n\n")));
  auto inputs = getInputs(content.substr(content.find("\n\n")));

  long long count = 0;
  for (auto &i : inputs)
    if (execute(i, workflows) == 'A')
      count += i.x + i.m + i.a + i.s;

  return count;
}

long long part_two(const string &content) {
  auto workflows = getWorkflows(content.substr(0, content.find("\n\n")));
  auto inputs = getInputs(content.substr(content.find("\n\n")));

  return getAllComb(workflows);
}
} // namespace Day19
