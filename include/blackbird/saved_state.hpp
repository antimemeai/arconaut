#pragma once
#include "blackbird/json.hpp"
#include "blackbird/retained_state.hpp"

namespace blackbird {
// Derived current-state reduction, not a serialized Snapshot or physical catalog.
// Historical bytes remain authoritative and are deliberately absent here.
struct SavedState {
  JournalCursor boundary;
  std::uint64_t fact_count = 0;
  std::uint64_t issuer_counter = 0;
  Json context;
  Json programs;
  std::vector<RetainedFact> unresolved;
  unsigned slot = 0;
};
Result<std::optional<SavedState>> load_saved_state(
    JournalDirectory &directory, std::string_view name, FramedJournal &journal,
    JournalCapacity capacity);
Result<void> publish_saved_state(JournalDirectory &directory, std::string_view name,
    FramedJournal &journal, const SavedState &saved, unsigned slot);
} // namespace blackbird
