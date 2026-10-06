#pragma once

#include "arconaut/journal_writer.hpp"
#include "arconaut/retained_events.hpp"

namespace arconaut {
struct RecordReference {
  AuditStreamId journal;
  std::uint64_t sequence;
  bool operator==(const RecordReference &) const = default;
};
struct RetainedFact {
  RecordReference record;
  RetainedEvent event;
  RetainedEvidence evidence;
};
struct Submission {
  bool existing;
  RecordReference record;
  RetainedEvidence evidence;
};
struct PendingProposal {
  ProvisionalCaptureEvent capture;
  std::vector<std::byte> bytes;
};
struct ProvisionalOriginal {
  RecordReference marker;
  ProvisionalCaptureEvent capture;
  std::vector<std::byte> bytes;
};
struct AttemptState {
  AttemptAdmissionEvent admission;
  bool opened;
  std::optional<AttemptObservationEvent> observation;
  std::optional<AdapterReceiptEvent> receipt;
  RetainedEvidence evidence;
  bool reconciliation_required;
};
class CustodyVerifier {
public:
  virtual ~CustodyVerifier() = default;
  // U3 implements actual resource/authority checks. Success never grants an old
  // recovered attempt permission to dispatch anew.
  virtual Result<void> verify(std::span<const AttemptState> attempts) = 0;
};
struct DispatchReport {
  bool dispatched;
  Result<void> effect;
  Result<void> recording;
};

// One sequential custodian owner, including reentrant effect callbacks. Caches
// are derived from this same journal. Ordinary model bindings must route external
// effects through dispatch; appending a fact alone never invokes an effect seam.
class RetainedState {
public:
  static Result<std::unique_ptr<RetainedState>>
  create(std::unique_ptr<JournalDirectory> directory, std::string_view name,
         JournalHeader header, JournalCapacity capacity);
  static Result<std::unique_ptr<RetainedState>>
  open(std::unique_ptr<JournalDirectory> directory, std::string_view name,
       JournalHeader header, JournalCapacity capacity);
  RetainedState(const RetainedState &) = delete;
  RetainedState &operator=(const RetainedState &) = delete;
  Result<Submission> submit(const RetainedEvent &event);
  Result<JournalCursor> append(JournalCursor expected,
                               std::span<const ByteView> sources,
                               std::span<const RetainedEvent> events);
  Result<DispatchReport> dispatch(OperationAttemptId attempt, EffectBoundary &boundary);
  Result<AttemptState> attempt(OperationAttemptId identity) const;
  Result<std::vector<std::byte>> source(SourceReference reference);
  // Unvalidated bounded inspection remains available while admission is closed.
  Result<ObservedJournalBytes> read_original_range(std::uint64_t offset,
                                                   std::size_t length) {
    return journal_->read_original_range(offset, length);
  }
  Result<void> confirm_recovery();
  Result<void> reconcile(CustodyVerifier &verifier);
  // A failed application-level recovery must close admission until reopen.
  void block_admission() noexcept { recording_failed_ = true; }
  template <typename T> Result<T> issue() {
    auto reserved = reserve_identity();
    if (!reserved.has_value()) {
      return Result<T>::failure(reserved.error());
    }
    return T::from_bytes(std::move(reserved).value());
  }
  JournalCursor cursor() const noexcept { return journal_->cursor(); }
  JournalWriterState state() const noexcept {
    return recording_failed_ && journal_->state() == JournalWriterState::live
               ? JournalWriterState::blocked
               : journal_->state();
  }
  const JournalRecoveryReport &recovery_report() const noexcept {
    return journal_->recovery_report();
  }
  // Borrow lasts until the next mutation; no staged/uncertain facts appear here.
  std::span<const RetainedFact> committed_facts() const noexcept {
    return committed_.facts;
  }
  std::uint64_t issuer_counter() const noexcept;
  // Surviving RAM originals, not committed sources or permission. Borrow ends
  // at the next mutation; reopening cannot recreate lost process memory.
  const std::optional<PendingProposal> &pending_proposal() const noexcept {
    return pending_proposal_;
  }

private:
  friend class RetainedEnvironment;
  struct Snapshot {
    std::vector<RetainedFact> facts;
    std::vector<PhysicalJournalRecord> sources;
    std::vector<RetainedFact> uncertain_facts;
    std::vector<ProvisionalOriginal> provisional_originals;
    std::uint64_t issuer_namespace = 0;
    std::uint64_t counter = 0;
    void swap(Snapshot &other) noexcept;
  };
  RetainedState(std::unique_ptr<JournalDirectory> directory, JournalCapacity capacity);
  static Result<std::unique_ptr<RetainedState>>
  allocate(std::unique_ptr<JournalDirectory> directory, JournalCapacity capacity);
  Result<void> rebuild();
  Result<void> replay(Snapshot &snapshot, FramedJournal &journal,
                      std::span<const SourceReference> capture_only = {},
                      std::uint64_t maintenance_end = 0);
  bool has_room(const Snapshot &snapshot, std::size_t count) const noexcept;
  FramedJournal *segment(AuditStreamId identity) const noexcept;
  Result<void> apply(Snapshot &snapshot, RetainedEvent event, RecordReference record,
                     RetainedEvidence evidence);
  Result<JournalCursor> append_impl(JournalCursor expected,
                                    std::span<const ByteView> sources,
                                    std::span<const RetainedEvent> events,
                                    bool diagnostic);
  Result<void> retain_rejection(JournalCursor expected,
                                std::span<const ByteView> sources,
                                std::span<const std::vector<std::byte>> events,
                                Error reason, const RetainedEvent *conflict);
  const RetainedFact *
  existing(const Snapshot &snapshot, const RetainedEvent &event,
           std::optional<std::uint64_t> issuer_namespace = std::nullopt) const;
  const RetainedFact *existing_uncertain(
      const Snapshot &snapshot, const RetainedEvent &event,
      std::optional<std::uint64_t> issuer_namespace = std::nullopt) const;
  static bool has_identity(const RetainedEvent &event);
  const Snapshot &visible() const noexcept;
  Result<IdentityBytes> reserve_identity();
  std::unique_ptr<JournalDirectory> directory_;
  std::vector<std::unique_ptr<FramedJournal>> historical_;
  std::unique_ptr<FramedJournal> journal_;
  JournalCapacity capacity_;
  std::size_t max_history_entries_;
  Snapshot committed_;
  std::optional<Snapshot> prepared_;
  std::optional<PendingProposal> pending_proposal_;
  std::uint64_t pending_counter_ = 0;
  std::uint64_t pending_namespace_ = 0;
  bool reconciled_ = true;
  bool in_transaction_ = false;
  bool recording_failed_ = false;
};
} // namespace arconaut
