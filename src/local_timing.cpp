#include "blackbird/local_timing.hpp"
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
    // Strings are native labels or existing hexadecimal identities, not content.
    // Escape linkage and labels to keep the primitive safe for any caller.
    const auto escaped = [](const char *s) {
      std::string out;
      for (; *s; ++s) {
        const auto c = static_cast<unsigned char>(*s);
        if (c < 32 || c == '"' || c == '\\') {
          char escape[7];
          std::snprintf(escape, sizeof escape, "\\u%04x", c);
          out += escape;
        } else
          out += *s;
      }
      return out;
    };
    char line[2048];
    for (std::size_t i = 0; i < size_ && ok; ++i) {
      const auto &m = records_[i];
      const auto action = escaped(m.action), source = escaped(m.source),
                 outcome = escaped(m.outcome), revision = escaped(m.revision.data()),
                 attempt = escaped(m.attempt.data());
      const int n = std::snprintf(
          line, sizeof line,
          "{\"type\":\"span\",\"action\":\"%s\",\"source\":\"%s\",\"outcome\":\"%s\","
          "\"wall_ns\":%llu,\"thread_cpu_ns\":%s,\"clock_valid\":%s,\"history_"
          "records\":%llu,\"observed_bytes\":%llu,\"sequence\":%llu,\"revision\":\"%"
          "s\",\"attempt\":\"%s\",\"linkage_truncated\":%s}\n",
          action.c_str(), source.c_str(), outcome.c_str(),
          static_cast<unsigned long long>(m.wall_ns),
          m.cpu_measured ? std::to_string(m.cpu_ns).c_str() : "null",
          m.clock_valid ? "true" : "false", static_cast<unsigned long long>(m.history),
          static_cast<unsigned long long>(m.bytes),
          static_cast<unsigned long long>(m.sequence), revision.c_str(),
          attempt.c_str(), m.linkage_truncated ? "true" : "false");
      ok = n > 0 && static_cast<std::size_t>(n) < sizeof line &&
           write_all(line, static_cast<std::size_t>(n));
    }
    const int n = std::snprintf(
        line, sizeof line,
        "{\"type\":\"final_status\",\"buffered\":%zu,\"dropped\":%llu,\"write_ok\":%s,"
        "\"clock\":\"CLOCK_MONOTONIC/CLOCK_THREAD_CPUTIME_ID\",\"durable\":false}\n",
        size_, static_cast<unsigned long long>(dropped_), ok ? "true" : "false");
    if (n > 0)
      ok = write_all(line, static_cast<std::size_t>(n)) && ok;
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
