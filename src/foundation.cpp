#include "blackbird/foundation.hpp"

#include <chrono>
#include <ratio>
#include <type_traits>

namespace blackbird {

const char *error_name(ErrorCode code) noexcept {
  switch (code) {
  case ErrorCode::invalid_identity:
    return "invalid_identity";
  case ErrorCode::wrong_environment:
    return "wrong_environment";
  case ErrorCode::wrong_incarnation:
    return "wrong_incarnation";
  case ErrorCode::wrong_registry:
    return "wrong_registry";
  case ErrorCode::stale_registry:
    return "stale_registry";
  case ErrorCode::stale_handle:
    return "stale_handle";
  case ErrorCode::capacity:
    return "capacity";
  case ErrorCode::allocation:
    return "allocation";
  case ErrorCode::invalid_range:
    return "invalid_range";
  case ErrorCode::overflow:
    return "overflow";
  case ErrorCode::wrong_clock:
    return "wrong_clock";
  case ErrorCode::clock_regressed:
    return "clock_regressed";
  case ErrorCode::unsupported:
    return "unsupported";
  case ErrorCode::interrupted:
    return "interrupted";
  case ErrorCode::io:
    return "io";
  case ErrorCode::provider_transport:
    return "provider_transport";
  case ErrorCode::external_unknown:
    return "external_unknown";
  case ErrorCode::incomplete:
    return "incomplete";
  case ErrorCode::corrupt:
    return "corrupt";
  case ErrorCode::conflict:
    return "conflict";
  case ErrorCode::busy:
    return "busy";
  case ErrorCode::audit_unavailable:
    return "audit_unavailable";
  }
  return "unknown_error";
}

Result<AuditSequence> AuditSequence::from(std::uint64_t value) {
  if (value == 0) {
    return Result<AuditSequence>::failure({ErrorCode::invalid_identity});
  }
  return Result<AuditSequence>::success(AuditSequence{value});
}
Result<AuditSequence> AuditSequence::next() const {
  if (value_ == UINT64_MAX) {
    return Result<AuditSequence>::failure({ErrorCode::overflow});
  }
  return from(value_ + 1);
}

Result<ByteView> slice(ByteView bytes, std::size_t offset, std::size_t length) {
  if (offset > bytes.size() || length > bytes.size() - offset) {
    return Result<ByteView>::failure({ErrorCode::invalid_range});
  }
  return Result<ByteView>::success(bytes.subspan(offset, length));
}
Result<ByteBuffer> ByteBuffer::copy(ByteView bytes, std::size_t limit) {
  std::vector<std::byte> data;
  if (bytes.size() > limit || bytes.size() > data.max_size()) {
    return Result<ByteBuffer>::failure({ErrorCode::capacity});
  }
  try {
    data.assign(bytes.begin(), bytes.end());
    return Result<ByteBuffer>::success(ByteBuffer{std::move(data)});
  } catch (const std::bad_alloc &) {
    return Result<ByteBuffer>::failure({ErrorCode::allocation});
  }
}

Result<Duration> elapsed(const MonotonicInstant &earlier,
                         const MonotonicInstant &later) {
  if (earlier.domain != later.domain) {
    return Result<Duration>::failure({ErrorCode::wrong_clock});
  }
  if (later.nanoseconds < earlier.nanoseconds) {
    return Result<Duration>::failure({ErrorCode::clock_regressed});
  }
  // Unsigned conversion/subtraction is defined modulo 2^64, including full signed
  // range.
  return Result<Duration>::success({static_cast<std::uint64_t>(later.nanoseconds) -
                                    static_cast<std::uint64_t>(earlier.nanoseconds)});
}
Result<Deadline> Deadline::after(MonotonicInstant start, Duration duration) {
  const auto available = static_cast<std::uint64_t>(INT64_MAX) -
                         static_cast<std::uint64_t>(start.nanoseconds);
  if (duration.nanoseconds > available) {
    return Result<Deadline>::failure({ErrorCode::overflow});
  }
  std::int64_t due = 0;
  if (start.nanoseconds >= 0) {
    due = start.nanoseconds + static_cast<std::int64_t>(duration.nanoseconds);
  } else {
    const auto distance_to_zero =
        static_cast<std::uint64_t>(-(start.nanoseconds + 1)) + 1;
    if (duration.nanoseconds >= distance_to_zero) {
      due = static_cast<std::int64_t>(duration.nanoseconds - distance_to_zero);
    } else {
      due = start.nanoseconds + static_cast<std::int64_t>(duration.nanoseconds);
    }
  }
  return Result<Deadline>::success(Deadline{start, due});
}
Result<bool> Deadline::due(const MonotonicInstant &now) const {
  if (now.domain != start_.domain) {
    return Result<bool>::failure({ErrorCode::wrong_clock});
  }
  if (now.nanoseconds < start_.nanoseconds) {
    return Result<bool>::failure({ErrorCode::clock_regressed});
  }
  return Result<bool>::success(now.nanoseconds >= due_.nanoseconds);
}

Result<ClockSample> ClockSample::from(WallObservation wall,
                                      MonotonicInstant monotonic) {
  if (wall.domain != monotonic.domain) {
    return Result<ClockSample>::failure({ErrorCode::wrong_clock});
  }
  return Result<ClockSample>::success(ClockSample{wall, monotonic});
}

namespace {
template <typename NativeDuration>
Result<std::int64_t> native_nanoseconds(NativeDuration duration) {
  using Rep = typename NativeDuration::rep;
  using Scale = std::ratio_divide<typename NativeDuration::period, std::nano>;
  static_assert(std::is_integral_v<Rep> && std::is_signed_v<Rep>);
  static_assert(sizeof(Rep) <= sizeof(std::int64_t));
  static_assert(Scale::den == 1 && Scale::num > 0);
  const auto count = static_cast<std::int64_t>(duration.count());
  if (count > INT64_MAX / Scale::num || count < INT64_MIN / Scale::num) {
    return Result<std::int64_t>::failure({ErrorCode::overflow});
  }
  return Result<std::int64_t>::success(count * Scale::num);
}
} // namespace

Result<ClockSample> NativeClock::observe() {
  static_assert(std::chrono::steady_clock::is_steady);
  const auto wall =
      native_nanoseconds(std::chrono::system_clock::now().time_since_epoch());
  if (!wall.has_value()) {
    return Result<ClockSample>::failure(wall.error());
  }
  const auto monotonic =
      native_nanoseconds(std::chrono::steady_clock::now().time_since_epoch());
  if (!monotonic.has_value()) {
    return Result<ClockSample>::failure(monotonic.error());
  }
  return ClockSample::from({domain_, wall.value()}, {domain_, monotonic.value()});
}

} // namespace blackbird
