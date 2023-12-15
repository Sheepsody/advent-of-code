#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_set>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

using Point = pair<int, int>;

int calculateDistance(Point a, Point b, vector<int> rows, vector<int> cols,
                      int expansionFactor) {
  int distance = 0;

  for (int i = min(a.first, b.first); i < max(a.first, b.first); ++i)
    distance += rows[i] * expansionFactor + 1;

  for (int i = min(a.second, b.second); i < max(a.second, b.second); ++i)
    distance += cols[i] * expansionFactor + 1;

  return distance;
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

  // Parse
  vector<string> lines;
  for (string line; getline(inputFile, line);)
    if (line.size() > 0)
      lines.push_back(line);

  // Parse grid
  vector<vector<int>> matrix(lines.size(), vector<int>(lines[0].size()));
  for (int i = 0; i < matrix.size(); i++)
    for (int j = 0; j < matrix[0].size(); j++)
      if (lines[i][j] == '#')
        matrix[i][j] = 1;

  // Dimension
  int height = matrix.size();
  int width = matrix[0].size(); // All rows have the same size

  // Add rows
  vector<int> rowsExpansion(height, 0);
  for (int i = 0; i < height; i++)
    // Check if the entire row is zero
    if (all_of(matrix[i].begin(), matrix[i].end(),
               [](int x) { return x == 0; }))
      // Insert a new row with zeros
      rowsExpansion[i] = 1;

  // Add columns
  vector<int> colsExpansion(width, 0);
  for (int j = 0; j < width; j++)
    // Check if the entire column is zero
    if (all_of(matrix.begin(), matrix.end(),
               [j](const vector<int> &row) { return row[j] == 0; }))
      // Insert a new column with zeros
      colsExpansion[j] = 1;

  // Print
  vector<Point> positions;
  for (int i = 0; i < matrix.size(); i++)
    for (int j = 0; j < matrix[0].size(); j++)
      if (matrix[i][j] == 1)
        positions.push_back(make_pair(i, j));

  int distance = 0;
  for (int i = 0; i < positions.size(); ++i)
    for (int j = i + 1; j < positions.size(); ++j)
      distance += calculateDistance(positions[i], positions[j], rowsExpansion,
                                    colsExpansion, 1);

  cout << "Part a: " << distance << endl;

  long distanceB = 0;
  for (int i = 0; i < positions.size(); ++i)
    for (int j = i + 1; j < positions.size(); ++j)
      distanceB += calculateDistance(positions[i], positions[j], rowsExpansion,
                                     colsExpansion, 999999);

  cout << "Part b: " << distanceB << endl;

  // Close file
  inputFile.close();

  return 0;
}
