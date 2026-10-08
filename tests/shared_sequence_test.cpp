#include "blackbird/shared_sequence.hpp"
#include <iostream>
#include <stdexcept>
using namespace blackbird;
struct Counted {
  static inline unsigned copies = 0;
  unsigned n;
  explicit Counted(unsigned value) : n(value) {}
  Counted(const Counted &o) : n(o.n) { ++copies; }
  Counted(Counted &&) = default;
  Counted &operator=(const Counted &) = default;
  Counted &operator=(Counted &&) = default;
};
int main() {
  try {
    SharedSequence<Counted> values;
    for (unsigned i = 0; i < 33000; ++i)
      values.push_back(Counted{i});
    Counted::copies = 0;
    const auto pinned = values;
    if (Counted::copies)
      throw std::runtime_error("snapshot copied records");
    values.push_back(Counted{33000});
    values[0].n = 99;
    if (Counted::copies > 64 || pinned[0].n != 0 || pinned.size() != 33000 ||
        pinned[32999].n != 32999)
      throw std::runtime_error("path copy");
    const auto copied = Counted::copies;
    for (unsigned i = 0; i < 33000; ++i)
      if (pinned[i].n != i)
        throw std::runtime_error("pinned contents");
    values.clear();
    if (pinned[32768].n != 32768)
      throw std::runtime_error("released owner");
    auto fork = pinned;
    fork.pop_back();
    fork.push_back(Counted{100});
    if (fork.back().n != 100 || pinned.back().n != 32999)
      throw std::runtime_error("fork append");
    std::cout
        << "33000 records: snapshot copied 0; append plus changed first leaf copied "
        << copied << " records\n";
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
