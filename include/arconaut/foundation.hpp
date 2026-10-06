#pragma once

#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <optional>
#include <span>
#include <utility>
#include <variant>
#include <vector>

namespace arconaut {

enum class ErrorCode : std::uint8_t {
  invalid_identity,
  wrong_environment,
  wrong_incarnation,
  wrong_registry,
  stale_registry,
  stale_handle,
  capacity,
  allocation,
  invalid_range,
  overflow,
  wrong_clock,
  clock_regressed,
  unsupported,
  interrupted,
  io,
  external_unknown,
  incomplete,
  corrupt,
  conflict,
  busy,
  audit_unavailable,
  provider_transport
};

struct Error {
  ErrorCode code;
  std::int64_t detail = 0;
  bool operator==(const Error &) const = default;
};
const char *error_name(ErrorCode code) noexcept;

// Accessing the wrong alternative is a programming error (bad_variant_access).
// Failure carries no allocating strings; Result does not catch business exceptions.
template <typename T> class [[nodiscard]] Result {
public:
  static Result success(T value) {
    return Result{std::in_place_index<0>, std::move(value)};
  }
  static Result failure(Error error) { return Result{std::in_place_index<1>, error}; }
  bool has_value() const noexcept { return data_.index() == 0; }
  T &value() & { return std::get<0>(data_); }
  const T &value() const & { return std::get<0>(data_); }
  T &&value() && { return std::get<0>(std::move(data_)); }
  const Error &error() const & { return std::get<1>(data_); }

private:
  template <std::size_t Index, typename Value>
  Result(std::in_place_index_t<Index> index, Value &&value)
      : data_(index, std::forward<Value>(value)) {}
  std::variant<T, Error> data_;
};

template <> class [[nodiscard]] Result<void> {
public:
  static Result success() { return Result{std::monostate{}}; }
  static Result failure(Error error) { return Result{error}; }
  bool has_value() const noexcept { return data_.index() == 0; }
  void value() const { (void)std::get<0>(data_); }
  const Error &error() const & { return std::get<1>(data_); }

private:
  explicit Result(std::monostate value) : data_(value) {}
  explicit Result(Error error) : data_(error) {}
  std::variant<std::monostate, Error> data_;
};

using IdentityBytes = std::array<std::byte, 16>;
template <typename Tag> class Id {
public:
  static Result<Id> from_bytes(IdentityBytes bytes) {
    for (const auto byte : bytes) {
      if (byte != std::byte{0}) {
        return Result<Id>::success(Id{bytes});
      }
    }
    return Result<Id>::failure({ErrorCode::invalid_identity});
  }
  const IdentityBytes &bytes() const noexcept { return bytes_; }
  auto operator<=>(const Id &) const = default;

private:
  explicit Id(IdentityBytes bytes) : bytes_(bytes) {}
  IdentityBytes bytes_;
};
struct EnvironmentTag;
struct ParticipantTag;
struct ConversationTag;
struct WorkflowTag;
struct InvocationTag;
struct OperationAttemptTag;
struct ProcessIncarnationTag;
struct ContextRevisionTag;
struct DefinitionGenerationTag;
struct AuditStreamTag;
struct RegistryTag;
struct ClockTag;
using EnvironmentId = Id<EnvironmentTag>;
using ParticipantId = Id<ParticipantTag>;
using ConversationId = Id<ConversationTag>;
using WorkflowId = Id<WorkflowTag>;
using InvocationId = Id<InvocationTag>;
using OperationAttemptId = Id<OperationAttemptTag>;
using ProcessIncarnationId = Id<ProcessIncarnationTag>;
using ContextRevisionId = Id<ContextRevisionTag>;
using DefinitionGenerationId = Id<DefinitionGenerationTag>;
using AuditStreamId = Id<AuditStreamTag>;
using RegistryId = Id<RegistryTag>;
using ClockId = Id<ClockTag>;

class AuditSequence {
public:
  static Result<AuditSequence> from(std::uint64_t value);
  Result<AuditSequence> next() const;
  std::uint64_t value() const noexcept { return value_; }
  auto operator<=>(const AuditSequence &) const = default;

private:
  explicit AuditSequence(std::uint64_t value) : value_(value) {}
  std::uint64_t value_;
};

struct HandleDomain {
  EnvironmentId environment;
  ProcessIncarnationId incarnation;
  RegistryId registry;
  bool operator==(const HandleDomain &) const = default;
};
template <typename Tag> struct Handle {
  HandleDomain domain;
  std::size_t slot;
  std::uint64_t generation;
  bool operator==(const Handle &) const = default;
};

// Single execution owner. Domain uniqueness is the caller's issuer obligation.
// This validates tokens, not payload lifetime or concurrent resource access.
template <typename Tag> class HandleRegistry {
public:
  static Result<HandleRegistry> create(HandleDomain domain, std::size_t capacity,
                                       std::uint64_t initial_generation = 1) {
    if (initial_generation == 0) {
      return Result<HandleRegistry>::failure({ErrorCode::invalid_identity});
    }
    std::vector<Slot> slots;
    if (capacity == 0 || capacity > slots.max_size()) {
      return Result<HandleRegistry>::failure({ErrorCode::capacity});
    }
    try {
      slots.resize(capacity, Slot{initial_generation, false});
      return Result<HandleRegistry>::success(HandleRegistry{domain, std::move(slots)});
    } catch (const std::bad_alloc &) {
      return Result<HandleRegistry>::failure({ErrorCode::allocation});
    }
  }
  HandleRegistry(const HandleRegistry &) = delete;
  HandleRegistry &operator=(const HandleRegistry &) = delete;
  HandleRegistry(HandleRegistry &&other) noexcept
      : domain_(std::exchange(other.domain_, std::nullopt)),
        slots_(std::move(other.slots_)) {}
  HandleRegistry &operator=(HandleRegistry &&other) noexcept {
    if (this != &other) {
      domain_ = std::exchange(other.domain_, std::nullopt);
      slots_ = std::move(other.slots_);
    }
    return *this;
  }
  Result<Handle<Tag>> acquire() {
    if (!domain_) {
      return Result<Handle<Tag>>::failure({ErrorCode::stale_registry});
    }
    for (std::size_t index = 0; index < slots_.size(); ++index) {
      auto &slot = slots_[index];
      if (!slot.live && slot.generation != 0) {
        slot.live = true;
        return Result<Handle<Tag>>::success({*domain_, index, slot.generation});
      }
    }
    return Result<Handle<Tag>>::failure({ErrorCode::capacity});
  }
  Result<std::size_t> validate(const Handle<Tag> &handle) const {
    if (!domain_) {
      return Result<std::size_t>::failure({ErrorCode::stale_registry});
    }
    if (handle.domain.environment != domain_->environment) {
      return Result<std::size_t>::failure({ErrorCode::wrong_environment});
    }
    if (handle.domain.incarnation != domain_->incarnation) {
      return Result<std::size_t>::failure({ErrorCode::wrong_incarnation});
    }
    if (handle.domain.registry != domain_->registry) {
      return Result<std::size_t>::failure({ErrorCode::wrong_registry});
    }
    if (handle.slot >= slots_.size() || !slots_[handle.slot].live ||
        slots_[handle.slot].generation != handle.generation) {
      return Result<std::size_t>::failure({ErrorCode::stale_handle});
    }
    return Result<std::size_t>::success(handle.slot);
  }
  Result<void> release(const Handle<Tag> &handle) {
    const auto checked = validate(handle);
    if (!checked.has_value()) {
      return Result<void>::failure(checked.error());
    }
    auto &slot = slots_[checked.value()];
    slot.live = false;
    slot.generation = slot.generation == UINT64_MAX ? 0 : slot.generation + 1;
    return Result<void>::success();
  }

private:
  struct Slot {
    std::uint64_t generation;
    bool live;
  };
  HandleRegistry(HandleDomain domain, std::vector<Slot> slots)
      : domain_(domain), slots_(std::move(slots)) {}
  std::optional<HandleDomain> domain_;
  std::vector<Slot> slots_;
};

using ByteView = std::span<const std::byte>;
using MutableByteView = std::span<std::byte>;
Result<ByteView> slice(ByteView bytes, std::size_t offset, std::size_t length);
class ByteBuffer {
public:
  static Result<ByteBuffer> copy(ByteView bytes, std::size_t limit);
  ByteView view() const & noexcept { return data_; }
  ByteView view() const && = delete;
  std::size_t size() const noexcept { return data_.size(); }

private:
  explicit ByteBuffer(std::vector<std::byte> data) : data_(std::move(data)) {}
  std::vector<std::byte> data_;
};

struct ClockDomain {
  EnvironmentId environment;
  ProcessIncarnationId incarnation;
  ClockId clock;
  bool operator==(const ClockDomain &) const = default;
};
struct Duration {
  std::uint64_t nanoseconds;
};
struct MonotonicInstant {
  ClockDomain domain;
  std::int64_t nanoseconds;
};
struct WallObservation {
  ClockDomain domain;
  std::int64_t nanoseconds;
};
Result<Duration> elapsed(const MonotonicInstant &earlier,
                         const MonotonicInstant &later);
class Deadline {
public:
  static Result<Deadline> after(MonotonicInstant start, Duration duration);
  Result<bool> due(const MonotonicInstant &now) const;
  const MonotonicInstant &instant() const noexcept { return due_; }

private:
  Deadline(MonotonicInstant start, std::int64_t due_nanoseconds)
      : start_(start), due_({start.domain, due_nanoseconds}) {}
  MonotonicInstant start_;
  MonotonicInstant due_;
};

class ClockSample {
public:
  static Result<ClockSample> from(WallObservation wall, MonotonicInstant monotonic);
  WallObservation wall;
  MonotonicInstant monotonic;

private:
  ClockSample(WallObservation wall_value, MonotonicInstant monotonic_value)
      : wall(wall_value), monotonic(monotonic_value) {}
};
class ClockSource {
public:
  virtual ~ClockSource() = default;
  virtual Result<ClockSample> observe() = 0;
};
class NativeClock final : public ClockSource {
public:
  explicit NativeClock(ClockDomain domain) : domain_(domain) {}
  Result<ClockSample> observe() override;

private:
  ClockDomain domain_;
};

enum class SyncStrength : std::uint8_t { data, full };
class Storage {
public:
  virtual ~Storage() = default;
  virtual Result<std::size_t> read_at(std::uint64_t offset, MutableByteView output) = 0;
  virtual Result<std::size_t> write_at(std::uint64_t offset, ByteView input) = 0;
  virtual Result<std::uint64_t> extent() = 0;
  virtual Result<void> synchronize(SyncStrength strength) = 0;
};
struct EffectIntent {
  EnvironmentId environment;
  ParticipantId actor;
  InvocationId invocation;
  OperationAttemptId attempt;
  ByteView input;
};
class EffectBoundary {
public:
  virtual ~EffectBoundary() = default;
  // Failure is a local adapter result, not evidence that an external effect did not
  // occur.
  virtual Result<void> dispatch(const EffectIntent &intent) = 0;
};

} // namespace arconaut
