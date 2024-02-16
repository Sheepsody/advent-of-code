#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <sstream>

namespace Day03 {

using namespace std;

constexpr auto decimal = [](auto &&c) { return c - '0'; };
constexpr auto is_symbol = [](auto &&c) { return !(isdigit(c) or c == '.'); };

const int directions[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                             {0, 1},   {1, -1}, {1, 0},  {1, 1}};

vector<vector<char>> parse(const string_view sv) {
  vector<vector<char>> lines;
  for (auto line :
       sv | std::views::split('\n') |
           std::views::filter([](auto &&x) { return !x.empty(); })) {
    lines.emplace_back(std::vector<char>(line.begin(), line.end()));
  }
  return lines;
}

tuple<int, unordered_map<int, vector<int>>>
get_gears(const vector<vector<char>> &lines) {
  int sum_a = 0;
  int height = lines.size(), width = lines[0].size();

  // Store all part numbers adjacent to a gear
  unordered_map<int, vector<int>> gears;

  for (auto i = 0; i < height; i++) {
    bool is_adjacent = false;
    int sum = 0;
    set<int> gear_positions;

    for (auto j = 0; j <= width; j++) {
      char c = (j != width) ? lines[i][j] : ' ';

      if (isdigit(c)) {
        sum = sum * 10 + decimal(c);

        for (auto const &d : directions) {
          int ni = i + d[0];
          int nj = j + d[1];
          if (ni >= 0 && nj >= 0 && ni < height && nj < width &&
              is_symbol(lines[ni][nj])) {

            is_adjacent = true;

            if (lines[ni][nj] == '*')
              gear_positions.insert(ni * width + nj);
          }
        }
      } else {
        if (is_adjacent) {
          sum_a += sum;

          for (const auto &pos : gear_positions)
            gears[pos].emplace_back(sum);
        }

        gear_positions.clear();
        sum = 0;
        is_adjacent = false;
      }
    }
  }

  return {sum_a, gears};
}

int part_one(const vector<vector<char>> &lines) {
  auto [sum_a, _] = get_gears(lines);
  return sum_a;
}

int part_two(const vector<vector<char>> &content) {
  auto [_, gears] = get_gears(content);

  return std::accumulate(gears.begin(), gears.end(), 0, [](auto sum, auto &g) {
    sum += (g.second.size() == 2) ? g.second[0] * g.second[1] : 0;
    return sum;
  });
}
} // namespace Day03
