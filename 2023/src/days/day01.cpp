#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

using namespace std;
namespace Day01 {

constexpr auto decimal = [](auto &&c) { return c - '0'; };
constexpr auto is_digit = [](auto &&c) { return isdigit(c); };

int part_one(const string_view &sv) {
  int sum = 0;
  size_t pos = 0;

  while (pos != std::string_view::npos) {
    size_t next = sv.find('\n', pos);
    std::string_view line = sv.substr(pos, next - pos);

    auto first = find_if(line.begin(), line.end(), is_digit);
    auto last = find_if(line.rbegin(), line.rend(), is_digit);

    if (!(first == line.end() || last.base() == line.begin()))
      sum += decimal(*first) * 10 + decimal(*last);

    if (next == std::string_view::npos)
      break;

    pos = next + 1;
  }

  return sum;
}

const string_view digits[9] = {"one", "two",   "three", "four", "five",
                               "six", "seven", "eight", "nine"};

int get_first_char(const string_view &line) {
  for (auto pos = line.begin(); pos != line.end(); pos++) {
    auto view = line.substr(pos - line.begin());

    if (isdigit(view[0]))
      return decimal(view[0]);

    for (auto i = 0; i < 9; i++) {
      if (view.starts_with(digits[i]))
        return i + 1;
    }
  }

  return -1;
}

int get_last_char(const string_view &line) {
  for (auto pos = line.rbegin(); pos != line.rend(); pos++) {
    auto view = string_view(&(*pos));

    if (isdigit(view[0]))
      return decimal(view[0]);

    for (auto i = 0; i < 9; i++) {
      if (view.starts_with(digits[i]))
        return i + 1;
    }
  }

  return -1;
}

int part_two(const string_view &sv) {
  int sum = 0;
  size_t pos = 0;

  while (pos != std::string_view::npos) {
    size_t next = sv.find('\n', pos);
    std::string_view line = sv.substr(pos, next - pos);

    auto first = get_first_char(line);
    auto last = get_last_char(line);

    if (first != -1 && last != -1)
      sum += first * 10 + last;

    if (next == std::string_view::npos)
      break;

    pos = next + 1;
  }

  return sum;
}
} // namespace Day01
