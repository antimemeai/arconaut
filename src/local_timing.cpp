#include "blackbird/local_timing.hpp"
#include "blackbird/packet.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <new>
#include <string>

#include <time.h>
#include <unistd.h>
namespace blackbird {
thread_local LocalTimingSink *local_timing_sink = nullptr;
LocalTick local_tick(bool cpu) noexcept {
  timespec wall{}, thread{};
  const bool valid = clock_gettime(CLOCK_MONOTONIC, &wall) == 0 &&
                     (!cpu || clock_gettime(CLOCK_THREAD_CPUTIME_ID, &thread) == 0);
  const auto ns = [](timespec t) {
    return static_cast<std::uint64_t>(t.tv_sec) * 1000000000ULL +
           static_cast<std::uint64_t>(t.tv_nsec);
  };
  return {ns(wall), ns(thread), valid};
}
void LocalTimingSink::push(const LocalMetric &m) noexcept {
  const std::lock_guard lock{mutex_};
  if (size_ == capacity) {
    ++dropped_;
    return;
  }
  records_[size_++] = m;
}
bool LocalTimingSink::persist(std::string_view path) noexcept {
  // Only shutdown does formatting/writes. Never overwrite another run.
  try {
    const std::string name{path};
    const int fd = open(name.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    if (fd < 0)
      return false;
    struct Descriptor {
      int value;
      ~Descriptor() {
        if (value >= 0)
          (void)close(value);
      }
    } owner{fd};
    bool ok = true;
    const auto write_all = [&](const char *data, std::size_t size) {
      while (size) {
        const auto n = write(fd, data, size);
        if (n < 0 && errno == EINTR)
          continue;
        if (n <= 0)
          return false;
        data += n;
        size -= static_cast<std::size_t>(n);
      }
      return true;
    };
    const auto write_packet = [&](const Value &value) {
      auto encoded = encode_packet_string(value);
      if (!encoded.has_value())
        throw encoded.error();
      const auto packet = std::move(encoded).value();
      std::array<char, 8> length{};
      for (unsigned i = 0; i < 8; ++i)
        length[i] = static_cast<char>(
            (static_cast<std::uint64_t>(packet.size()) >> (i * 8)) & 255U);
      return write_all(length.data(), length.size()) &&
             write_all(packet.data(), packet.size());
    };
    ok = write_all("BBMS\1", 5);
    for (std::size_t i = 0; i < size_ && ok; ++i) {
      const auto &m = records_[i];
      ok = write_packet(Value::object(
          {{"type", Value{"span"}},
           {"action", Value{m.action}},
           {"source", Value{m.source}},
           {"outcome", Value{m.outcome}},
           {"start_wall_ns", Value{Number{m.start_wall_ns}}},
           {"wall_ns", Value{Number{m.wall_ns}}},
           {"thread_cpu_ns", m.cpu_measured ? Value{Number{m.cpu_ns}} : Value{}},
           {"clock_valid", Value{m.clock_valid}},
           {"history_records", Value{Number{m.history}}},
           {"observed_bytes", Value{Number{m.bytes}}},
           {"sequence", Value{Number{m.sequence}}},
           {"revision", Value{m.revision.data()}},
           {"attempt", Value{m.attempt.data()}},
           {"linkage_truncated", Value{m.linkage_truncated}}}));
    }
    ok = write_packet(
             Value::object({{"type", Value{"final_status"}},
                            {"buffered", Value{Number{size_}}},
                            {"dropped", Value{Number{dropped_}}},
                            {"write_ok", Value{ok}},
                            {"clock", Value{"CLOCK_MONOTONIC/CLOCK_THREAD_CPUTIME_ID"}},
                            {"durable", Value{false}}})) &&
         ok;
    if (close(fd) != 0)
      ok = false;
    owner.value = -1;
    return ok;
  } catch (...) {
    return false;
  }
}
// Labels/gauges follow a fixed documented order; distinct wrappers add no safety here.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
LocalSpan::LocalSpan(const char *action, const char *source, bool cpu) noexcept
    : sink_(local_timing_sink) {
  if (!sink_)
    return;
  metric_.action = action;
  metric_.source = source;
  metric_.cpu_measured = cpu;
  start_ = sink_->clock()(cpu);
  metric_.start_wall_ns = start_.wall;
}
// Labels/gauges follow a fixed documented order; distinct wrappers add no safety here.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void LocalSpan::observe(std::uint64_t history, std::uint64_t bytes,
                        std::uint64_t sequence) noexcept {
  if (!sink_)
    return;
  metric_.history = history;
  metric_.bytes = bytes;
  metric_.sequence = sequence;
}
// Labels/gauges follow a fixed documented order; distinct wrappers add no safety here.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void LocalSpan::linkage(std::string_view revision, std::string_view attempt) noexcept {
  if (!sink_)
    return;
  const auto copy = [&](auto &to, std::string_view from) {
    const auto n = std::min(from.size(), to.size() - 1);
    std::memcpy(to.data(), from.data(), n);
    to[n] = 0;
    metric_.linkage_truncated |= n != from.size();
  };
  copy(metric_.revision, revision);
  copy(metric_.attempt, attempt);
}
void LocalSpan::finish() noexcept {
  if (!sink_)
    return;
  const auto end = sink_->clock()(metric_.cpu_measured);
  metric_.clock_valid =
      start_.valid && end.valid && end.wall >= start_.wall && end.cpu >= start_.cpu;
  if (metric_.clock_valid) {
    metric_.wall_ns = end.wall - start_.wall;
    metric_.cpu_ns = end.cpu - start_.cpu;
  }
  sink_->push(metric_);
  sink_ = nullptr;
}
LocalTimingSession::LocalTimingSession() noexcept
    : path_(std::getenv("BLACKBIRD_LOCAL_TIMING")) {
  if (!path_ || !*path_ || local_timing_sink)
    return;
  sink_.reset(new (std::nothrow) LocalTimingSink);
  if (sink_)
    local_timing_sink = sink_.get();
  else
    std::fputs("local timing: allocation failed\n", stderr);
}
LocalTimingSession::~LocalTimingSession() {
  if (!sink_)
    return;
  local_timing_sink = nullptr;
  if (!sink_->persist(path_))
    std::fputs("local timing: persistence failed (not audit failure)\n", stderr);
}
} // namespace blackbird
