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

int part_a(const vector<string> &cards) {
  int total = 0;
  int next;

  for (const auto &card : cards) {
    stringstream stream(card);
    stream.ignore(max_discard, ':');

    unordered_set<int> s;
    while (stream >> next)
      s.insert(next);

    if (stream.fail())
      stream.clear();
    stream.ignore(max_discard, '|');

    int sum = 0;
    while (stream >> next)
      if (s.contains(next))
        sum = (sum == 0) ? 1 : 2 * sum;

    total += sum;
  }
  return total;
}

int part_b(const vector<string> &cards) {
  int next;

  // Stores all cards we won
  vector<int> ncards(cards.size(), 1);

  for (int i = 0; i < cards.size(); i++) {
    int copies = ncards[i];

    stringstream stream(cards[i]);
    stream.ignore(max_discard, ':');

    unordered_set<int> s;
    while (stream >> next)
      s.insert(next);

    if (stream.fail())
      stream.clear();
    stream.ignore(max_discard, '|');

    int sum = 0;
    while (stream >> next) {
      if (s.contains(next))
        sum++;
    }

    for (int j = 1; j <= sum; j++)
      if (i + j < cards.size())
        ncards[i + j] += copies;
  }

  return accumulate(ncards.begin(), ncards.end(), 0);
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
