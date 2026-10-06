#pragma once

#include "arconaut/journal_writer.hpp"

namespace arconaut {
struct RetainedProposal {
  JournalCursor expected;
  std::vector<std::vector<std::byte>> sources;
  std::vector<std::vector<std::byte>> events;
};
// Exact ARPROP01 caller originals, with no semantic/admission authority. The
// caller supplies a byte bound; input-declared counts never choose that bound.
Result<std::vector<std::byte>>
encode_retained_proposal(JournalCursor expected, std::span<const ByteView> sources,
                         std::span<const std::vector<std::byte>> events,
                         std::uint64_t max_bytes);
Result<RetainedProposal> decode_retained_proposal(ByteView bytes,
                                                  std::uint64_t max_bytes);
} // namespace arconaut
