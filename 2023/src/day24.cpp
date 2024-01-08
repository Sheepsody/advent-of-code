#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

struct Hail {
  double px, py, pz;
  double vx, vy, vz;
};

struct Intersection {
  double x;
  double y;
};

using Matrix = vector<vector<double>>;

/*
 * Parse input with regex
 * Input in the form : 19, 13, 30 @ -2,  1, -2
 */
vector<Hail> parse(basic_ifstream<char> &stream) {
  vector<Hail> hails;
  std::regex pattern(
      "([-]*\\d+),\\s+([-]*\\d+),\\s+([-]*\\d+)\\s+@\\s+([-]*\\d+),\\s+"
      "([-]*\\d+),\\s+([-]*\\d+)");
  std::smatch matches;

  for (string line; getline(stream, line);) {
    if (std::regex_search(line, matches, pattern)) {
      vector<double> coords;
      transform(matches.begin() + 1, matches.end(), back_inserter(coords),
                [](auto &&i) { return stod(i.str()); });

      hails.push_back(Hail{
          .px = coords[0],
          .py = coords[1],
          .pz = coords[2],
          .vx = coords[3],
          .vy = coords[4],
          .vz = coords[5],
      });
    }
  };

  return hails;
}

pair<long, long> getLims(basic_ifstream<char> &stream) {
  long pMin, pMax;
  stream >> pMin >> pMax;
  return {pMin, pMax};
}

inline double getC(const Hail &h) { return h.px * h.vy - h.py * h.vx; }

/*
** Intersect the two paths
**
** 1. Write line as line, i.e.
**    $vy * x - vx * y = vy * x0 - vx * y0$
** 2. Linear Algebra !
*/
optional<Intersection> getIntersection(const Hail &a, const Hail &b) {

  double denom = a.vx * b.vy - b.vx * a.vy;
  if (denom == 0)
    return {};

  double ca = getC(a), cb = getC(b);

  double x = (a.vx * cb - b.vx * ca) / denom;
  double y = (a.vy * cb - b.vy * ca) / denom;

  return Intersection{x, y};
}

/*
 * Gaussian Elimination : Algorithm to solve linear system of equations
 * Needed a refresher : https://brilliant.org/wiki/gaussian-elimination/
 */
optional<vector<double>> gaussianElimination(Matrix &m) {
  int nRows = m.size();
  if (m[0].size() != nRows + 1) // Is augmented matrix ?
    return {};

  for (int i = 0; i < nRows; i++) {
    // Search for maximum in this column
    double maxEl = abs(m[i][i]);
    int maxRow = i;
    for (int k = i + 1; k < nRows; k++) {
      if (abs(m[k][i]) > maxEl) {
        maxEl = abs(m[k][i]);
        maxRow = k;
      }
    }

    // Swap maximum row with current row (column by column)
    for (int k = i; k < nRows + 1; k++)
      swap(m[maxRow][k], m[i][k]);

    // Make all rows below this one 0 in current column
    for (int k = i + 1; k < nRows; k++) {
      double c = -m[k][i] / m[i][i];
      for (int j = i; j < nRows + 1; j++) {
        m[k][j] += c * m[i][j];
      }
    }
  }

  // Solve equation for an upper triangular matrix A
  vector<double> x(nRows);
  for (int i = nRows - 1; i >= 0; i--) {
    x[i] = m[i][nRows] / m[i][i];
    for (int k = i - 1; k >= 0; k--) {
      m[k][nRows] -= m[k][i] * x[i];
    }
  }

  return x;
}

/*
 * Get the position of a rock that would collide all hails
 *
 * Steps (pxR, vxR, etc. as rock and dx, vx for _given_ hail) :
 * 1. Match positions : $vx * t + dx = vxR * t + dxR$
 * 2. Express t : $t = - (pxR - px) / (vxR - vx) = - (pyR - py) / (vyR -
 * vy)...$
 * 3. Cross and develop
 *    - $yR*dxR - xR*dyR = yR*dx' - y'*dx' + y'*dxR - xR*dy' + x'*dy' -
 * x'*dyR$
 *    - Right side is constant for two different hails
 * 4. Match and solve !
 */
optional<Hail> getRock(const vector<Hail> &hails) {
  Matrix augMat;

  // Eqs for x and y axis
  for (size_t i = 0; i < 4; ++i) {
    const auto &h1 = hails[i];
    const auto &h2 = hails[i + 1];

    vector<double> equation = {h2.vy - h1.vy,
                               h1.vx - h2.vx,
                               0,
                               h1.py - h2.py,
                               h2.px - h1.px,
                               0,
                               h1.py * h1.vx - h1.px * h1.vy - h2.py * h2.vx +
                                   h2.px * h2.vy};

    augMat.push_back(equation);
  }

  for (size_t i = 0; i < 2; ++i) {
    const auto &h1 = hails[i];
    const auto &h2 = hails[i + 1];

    vector<double> equation = {h2.vz - h1.vz,
                               0,
                               h1.vx - h2.vx,
                               h1.pz - h2.pz,
                               0,
                               h2.px - h1.px,
                               h1.pz * h1.vx - h1.px * h1.vz - h2.pz * h2.vx +
                                   h2.px * h2.vz};

    augMat.push_back(equation);
  }

  auto s = gaussianElimination(augMat);

  if (!s.has_value())
    return {};

  auto coords = s.value();
  return Hail{
      .px = coords[0],
      .py = coords[1],
      .pz = coords[2],
      .vx = coords[3],
      .vy = coords[4],
      .vz = coords[5],
  };
}

double timeToReach(const Hail &a, const Intersection &i) {
  return (i.x - a.px) / a.vx;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "Missing argument file" << endl;
    throw;
  }

  ifstream inputFile(argv[1]);

  // Check if the file is open
  if (!inputFile.is_open()) {
    cerr << "Error opening the file!" << endl;
    return 1; // Return an error code
  }

  // Get limits
  auto [pMin, pMax] = getLims(inputFile);

  // Parse input
  auto hails = parse(inputFile);

  int count = 0;

  // Create graph
  for (int i = 0; i < hails.size(); i++)
    for (int j = 0; j < i; j++) {
      auto r = getIntersection(hails[i], hails[j]);

      if (!r.has_value())
        continue;

      if (timeToReach(hails[i], r.value()) < 0 ||
          timeToReach(hails[j], r.value()) < 0)
        continue;

      if (r.value().x < pMin || r.value().x > pMax)
        continue;

      if (r.value().y < pMin || r.value().y > pMax)
        continue;

      count++;
    }

  cout << "Part a: " << count << endl;

  auto r = getRock(hails).value();

  // Had issue with precision => rounded up...
  cout << "Part b: " << (long)roundl(r.px + r.py + r.pz) << endl;

  // Close file
  inputFile.close();

  return 0;
}
