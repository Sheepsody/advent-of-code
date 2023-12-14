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
  vector<vector<int>> records;
  for (string line; getline(inputFile, line);)
    if (line.size() > 0) {
      vector<int> new_record;
      stringstream stream(line);
      for (int num; stream >> num;)
        new_record.push_back(num);
      records.push_back(std::move(new_record));
    }

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

  cout << "Part a: " << sum << endl;

  // Part B
  sum = 0;
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

  cout << "Part b: " << sum << endl;

  // Close file
  inputFile.close();

  return 0;
}
