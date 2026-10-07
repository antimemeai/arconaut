#include "blackbird/retained_proposal.hpp"

#include <algorithm>

namespace blackbird {
namespace {
constexpr std::string_view magic{"ARPROP01"};
template <std::size_t Width>
void put(std::vector<std::byte> &bytes, std::uint64_t value) {
  for (std::size_t index = 0; index < Width; ++index) {
    bytes.push_back(static_cast<std::byte>(value & 255));
    value >>= 8;
  }
}
class ProposalReader {
public:
  explicit ProposalReader(ByteView bytes) : bytes_(bytes) {}
  ByteView take(std::size_t count) noexcept {
    if (failed_ || count > remaining()) {
      failed_ = true;
      return {};
    }
    const auto view = bytes_.subspan(offset_, count);
    offset_ += count;
    return view;
  }
  template <std::size_t Width> std::uint64_t number() noexcept {
    const auto bytes = take(Width);
    std::uint64_t value = 0;
    for (std::size_t index = 0; index < bytes.size(); ++index) {
      value |= static_cast<std::uint64_t>(std::to_integer<unsigned int>(bytes[index]))
               << (index * 8);
    }
    return value;
  }
  Result<std::vector<std::vector<std::byte>>> blobs() {
    const auto count = number<4>();
    if (failed_ || count > remaining() / 4) {
      return Result<std::vector<std::vector<std::byte>>>::failure({ErrorCode::corrupt});
    }
    std::vector<std::vector<std::byte>> values;
    values.reserve(static_cast<std::size_t>(count));
    for (std::uint64_t index = 0; index < count; ++index) {
      const auto length = number<4>();
      const auto view = take(static_cast<std::size_t>(length));
      if (failed_) {
        return Result<std::vector<std::vector<std::byte>>>::failure(
            {ErrorCode::corrupt});
      }
      values.emplace_back(view.begin(), view.end());
    }
    return Result<std::vector<std::vector<std::byte>>>::success(std::move(values));
  }
  bool complete() const noexcept { return !failed_ && remaining() == 0; }

private:
  std::size_t remaining() const noexcept { return bytes_.size() - offset_; }
  ByteView bytes_;
  std::size_t offset_ = 0;
  bool failed_ = false;
};
} // namespace
Result<std::vector<std::byte>>
encode_retained_proposal(JournalCursor expected, std::span<const ByteView> sources,
                         std::span<const std::vector<std::byte>> events,
                         std::uint64_t max_bytes) {
  const auto limit =
      std::min<std::uint64_t>(max_bytes, std::vector<std::byte>{}.max_size());
  if (limit < 48 || sources.size() > UINT32_MAX || events.size() > UINT32_MAX) {
    return Result<std::vector<std::byte>>::failure({ErrorCode::capacity});
  }
  std::size_t size = 48;
  const auto add = [&](std::size_t length) {
    if (length > UINT32_MAX || length > limit - 4 || size > limit - 4 - length) {
      return false;
    }
    size += 4 + length;
    return true;
  };
  for (const auto source : sources) {
    if (!add(source.size())) {
      return Result<std::vector<std::byte>>::failure({ErrorCode::capacity});
    }
  }
  for (const auto &event : events) {
    if (!add(event.size())) {
      return Result<std::vector<std::byte>>::failure({ErrorCode::capacity});
    }
  }
  try {
    std::vector<std::byte> proposal;
    proposal.reserve(size);
    for (const auto value : magic) {
      proposal.push_back(static_cast<std::byte>(value));
    }
    proposal.insert(proposal.end(), expected.journal.bytes().begin(),
                    expected.journal.bytes().end());
    put<8>(proposal, expected.sequence);
    put<8>(proposal, expected.end_offset);
    put<4>(proposal, sources.size());
    for (const auto source : sources) {
      put<4>(proposal, source.size());
      proposal.insert(proposal.end(), source.begin(), source.end());
    }
    put<4>(proposal, events.size());
    for (const auto &event : events) {
      put<4>(proposal, event.size());
      proposal.insert(proposal.end(), event.begin(), event.end());
    }
    return Result<std::vector<std::byte>>::success(std::move(proposal));
  } catch (const std::bad_alloc &) {
    return Result<std::vector<std::byte>>::failure({ErrorCode::allocation});
  }
}
Result<RetainedProposal> decode_retained_proposal(ByteView bytes,
                                                  std::uint64_t max_bytes) {
  if (bytes.size() > max_bytes) {
    return Result<RetainedProposal>::failure({ErrorCode::capacity});
  }
  if (bytes.size() < 48) {
    return Result<RetainedProposal>::failure({ErrorCode::corrupt});
  }
  try {
    ProposalReader reader{bytes};
    const auto prefix = reader.take(8);
    const auto expected_magic = std::as_bytes(std::span{magic.data(), magic.size()});
    if (!std::equal(prefix.begin(), prefix.end(), expected_magic.begin(),
                    expected_magic.end())) {
      return Result<RetainedProposal>::failure({ErrorCode::corrupt});
    }
    IdentityBytes id{};
    const auto raw_id = reader.take(16);
    std::copy(raw_id.begin(), raw_id.end(), id.begin());
    auto journal = AuditStreamId::from_bytes(id);
    if (!journal.has_value()) {
      return Result<RetainedProposal>::failure({ErrorCode::corrupt});
    }
    const auto sequence = reader.number<8>();
    const auto end = reader.number<8>();
    auto sources = reader.blobs();
    if (!sources.has_value()) {
      return Result<RetainedProposal>::failure(sources.error());
    }
    auto events = reader.blobs();
    if (!events.has_value()) {
      return Result<RetainedProposal>::failure(events.error());
    }
    if (!reader.complete()) {
      return Result<RetainedProposal>::failure({ErrorCode::corrupt});
    }
    return Result<RetainedProposal>::success(
        {{std::move(journal).value(), sequence, end},
         std::move(sources).value(),
         std::move(events).value()});
  } catch (const std::bad_alloc &) {
    return Result<RetainedProposal>::failure({ErrorCode::allocation});
  }
}
} // namespace blackbird
