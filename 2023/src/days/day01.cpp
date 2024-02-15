#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string_view>

namespace Day01 {

constexpr auto decimal = [](auto &&c) { return c - '0'; };
constexpr auto is_digit = [](auto &&c) { return isdigit(c); };

int part_one(std::string_view sv) {
  auto view = sv | std::views::split('\n') |
              std::views::filter([](auto &&s) { return !s.empty(); });

  return std::accumulate(view.begin(), view.end(), 0, [](auto sum, auto line) {
    auto s = line | std::views::filter(is_digit);
    auto it = s.begin();

    int first_digit = *it++ - '0';
    int last_digit = *std::prev(s.end()) - '0';

    return sum + first_digit * 10 + last_digit;
  });
}

constexpr std::array<std::string_view, 9> SDIGITS = {
    "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

int to_digit(std::string_view sub) {
  if (isdigit(sub[0]))
    return decimal(sub[0]);

  auto v = std::find_if(SDIGITS.begin(), SDIGITS.end(),
                        [sub](auto &&s) { return sub.starts_with(s); });
  if (v != SDIGITS.end())
    return std::distance(SDIGITS.begin(), v) + 1;

  return -1;
}

int part_two(std::string_view sv) {
  auto view = sv | std::views::split('\n') |
              std::views::filter([](auto &&s) { return !s.empty(); });

  return std::accumulate(view.begin(), view.end(), 0, [](auto sum, auto line) {
    int first = -1;
    int last = -1;
    for (auto it : std::views::iota(line.begin(), line.end())) {
      std::string_view sub(it, line.end());
      if (auto d = to_digit(sub); d != -1) {
        if (first == -1)
          first = d;
        last = d;
      }
    }
    return sum + first * 10 + last;
  });
}
} // namespace Day01
