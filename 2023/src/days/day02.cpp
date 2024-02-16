#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;
namespace Day02 {

constexpr auto max_size = numeric_limits<streamsize>::max();

enum class Color { Red, Green, Blue };

string_view parse(string_view sv) { return sv; }

int part_one(string_view sv) {
  int sum = 0;

  for (auto line :
       sv | ranges::views::split('\n') |
           ranges::views::filter([](auto &&s) { return !s.empty(); })) {

    // @note Could avoid this copy by using a custom split view
    std::istringstream iss(std::string(line.begin(), line.end()));
    iss.ignore(max_size, ' ');

    // Extract game ID
    int gameId;
    iss >> gameId;

    iss.ignore(1, ' ');

    bool possible = true;

    while (!iss.eof()) {
      int current_red = 0, current_green = 0, current_blue = 0;

      int count;
      std::string color;

      while (iss >> count >> color) {
        if (color.starts_with("red"))
          current_red += count;
        if (color.starts_with("blue"))
          current_blue += count;
        if (color.starts_with("green"))
          current_green += count;

        if (color[color.size() - 1] != ',')
          break;
      }

      if (current_blue > 14 || current_green > 13 || current_red > 12) {
        possible = false;
        break;
      }
    }

    if (possible)
      sum += gameId;
  }
  return sum;
}

int part_two(string_view sv) {
  int sum = 0;

  for (auto line :
       sv | ranges::views::split('\n') |
           ranges::views::filter([](auto &&s) { return !s.empty(); })) {

    std::string data(line.begin(), line.end());
    std::istringstream iss(data);
    iss.ignore(max_size, ' ');

    // Extract game ID
    int gameId;
    iss >> gameId;

    iss.ignore(1, ' ');

    int max_red = 0, max_green = 0, max_blue = 0;

    while (!iss.eof()) {
      int current_red = 0, current_green = 0, current_blue = 0;

      int count;
      std::string color;

      while (iss >> count >> color) {
        if (color.starts_with("red"))
          current_red += count;
        if (color.starts_with("blue"))
          current_blue += count;
        if (color.starts_with("green"))
          current_green += count;

        if (color[color.size() - 1] != ',')
          break;
      }

      max_red = max(max_red, current_red);
      max_blue = max(max_blue, current_blue);
      max_green = max(max_green, current_green);
    }

    sum += max_red * max_green * max_blue;
  }
  return sum;
}
} // namespace Day02
