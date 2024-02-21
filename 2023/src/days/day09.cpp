#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <unordered_set>

namespace Day09 {
using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

vector<vector<int>> parse(const string_view &content) {
  vector<vector<int>> records;
  for (auto line : content | std::views::split('\n') |
                       std::views::filter([](auto x) { return !x.empty(); })) {
    vector<int> new_record;
    stringstream stream(string(line.begin(), line.end()));
    for (int num; stream >> num;)
      new_record.push_back(num);
    records.push_back(std::move(new_record));
  }
  return records;
}

int part_one(const vector<vector<int>> &records) {
  // Part A
  int sum = 0;
  for (const auto &const_record : records) {
    vector<int> record;
    record.reserve(const_record.size());
    copy(const_record.begin(), const_record.end(), back_inserter(record));

    int length = record.size();
    while (!all_of(record.begin(), record.begin() + length,
                   [](int x) { return x == 0; })) {
      length -= 1;
      for (int j = 0; j < length; j++)
        record[j] = record[j + 1] - record[j];
    }

    sum += accumulate(record.begin(), record.end(), 0);
  }

  return sum;
}

int part_two(const vector<vector<int>> &records) {

  // Part B
  int sum = 0;
  for (const auto &const_record : records) {
    vector<int> record;
    record.reserve(const_record.size());
    copy(const_record.begin(), const_record.end(), back_inserter(record));

    vector<int> beginning;
    int length = record.size();
    while (!all_of(record.begin(), record.begin() + length,
                   [](int x) { return x == 0; })) {
      beginning.push_back(record[0]);
      length -= 1;
      for (int j = 0; j < length; j++)
        record[j] = record[j + 1] - record[j];
    }

    sum += accumulate(beginning.rbegin(), beginning.rend(), 0,
                      [](int a, int b) { return b - a; });
  }
  return sum;
}
} // namespace Day09
