#include "blackbird/retained_proposal.hpp"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

using namespace blackbird;
namespace {
std::atomic<std::ptrdiff_t> allocation_cut{-1};
}
void *operator new(std::size_t size) {
  if (allocation_cut.load() >= 0 && allocation_cut.fetch_sub(1) == 0) {
    throw std::bad_alloc{};
  }
  if (void *pointer = std::malloc(size == 0 ? 1 : size)) {
    return pointer;
  }
  throw std::bad_alloc{};
}
void operator delete(void *pointer) noexcept { std::free(pointer); }
void *operator new[](std::size_t size) { return ::operator new(size); }
void operator delete[](void *pointer) noexcept { ::operator delete(pointer); }
#if defined(__linux__)
void operator delete(void *pointer, std::size_t) noexcept { std::free(pointer); }
void operator delete[](void *pointer, std::size_t) noexcept { std::free(pointer); }
#endif
namespace {
void check(bool condition, const char *expression, int line) {
  if (!condition) {
    throw std::runtime_error(std::to_string(line) + ": " + expression);
  }
}
#define CHECK(...) check((__VA_ARGS__), #__VA_ARGS__, __LINE__)
template <typename T> T require(Result<T> result) {
  CHECK(result.has_value());
  return std::move(result).value();
}
template <typename T> void error_is(const Result<T> &result, ErrorCode code) {
  CHECK(!result.has_value() && result.error().code == code);
}
AuditStreamId journal() {
  IdentityBytes bytes{};
  bytes[15] = std::byte{2};
  return require(AuditStreamId::from_bytes(bytes));
}
void exact_owned_proposal() {
  const JournalCursor cursor{journal(), 2, 1178};
  std::vector<std::byte> source{std::byte{0}, std::byte{255}, std::byte{10}};
  std::vector<std::byte> reservation(16, std::byte{0});
  reservation[0] = std::byte{1};
  reservation[2] = std::byte{1};
  reservation[8] = std::byte{77};
  const std::array<ByteView, 1> sources{source};
  std::vector<std::vector<std::byte>> events{reservation};
  std::vector<std::byte> expected(75, std::byte{0});
  const auto magic = std::as_bytes(std::span{"ARPROP01", 8});
  std::copy(magic.begin(), magic.end(), expected.begin());
  expected[23] = std::byte{2};
  expected[24] = std::byte{2};
  expected[32] = std::byte{154};
  expected[33] = std::byte{4};
  expected[40] = std::byte{1};
  expected[44] = std::byte{3};
  expected[49] = std::byte{255};
  expected[50] = std::byte{10};
  expected[51] = std::byte{1};
  expected[55] = std::byte{16};
  expected[59] = std::byte{1};
  expected[61] = std::byte{1};
  expected[67] = std::byte{77};
  CHECK(require(encode_retained_proposal(cursor, sources, events, 75)) == expected);
  auto decoded = require(decode_retained_proposal(expected, 75));
  CHECK(decoded.expected.journal == cursor.journal && decoded.expected.sequence == 2 &&
        decoded.expected.end_offset == 1178);
  CHECK(decoded.sources == std::vector<std::vector<std::byte>>{source});
  CHECK(decoded.events == events);
  // Parsed bytes own their storage independently of input/caller memory.
  auto mutable_wire = expected;
  auto owned = require(decode_retained_proposal(mutable_wire, 75));
  std::fill(mutable_wire.begin(), mutable_wire.end(), std::byte{33});
  CHECK(owned.sources == decoded.sources && owned.events == decoded.events);
  error_is(encode_retained_proposal(cursor, sources, events, 74), ErrorCode::capacity);
  error_is(decode_retained_proposal(expected, 74), ErrorCode::capacity);
  for (std::size_t end = 0; end < expected.size(); ++end) {
    error_is(decode_retained_proposal(ByteView{expected}.first(end), 75),
             ErrorCode::corrupt);
  }
  auto bad = expected;
  bad.push_back(std::byte{0});
  error_is(decode_retained_proposal(bad, 76), ErrorCode::corrupt);
  for (const auto offset : std::array<std::size_t, 6>{0, 23, 40, 44, 51, 55}) {
    bad = expected;
    bad[offset] = offset == 23 ? std::byte{0} : std::byte{255};
    error_is(decode_retained_proposal(bad, 75), ErrorCode::corrupt);
  }
  // A rejected proposal may have any stale cursor; parser cannot grant authority.
  const JournalCursor stale{journal(), UINT64_MAX, UINT64_MAX};
  const auto stale_bytes = require(encode_retained_proposal(stale, {}, {}, 48));
  const auto parsed_stale = require(decode_retained_proposal(stale_bytes, 48));
  CHECK(parsed_stale.expected.sequence == UINT64_MAX &&
        parsed_stale.expected.end_offset == UINT64_MAX);
  const std::array<ByteView, 2> empty_sources{ByteView{}, ByteView{}};
  const std::vector<std::vector<std::byte>> empty_events{{}, {}};
  const auto empty =
      require(encode_retained_proposal(cursor, empty_sources, empty_events, 64));
  const auto empty_decoded = require(decode_retained_proposal(empty, 64));
  CHECK(empty.size() == 64 && empty_decoded.sources == empty_events &&
        empty_decoded.events == empty_events);
  for (const bool encoding : {false, true}) {
    bool success = false;
    for (std::ptrdiff_t cut = 0; cut < 64; ++cut) {
      allocation_cut.store(cut);
      if (encoding) {
        const auto result = encode_retained_proposal(cursor, sources, events, 75);
        allocation_cut.store(-1);
        if (result.has_value()) {
          CHECK(result.value() == expected);
          success = true;
          break;
        }
        error_is(result, ErrorCode::allocation);
      } else {
        const auto result = decode_retained_proposal(expected, 75);
        allocation_cut.store(-1);
        if (result.has_value()) {
          CHECK(result.value().sources == decoded.sources &&
                result.value().events == decoded.events);
          success = true;
          break;
        }
        error_is(result, ErrorCode::allocation);
      }
    }
    CHECK(success);
  }
}
} // namespace
int main() {
  try {
    exact_owned_proposal();
    std::puts("PASS exact owned proposal bytes, bounded parsing and allocator cuts");
    return 0;
  } catch (const std::exception &error) {
    std::fprintf(stderr, "FAIL retained proposal: %s\n", error.what());
    return 1;
  }
}
