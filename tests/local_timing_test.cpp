#include "blackbird/local_timing.hpp"
#include <cstdlib>
#include <filesystem>
#include <new>
#include <stdexcept>
#include <unistd.h>
static std::size_t allocations;
void *operator new(std::size_t n) {
  ++allocations;
  if (void *p = std::malloc(n ? n : 1))
    return p;
  throw std::bad_alloc{};
}
void operator delete(void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }
using namespace blackbird;
static unsigned calls;
static LocalTick invalid_tick(bool) noexcept { return {0, 0, false}; }
static LocalTick tick(bool cpu) noexcept {
  ++calls;
  return {calls * 100ULL, cpu ? calls * 30ULL : 0, true};
}
static void check(bool b) {
  if (!b)
    throw std::runtime_error("timing oracle");
}
int main() {
  auto sink = std::make_unique<LocalTimingSink>(tick);
  const auto before_allocations = allocations;
  {
    LocalSpan s{"disabled", "test"};
    s.observe(1, 2);
    s.linkage("r");
  }
  check(calls == 0 && sink->size() == 0 && allocations == before_allocations);
  local_timing_sink = sink.get();
  {
    LocalSpan s{"local", "test"};
    s.observe(7, 19, 3);
    s.linkage("rev", "attempt");
    s.outcome("success");
    s.finish();
  }
  check(calls == 2 && sink->at(0).wall_ns == 100 && sink->at(0).cpu_ns == 30 &&
        sink->at(0).history == 7 && sink->at(0).bytes == 19);
  {
    LocalSpan s{"transport", "test", false};
    s.outcome("returned");
  }
  check(calls == 4 && !sink->at(1).cpu_measured && sink->at(1).cpu_ns == 0);
  for (std::size_t i = 0; i < LocalTimingSink::capacity; ++i) {
    LocalSpan s{"overflow", "test"};
  }
  check(sink->size() == LocalTimingSink::capacity && sink->dropped() == 2);
  auto invalid = std::make_unique<LocalTimingSink>(invalid_tick);
  local_timing_sink = invalid.get();
  {
    LocalSpan s{"invalid", "test"};
  }
  check(!invalid->at(0).clock_valid && invalid->at(0).wall_ns == 0);
  local_timing_sink = nullptr;
  const auto path = "/tmp/p1-timing-oracle-" + std::to_string(getpid());
  check(!std::filesystem::exists(path));
  check(sink->persist(path));
  check(!sink->persist(path));
  check(!sink->persist("/nonexistent-p1-directory/output"));
  std::filesystem::remove(path);
}
