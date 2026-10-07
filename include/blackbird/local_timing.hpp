#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

namespace blackbird {
struct LocalTick {
  std::uint64_t wall{}, cpu{};
  bool valid{};
};
using LocalClock = LocalTick (*)(bool) noexcept;
LocalTick local_tick(bool cpu = true) noexcept;
struct LocalMetric {
  const char *action{}, *source{}, *outcome{"incomplete"};
  std::uint64_t wall_ns{}, cpu_ns{}, history{}, bytes{}, sequence{};
  bool cpu_measured{true}, clock_valid{}, linkage_truncated{};
  std::array<char, 96> revision{}, attempt{};
};
// Single owning thread. No file I/O in push; capacity fixed for the whole session.
class LocalTimingSink {
public:
  static constexpr std::size_t capacity = 8192;
  explicit LocalTimingSink(LocalClock clock = local_tick) noexcept : clock_(clock) {}
  void push(const LocalMetric &metric) noexcept;
  bool persist(std::string_view path) noexcept;
  LocalClock clock() const noexcept { return clock_; }
  std::size_t size() const noexcept { return size_; }
  std::uint64_t dropped() const noexcept { return dropped_; }
  const LocalMetric &at(std::size_t i) const noexcept { return records_[i]; }

private:
  LocalClock clock_;
  std::array<LocalMetric, capacity> records_{};
  std::size_t size_{};
  std::uint64_t dropped_{};
};
extern thread_local LocalTimingSink *local_timing_sink;
class LocalSpan {
public:
  LocalSpan(const char *action, const char *source, bool cpu = true) noexcept;
  ~LocalSpan() { finish(); }
  LocalSpan(const LocalSpan &) = delete;
  LocalSpan &operator=(const LocalSpan &) = delete;
  bool enabled() const noexcept { return sink_ != nullptr; }
  void observe(std::uint64_t history, std::uint64_t bytes,
               std::uint64_t sequence = 0) noexcept;
  void linkage(std::string_view revision, std::string_view attempt = {}) noexcept;
  void outcome(const char *value) noexcept {
    if (sink_)
      metric_.outcome = value;
  }
  void finish() noexcept;

private:
  LocalTimingSink *sink_{};
  LocalMetric metric_{};
  LocalTick start_{};
};
// Environment opt-in read once at process entry; explicit persistence at shutdown.
class LocalTimingSession {
public:
  LocalTimingSession() noexcept;
  ~LocalTimingSession();

private:
  std::unique_ptr<LocalTimingSink> sink_;
  const char *path_{};
};
} // namespace blackbird
