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

int getIntCode(const string &line) {
  return accumulate(line.begin(), line.end(), 0,
                    [](int a, char c) { return a * 26 + (c - 'A'); });
}

template <typename T> T lcm(T a, T b) { return (a * b) / std::gcd(a, b); }

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
  for (std::string line; std::getline(inputFile, line);)
    lines.emplace_back(std::move(line));

  // Parse lines
  std::vector<char> instructions;
  instructions.reserve(lines[0].size());
  for (const auto &c : lines[0])
    if (std::isalpha(c))
      instructions.push_back(c);

  // Parse instructions
  struct Node {
    int left;
    int right;
    bool is_terminal;
  };

  vector<int> nodes;
  unordered_map<int, Node> network;

  for (const auto &line : lines)
    if (line.size() > 15) {
      int node_code = getIntCode(line.substr(0, 3));
      Node node = {getIntCode(line.substr(7, 3)),
                   getIntCode(line.substr(12, 3)), false};

      if (line.substr(0, 3).find('A') != string::npos)
        nodes.push_back(node_code);
      if (line.substr(0, 3).find('Z') != string::npos)
        node.is_terminal = true;

      network[node_code] = node;
    }

  int current = getIntCode("AAA");
  int target = getIntCode("ZZZ");

  int i = 0;
  for (; current != target; ++i) {
    char dir = instructions[i % instructions.size()];
    current = (dir == 'L') ? network[current].left : network[current].right;
  }

  cout << "Part a: " << i << endl;

  vector<long> end_times;
  for (auto n : nodes) {
    int j = 0;
    for (; !network[n].is_terminal; ++j) {
      char dir = instructions[j % instructions.size()];
      n = (dir == 'L') ? network[n].left : network[n].right;
    }
    end_times.push_back(j);
  }

  long part_2 = accumulate(end_times.begin() + 1, end_times.end(), end_times[0],
                           [](long a, long b) { return lcm(a, b); });

  cout << "Part b: " << part_2 << endl;

  // Close file
  inputFile.close();

  return 0;
}
