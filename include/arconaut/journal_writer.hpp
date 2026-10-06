#pragma once

#include "arconaut/journal.hpp"
#include "arconaut/journal_storage.hpp"

namespace arconaut {

struct JournalCapacity {
  std::uint64_t max_file_bytes;
  std::size_t max_records;
};
struct JournalDraft {
  FrameKind kind;
  ByteView payload;
};
struct PhysicalJournalRecord {
  EnvironmentId environment;
  AuditStreamId journal;
  FrameKind kind;
  std::uint64_t sequence;
  std::uint64_t batch_first;
  std::uint64_t payload_offset;
  std::uint32_t payload_size;
  std::uint32_t frame_checksum = 0;
  bool operator==(const PhysicalJournalRecord &) const = default;
};
struct JournalCursor {
  AuditStreamId journal;
  std::uint64_t sequence;
  std::uint64_t end_offset;
};
enum class JournalWriterState : std::uint8_t {
  live,
  recovery_pending,
  recovered,
  blocked,
  poisoned
};
struct JournalRecoveryReport {
  std::optional<Error> problem;
  std::uint64_t diagnostic_offset;
  std::uint64_t available_end;
  bool clean() const noexcept { return !problem; }
};
// Current storage observation, not validated history or a source dependency.
struct ObservedJournalBytes {
  AuditStreamId journal;
  std::uint64_t offset;
  std::uint64_t observed_extent;
  std::vector<std::byte> bytes;
};

// Internal physical framing owner. RetainedState validates all semantic bodies /
// source dependencies and owns authoritative publication. These physical indexes
// are inspection/replay inputs, never an admission permission or context API.
// The supplied directory must outlive this object; RetainedState owns both.
class FramedJournal {
public:
  static Result<std::unique_ptr<FramedJournal>>
  create(JournalDirectory &directory, std::string_view name, JournalHeader header,
         JournalCapacity capacity, SyncStrength strength = SyncStrength::full);
  static Result<std::unique_ptr<FramedJournal>>
  open(JournalDirectory &directory, std::string_view name, JournalHeader expected,
       JournalCapacity capacity, SyncStrength strength = SyncStrength::full);
  FramedJournal(const FramedJournal &) = delete;
  FramedJournal &operator=(const FramedJournal &) = delete;
  Result<JournalCursor> append(std::span<const JournalDraft> drafts);
  // Close writes and restage under the existing descriptor/lease, preserving
  // and comparing the previously acknowledged physical prefix.
  Result<void> restage();
  Result<void> select_recovered_prefix(JournalPredecessor selected);
  // Caller has staged and validated semantic replay before this new sync.
  // A damaged tail remains explicit/blocked; a valid prefix can be inspected.
  Result<void> confirm_recovery();
  // Semantic replay may reject an entire structurally complete batch. Retain
  // rejected/later records for diagnosis and restrict publication to its prefix.
  Result<void> reject_recovery_batch(std::uint64_t batch_first, Error reason);
  // U3/RetainedState verifies actual living resource custody before this call.
  Result<void> resume_after_reconciliation();
  Result<std::vector<std::byte>> read_payload(const PhysicalJournalRecord &record);
  Result<ObservedJournalBytes> read_original_range(std::uint64_t offset,
                                                   std::size_t length);
  const JournalHeader &header() const noexcept { return header_; }
  JournalWriterState state() const noexcept { return state_; }
  JournalCursor cursor() const noexcept { return cursor_; }
  const JournalRecoveryReport &recovery_report() const noexcept { return recovery_; }
  std::span<const PhysicalJournalRecord> physical_records() const noexcept {
    return records_;
  }
  std::span<const PhysicalJournalRecord> staged_records() const noexcept {
    return staged_;
  }
  std::span<const PhysicalJournalRecord> pending_records() const noexcept {
    return pending_;
  }

private:
  FramedJournal(JournalDirectory &directory, JournalHeader header,
                JournalCapacity capacity, SyncStrength strength);
  static Result<std::unique_ptr<FramedJournal>> allocate(JournalDirectory &directory,
                                                         JournalHeader header,
                                                         JournalCapacity capacity,
                                                         SyncStrength strength);
  Result<void> scan(bool propagate_read_errors = false);
  Result<void> read_exact(std::uint64_t offset, MutableByteView output);
  Result<void> write_exact(std::uint64_t offset, ByteView input);
  JournalHeader header_;
  JournalCapacity capacity_;
  SyncStrength strength_;
  JournalDirectory &directory_;
  std::unique_ptr<JournalFile> file_;
  std::vector<PhysicalJournalRecord> records_;
  std::vector<PhysicalJournalRecord> staged_;
  std::vector<PhysicalJournalRecord> pending_;
  JournalCursor cursor_;
  JournalRecoveryReport recovery_;
  JournalWriterState state_ = JournalWriterState::recovery_pending;
  bool historical_ = false;
  bool in_restage_ = false;
};

} // namespace arconaut
