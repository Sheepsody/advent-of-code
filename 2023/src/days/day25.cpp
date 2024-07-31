#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

namespace Day25 {
using namespace std;

static default_random_engine engine(3);

// Union find DS
// Partitions data in a set of disjoint subsets
struct UnionFind {
  int nbSubsets;
  vector<int> parent;
  vector<int> rank;

  UnionFind(int n) : nbSubsets(n) {
    rank = vector<int>(n, 1);
    parent = vector<int>(n, 0);
    for (int i = 0; i < n; i++)
      parent[i] = i;
  }

  int find(int x) {
    if (x != parent[x])
      parent[x] = find(parent[x]); // Path compression
    return parent[x];
  }

  bool is_connected(int x, int y) { return find(x) == find(y); };

  void merge(int x, int y) {
    const auto i = find(x), j = find(y);
    if (i == j)
      return;

    if (rank[i] < rank[j]) {
      parent[i] = j;
      rank[j] += rank[i];
    } else {
      parent[j] = i;
      rank[i] += rank[j];
    }

    nbSubsets--;
  }
};

struct GraphCut {
  int cutSize;
  UnionFind uf;

  pair<vector<int>, vector<int>> getPartitions() {
    vector<int> p, q;
    p.reserve(uf.rank[0]);
    q.reserve(uf.parent.size() - p.size());

    for (int i = 0; i < uf.parent.size(); i++) {
      if (uf.is_connected(0, i))
        p.push_back(i);
      else
        q.push_back(i);
    }

    return {p, q};
  }
};

// Krager algorithm
// Finds minimal cut with high-probability
GraphCut kraker(int nbVertices, vector<pair<int, int>> &edges) {
  UnionFind uf(nbVertices);
  auto start = edges.begin();

  for (int m = edges.size() - 1; uf.nbSubsets != 2; ++start, --m) {
    // Swap first edge with random un-used edge
    iter_swap(start, start + std::uniform_int_distribution<int>(0, m)(engine));
    uf.merge(start->first, start->second);
  }

  int cutSize = static_cast<int>(count_if(start, edges.end(), [&](auto e) {
    return !uf.is_connected(e.first, e.second);
  }));

  return GraphCut{cutSize, std::move(uf)};
}

int getVerticeIndex(unordered_map<string, int> &vertices, const string &s) {
  if (vertices.contains(s))
    return vertices[s];

  return vertices[s] = vertices.size();
}

using Input = pair<vector<pair<int, int>>, unordered_map<string, int>>;

Input parse(const string_view &content) {
  std::regex pattern("(\\w+)");
  vector<pair<int, int>> edges;
  unordered_map<string, int> vertices;

  for (auto line : content | std::views::split('\n') |
                       std::views::filter([](auto x) { return !x.empty(); })) {
    string sline(line.begin(), line.end());
    auto words_begin =
        std::sregex_token_iterator(sline.begin(), sline.end(), pattern);
    auto words_end = std::sregex_token_iterator();

    // Collect source
    string source = *words_begin;
    words_begin++;

    // Add edges
    for (auto it = words_begin; it != words_end; ++it)
      edges.push_back(
          {getVerticeIndex(vertices, source), getVerticeIndex(vertices, *it)});
  }

  return make_tuple(edges, vertices);
}

auto part_one(const Input &input) {
  auto [edges, vertices] = input;
  int i;
  GraphCut cut{INT_MAX, UnionFind(0)};
  for (i = 0; cut.cutSize > 3; i++)
    cut = kraker(vertices.size(), edges);

  auto partitions = cut.getPartitions();

  return partitions.first.size() * partitions.second.size();
}

auto part_two(const Input &_) { return 0; }
} // namespace Day25
