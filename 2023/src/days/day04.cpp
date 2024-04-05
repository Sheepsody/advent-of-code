#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace Day04 {
using namespace std;

constexpr auto max_discard = numeric_limits<streamsize>::max();

using Input = vector<tuple<vector<int>, vector<int>>>;

Input parse(const string_view sv) {
  Input cards;
  int next;

  for (auto line :
       sv | std::views::split('\n') |
           std::views::filter([](auto &&x) { return !x.empty(); })) {

    stringstream stream(string(line.begin(), line.end()));
    stream.ignore(max_discard, ':');

    vector<int> s;
    while (stream >> next)
      s.push_back(next);

    if (stream.fail())
      stream.clear();
    stream.ignore(max_discard, '|');

    vector<int> s2;
    while (stream >> next)
      s2.push_back(next);

    cards.push_back({s, s2});
  }
  return cards;
}

int part_one(const Input &cards) {
  int total = 0;

  for (const auto &card : cards) {
    unordered_set<int> s;
    for (auto n : std::get<0>(card))
      s.insert(n);

    int sum = 0;
    for (auto n : std::get<1>(card))
      if (s.contains(n))
        sum = (sum == 0) ? 1 : 2 * sum;

    total += sum;
  }
  return total;
}

int part_two(const Input &cards) {
  // Stores all cards we won
  vector<int> ncards(cards.size(), 1);
  int nb = cards.size();

  for (int i = 0; i < nb; i++) {
    int copies = ncards[i];

    unordered_set<int> s;
    for (auto n : std::get<0>(cards[i]))
      s.insert(n);

    int sum = 0;
    for (auto n : std::get<1>(cards[i]))
      if (s.contains(n))
        sum++;

    for (int j = 1; j <= sum; j++)
      if (i + j < nb)
        ncards[i + j] += copies;
  }

  return accumulate(ncards.begin(), ncards.end(), 0);
}
} // namespace Day04
