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

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

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

  std::stringstream buffer;
  buffer << inputFile.rdbuf();
  string filecontent = buffer.str();

  auto workflows =
      getWorkflows(filecontent.substr(0, filecontent.find("\n\n")));
  auto inputs = getInputs(filecontent.substr(filecontent.find("\n\n")));

  // Part a
  long long count = 0;
  for (auto &i : inputs)
    if (execute(i, workflows) == 'A')
      count += i.x + i.m + i.a + i.s;
  cout << "Part a: " << count << endl;

  // Part b

  // Close file
  inputFile.close();

  return 0;
}
