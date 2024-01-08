#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

using namespace std;
constexpr auto max_discard = numeric_limits<streamsize>::max();

struct Pulse {
  string src;
  string dst;
  bool high;

  /*
  ** Debug print
  */
  friend std::ostream &operator<<(std::ostream &os, const Pulse &p) {
    os << p.src << ((p.high) ? " -high-> " : " -low-> ") << p.dst;
    return os;
  };
};

struct Pulses {
  int countLow = 0, countHigh = 0;
  queue<Pulse> pq;
};

struct Module {
  string src;
  vector<string> dests;

  // Abstact base class
  virtual ~Module() {}
  virtual vector<Pulse> process(Pulse &p) = 0;
};

using Modules = unordered_map<string, unique_ptr<Module>>;

/*
** Broadcast module
** Sends the same pulse to all of its destination modules.
*/
struct Broadcaster : public Module {
  vector<Pulse> process(Pulse &p) override {
    vector<Pulse> outputs{};
    for (const auto &dst : this->dests)
      outputs.push_back({
          .src = this->src,
          .dst = dst,
          .high = p.high,
      });
    return outputs;
  }
};

/*
** Flip-flop (%)
** High pulse => ignored
** Low pulse => toggle and send new pulse
*/
struct FlipFlop : public Module {
  bool on = false;

  vector<Pulse> process(Pulse &p) override {
    if (p.high)
      return {};

    // Flip
    this->on = !this->on;

    vector<Pulse> outputs{};
    for (const auto &dst : this->dests)
      outputs.push_back({
          .src = this->src,
          .dst = dst,
          .high = this->on,
      });

    return outputs;
  }
};

/*
** Conjunction (&)
** Updates its memory for each input received first
** Outputs low pulse only if remembers high pulses for all inputs
*/
struct Conjunction : public Module {
  int countHigh = 0; // Faster
  unordered_map<string, bool> mem{};
  bool highSent = false;

  bool isHigh() const {
    return !all_of(this->mem.begin(), this->mem.end(),
                   [](const auto &e) { return e.second; });
  }

  vector<Pulse> process(Pulse &p) override {
    this->mem[p.src] = p.high; // Update

    bool statusHigh = this->isHigh();
    if (statusHigh)
      highSent = true;

    vector<Pulse> outputs{};
    for (const auto &dst : this->dests)
      outputs.push_back({
          .src = this->src,
          .dst = dst,
          .high = statusHigh,
      });
    return outputs;
  }
};

Modules parse(ifstream &input) {
  // Use smart pointers
  unordered_map<string, unique_ptr<Module>> modules;

  for (string line; std::getline(input, line);)
    if (!line.empty()) {
      istringstream stream(line);
      string moduleInfo, arrow;
      vector<string> dests;

      stream >> moduleInfo >> arrow;

      for (string dest; std::getline(stream, dest, ',');) {
        // Remove spaces
        dest.erase(remove_if(dest.begin(), dest.end(),
                             [](auto &&c) { return isspace(c); }),
                   dest.end());
        if (!dest.empty())
          dests.push_back(dest);
      }

      unique_ptr<Module> mod;
      switch (moduleInfo[0]) {
      case '%':
        mod = make_unique<FlipFlop>();
        break;
      case '&':
        mod = make_unique<Conjunction>();
        break;
      default:
        mod = make_unique<Broadcaster>();
      }

      string src = moduleInfo.substr(
          moduleInfo[0] == '%' || moduleInfo[0] == '&' ? 1 : 0);

      mod->dests = std::move(dests);
      mod->src = src;

      modules[src] = std::move(mod);
    }

  // Set conjunction modules
  // FIXME Dirty but inexpensive
  for (const auto &[kc, c] : modules)
    if (dynamic_cast<Conjunction *>(c.get())) // Is conjunction ?
      for (const auto &[k2, m] : modules)
        // Conjunction in outputs ?
        if (find(m->dests.begin(), m->dests.end(), kc) != m->dests.end())
          dynamic_cast<Conjunction *>(modules[kc].get())->mem[k2] = false;

  return modules;
}

pair<long, long> executeSequence(Modules &modules) {
  Pulses pulses;
  pulses.pq.push({
      .src = "button",
      .dst = "broadcaster",
      .high = false,
  });

  while (!pulses.pq.empty()) {
    auto p = pulses.pq.front();
    pulses.pq.pop();

    if (p.high)
      pulses.countHigh++;
    else
      pulses.countLow++;

    if (!modules.contains(p.dst))
      continue;

    auto newPulses = modules[p.dst].get()->process(p);
    for (auto &p : newPulses)
      pulses.pq.push(p);
  }

  return {pulses.countLow, pulses.countHigh};
}

long partA(ifstream &inputFile) {
  auto modules = parse(inputFile);

  long low = 0, high = 0;
  for (int i = 0; i < 1000; i++) {
    auto [l, h] = executeSequence(modules);
    low += l;
    high += h;
  }

  return low * high;
}

/*
** Part b
**
** Need all inputs of conjunction nr to be low
** => all of (lh, fk, ff, mm) have to be low
** => Find lowest common multipliers of repetition
**
** &lh -> nr
** &fk -> nr
** &ff -> nr
** &mm -> nr
** &nr -> rx
*/
long partB(ifstream &inputFile) {
  auto modules = parse(inputFile);

  // Print inputs to conjunction
  auto d = dynamic_cast<Conjunction *>(modules["nr"].get())->mem;
  unordered_map<string, long> conjTime;
  for (auto &[k, _] : d)
    conjTime[k] = -1;

  int i = 0;
  while (true) {
    i++;

    // Execute sequence
    auto _ = executeSequence(modules);

    // Update status
    for (const auto &[conj, status] : conjTime)
      if (status == -1 &&
          dynamic_cast<Conjunction *>(modules[conj].get())->highSent)
        conjTime[conj] = i;

    // Check all passed
    if (all_of(conjTime.begin(), conjTime.end(),
               [](const auto &p) { return p.second != -1; }))
      break;
  }

  // Compute LCM (Last Common Multiplier)
  long r = 1;
  for (auto &[_, v] : conjTime)
    r = lcm(r, v);

  return r;
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

  // Part a
  cout << "Part a: " << partA(inputFile) << endl;

  inputFile.clear(); // Clear any error flags
  inputFile.seekg(
      0, std::ios::beg); // Move the position to the beginning of the file

  cout << "Part a: " << partB(inputFile) << endl;

  // Close file
  inputFile.close();

  return 0;
}
