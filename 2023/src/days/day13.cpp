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

using namespace std;

namespace Day13 {

using Input = vector<vector<vector<int>>>;

// Counts the number of ones (bit)
int countOnes(int value) {
  int count = 0;
  while (value > 0) {
    count += value & 1;
    value >>= 1;
  }
  return count;
}

// Find index of the symmetry of a matrix
int getSimIndex(const vector<int> &vec, int nb_smudge) {
  int length = vec.size();

  for (int i = 1; i < length; i++) {
    int dist = min(i, length - i);

    int mismatch = 0;
    for (int j = 0; j < dist; j++)
      if (vec[i - j - 1] != vec[i + j])
        mismatch += countOnes(vec[i - j - 1] ^ vec[i + j]);

    if (mismatch == nb_smudge)
      return i;
  }

  return 0;
}

// Get score matrix
int getScore(const vector<vector<int>> &matrix, int nb_smudge) {
  int height = matrix.size();
  int width = matrix[0].size();
  int result = 0;

  // Create columns
  vector<int> cols(width);
  for (int j = 0; j < width; j++)
    cols[j] =
        accumulate(matrix.begin(), matrix.end(), 0,
                   [j](int c, vector<int> row) { return (c << 1) + row[j]; });

  result += getSimIndex(cols, nb_smudge);

  // Create rows
  vector<int> rows;
  transform(
      matrix.begin(), matrix.end(), back_inserter(rows), [](const auto &row) {
        return accumulate(row.begin(), row.end(), 0,
                          [](int acc, int elem) { return (acc << 1) + elem; });
      });

  result += getSimIndex(rows, nb_smudge) * 100;

  return result;
}

Input parse(const string_view &content) {
  vector<vector<vector<int>>> list_matrix;
  vector<vector<int>> matrix;
  for (auto s : content | std::views::split('\n')) {
    auto line = string(string(s.begin(), s.end()));
    if (line.size() > 0) {
      // Parse rows of new matrix
      vector<int> row;
      row.reserve(line.size());

      istringstream iss(line);
      transform(istream_iterator<char>(iss), istream_iterator<char>(),
                back_inserter(row), [](int c) { return (c == '#') ? 1 : 0; });

      matrix.push_back(std::move(row));
    } else {
      // Append matrix
      if (matrix.size() > 0)
        list_matrix.push_back(matrix);
      matrix.clear();
    }
  }
  if (matrix.size() > 0)
    list_matrix.push_back(matrix);

  return list_matrix;
}

int part_one(const Input& list_matrix ){
  return accumulate(list_matrix.begin(), list_matrix.end(), 0,
             [](int acc, auto m) { return acc + getScore(m, 0); });
}

int part_two(const Input& list_matrix) {
  return accumulate(list_matrix.begin(), list_matrix.end(), 0,
             [](int acc, auto m) { return acc + getScore(m, 1); });
}
} // namespace Day13
