#pragma once
#include "blackbird/journal_writer.hpp"
#include "blackbird/recovery_index.hpp"

namespace blackbird {
struct ArchiveEntry {
  RecoveryKey key;
  PhysicalJournalRecord record;
};
// Immutable sorted disk locators. Only a checked header and one selected entry
// are resident on reopen; count does not size a vector or a permanent keydir.
// Publication uses a complete batched file, not one path-copy per put.
class ArchiveCatalog {
public:
  static Result<void> publish(JournalDirectory &directory, std::string_view name,
                              JournalHeader header, JournalCursor boundary,
                              std::span<const ArchiveEntry> sorted);
  static Result<std::uint64_t>
  publish_merge(JournalDirectory &directory, std::string_view name,
                JournalHeader header, JournalCursor boundary,
                const ArchiveCatalog *previous, std::span<const ArchiveEntry> delta);
  static Result<std::unique_ptr<ArchiveCatalog>>
  open(JournalDirectory &directory, std::string_view name, JournalHeader header,
       JournalCursor boundary, std::uint64_t maximum_entries);
  Result<std::optional<PhysicalJournalRecord>> find(const RecoveryKey &key) const;
  std::uint64_t count() const noexcept { return count_; }
  Result<ArchiveEntry> entry(std::uint64_t ordinal) const;

private:
  ArchiveCatalog(std::unique_ptr<JournalFile> file, JournalHeader header,
                 JournalCursor boundary, std::uint64_t count)
      : file_(std::move(file)), header_(header), boundary_(boundary), count_(count) {}
  std::unique_ptr<JournalFile> file_;
  JournalHeader header_;
  JournalCursor boundary_;
  std::uint64_t count_;
};
} // namespace blackbird
