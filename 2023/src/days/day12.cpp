#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <utility>

using Key = std::tuple<int, int, int>;

namespace std {
template <> struct hash<Key> {
  size_t operator()(const Key &p) const {
    const auto [v1, v2, v3] = p;
    size_t hash1 = hash<int>{}(v1);
    size_t hash2 = hash<int>{}(v2);
    size_t hash3 = hash<int>{}(v3);

    return hash1 + (hash2 << 1) + (hash3 << 2);
  }
};

} // namespace std

namespace Day12 {
using namespace std;

struct Record {
  string rec;
  vector<int> cnt;
};

vector<int> parseGroups(const string &groupsStr) {
  vector<int> groups;
  stringstream ss(groupsStr);
  for (string item; getline(ss, item, ',');) {
    groups.push_back(stoi(item));
  }
  return groups;
}

Record unfold(const Record &r) {
  std::stringstream ss;
  for (int i = 1; i <= 5; i++)
    ss << r.rec << (i == 5 ? "" : "?");

  vector<int> counts;
  for (int i = 0; i < 5; i++)
    counts.insert(counts.end(), r.cnt.begin(), r.cnt.end());

  return Record({
      ss.str(),
      counts,
  });
}

// Check that a given combination is valid
bool isValid(const string &record, const vector<int> &counts) {
  int cnt = 0, idx = 0;
  int countSize = counts.size();

  for (char c : record) {
    if (c == '#')
      cnt++;
    else if (cnt > 0) {
      // Count is valid
      if (idx >= countSize || counts[idx] != cnt)
        return false;
      cnt = 0;
      idx++;
    }
  }

  // Check last cnt valid
  return (idx == countSize && cnt == 0) ||
         (idx == countSize - 1 && cnt == counts[idx]);
}

// Counts the number of combinations
// NOTE: returns unchanged string
int countCombinations(int idx, string &record, vector<int> &counts) {
  // Termination condition
  if (idx == (int)record.size())
    return isValid(record, counts) ? 1 : 0;

  if (record[idx] != '?')
    return countCombinations(idx + 1, record, counts);

  record[idx] = '.';
  int cnt = countCombinations(idx + 1, record, counts);
  record[idx] = '#';
  cnt += countCombinations(idx + 1, record, counts);

  // Reset
  record[idx] = '?';
  return cnt;
}

long optimizedCount(int idx, int cntIdx, const string &record,
                    const vector<int> &counts, int currCnt,
                    unordered_map<Key, long> &mem) {
  // Termination condition
  int countSize = counts.size();
  if (idx == (int)record.size()) {
    if (cntIdx == countSize && currCnt == 0)
      return 1;
    if (cntIdx == countSize - 1 && currCnt == counts[cntIdx])
      return 1;
    return 0;
  }

  // Use memoization to avoid redundant calculations
  Key key({idx, cntIdx, currCnt});
  if (mem.find(key) != mem.end()) {
    return mem[key];
  }

  long count = 0;

  // Handle the case when the current spring is broken or unknown
  if (record[idx] == '#' || record[idx] == '?') {
    count += optimizedCount(idx + 1, cntIdx, record, counts, currCnt + 1, mem);
  }

  // Handle the case when the current spring is operational or unknown
  if (record[idx] == '.' || record[idx] == '?') {
    if (currCnt == 0 || (cntIdx < countSize && currCnt == counts[cntIdx])) {
      count += optimizedCount(idx + 1, (currCnt == 0) ? cntIdx : cntIdx + 1,
                              record, counts, 0, mem);
    }
  }

  // Store the result in the memoization map
  mem[key] = count;
  return count;
}

vector<Record> parse(const string_view &content) {
  vector<Record> records;

  for (auto line : content | std::views::split('\n') |
                       std::views::filter([](auto x) { return !x.empty(); })) {
    auto s = string(string(line.begin(), line.end()));
    size_t spacePos = s.find(' ');
    if (spacePos == string::npos)
      continue;
    records.push_back(
        {s.substr(0, spacePos), parseGroups(s.substr(spacePos + 1))});
  }

  return records;
}

int part_one(const vector<Record> &records) {
  int cnt = 0;
  for (auto &r : records) {
    unordered_map<tuple<int, int, int>, long> mem;
    cnt += optimizedCount(0, 0, r.rec, r.cnt, 0, mem);
  }
  return cnt;
}

long long part_two(const vector<Record> &records) {
  vector<Record> unfoldedRecords;
  for (auto &record : records)
    unfoldedRecords.push_back(unfold(record));

  long long longCnt = 0;
  for (auto &r : unfoldedRecords) {
    unordered_map<tuple<int, int, int>, long> mem;
    longCnt += optimizedCount(0, 0, r.rec, r.cnt, 0, mem);
  }
  return longCnt;
}
} // namespace Day12
