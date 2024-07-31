// Note: did second part in python

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string_view>
#include <unordered_set>

namespace Day07 {
using namespace std;

// Displays the bits of a number grouped by four
template <typename T> void display_bits(T value) {
  constexpr size_t num_bits = 8 * sizeof(T);
  for (int i = num_bits - 1; i >= 0; i--) {
    cout << bitset<num_bits>(value)[i];
    if (i % 4 == 0 && i != 0)
      cout << "_";
  }
  cout << endl;
}

int parseCard(char card) {
  if (isdigit(card))
    return (card - '2');
  if (card == 'T')
    return 8;
  if (card == 'J')
    return 9;
  if (card == 'Q')
    return 10;
  if (card == 'K')
    return 11;
  if (card == 'A')
    return 12;
  return -1;
}

uint32_t getHandScore(const vector<int> &parsedCards) {
  // FIXME https://en.cppreference.com/w/cpp/algorithm/transform
  uint32_t order = 0;
  uint16_t strength = 0;
  uint64_t counter = 0;

  for (int score : parsedCards) {
    order = (order << 4) + score;
    counter += ((uint64_t)1) << (4 * score);
  }

  // Masks
  uint64_t mask_one = 0x1111111111111111;
  uint64_t mask_two = 0x2222222222222222;
  uint64_t mask_four = 0x4444444444444444;

  // Easy ones
  // Defined in hierarchy
  uint64_t five_kind = ((counter & mask_four) >> 2) & (counter & mask_one);
  uint64_t four_kind = counter & mask_four;
  uint64_t three_kind = ((counter & mask_two) >> 1) & (counter & mask_one);
  uint64_t two_kind = counter & mask_two;

  // Two pairs
  // FIXME Remove loop ?
  uint64_t counter_pairs = 0;
  for (int i = 0; i < 13; i++)
    counter_pairs += (counter >> (4 * i + 1)) & 0x1;
  bool two_pairs = counter_pairs > 1;

  // Full house
  uint64_t two_kind_exclusive =
      ((counter & mask_two) >> 1) & (~(counter & mask_one));
  bool full_house = (two_kind_exclusive & (~three_kind)) &&
                    (three_kind & (~two_kind_exclusive));

  // Return value of combinations
  if (five_kind)
    strength = 6;
  else if (four_kind)
    strength = 5;
  else if (full_house)
    strength = 4;
  else if (three_kind)
    strength = 3;
  else if (two_pairs)
    strength = 2;
  else if (two_kind)
    strength = 1;

  // Result
  order |= (strength << 20);
  return order;
}

struct Hand {
  string cards;
  uint32_t score;
  int bid;
};

struct {
  bool operator()(Hand a, Hand b) const { return a.score < b.score; }
} handLess;

vector<string> parse(string_view content) {
  vector<string> lines;
  for (auto line : content | std::views::split('\n')) {
    if (line.empty())
      continue;
    lines.push_back(string(line.begin(), line.end()));
  }
  return lines;
}

int part_one(vector<string> &lines) {
  vector<Hand> hands;

  for (string line : lines) {
    string hand = line.substr(0, line.find(' '));

    int bid = stoi(line.substr(line.find(' ') + 1));
    vector<int> parsedCards;
    for (auto c : hand)
      parsedCards.push_back(parseCard(c));
    uint32_t score = getHandScore(parsedCards);

    hands.push_back({hand, score, bid});
  }

  sort(hands.begin(), hands.end(), handLess);

  long result = 0;
  int length = hands.size();
  for (int i = 0; i < length; i++)
    result += (i + 1) * hands[i].bid;
  return result;
}

// TODO
int part_two(vector<string> &lines) { return 0; }
} // namespace Day07
