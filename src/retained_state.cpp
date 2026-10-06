#include "arconaut/retained_state.hpp"
#include "arconaut/retained_proposal.hpp"

#include <algorithm>
#include <type_traits>

namespace arconaut {
namespace {
struct EventKey {
  RetainedKind kind;
  IdentityBytes identity;
  bool operator==(const EventKey &) const = default;
};
std::optional<EventKey> key(const RetainedBody &body,
                            std::uint64_t issuer_namespace = 0) {
  return std::visit(
      [&](const auto &event) -> std::optional<EventKey> {
        using T = std::decay_t<decltype(event)>;
        const auto kind = retained_kind(body);
        if constexpr (std::is_same_v<T, DecisionEvent>) {
          return EventKey{kind, event.decision.bytes()};
        } else if constexpr (std::is_same_v<T, InvocationEvent>) {
          return EventKey{kind, event.invocation.bytes()};
        } else if constexpr (std::is_same_v<T, AttemptAdmissionEvent> ||
                             std::is_same_v<T, AttemptOpenEvent> ||
                             std::is_same_v<T, RetryEvent> ||
                             std::is_same_v<T, AdapterReceiptEvent>) {
          return EventKey{kind, event.attempt.bytes()};
        } else if constexpr (std::is_same_v<T, ComplaintEvent>) {
          return EventKey{kind, event.complaint.bytes()};
        } else if constexpr (std::is_same_v<T, ApplicationRecordEvent>) {
          return EventKey{kind, event.identity.bytes()};
        } else if constexpr (std::is_same_v<T, IssuerReservationEvent>) {
          IdentityBytes bytes{};
          auto counter = event.counter;
          for (std::size_t index = 0; index < 8; ++index) {
            bytes[index] = static_cast<std::byte>(issuer_namespace & 255);
            issuer_namespace >>= 8;
          }
          for (std::size_t index = 8; index < 16; ++index) {
            bytes[index] = static_cast<std::byte>(counter & 255);
            counter >>= 8;
          }
          return EventKey{kind, bytes};
        } else {
          return std::nullopt;
        }
      },
      body);
}
template <typename T, typename Predicate>
const T *find_event(std::span<const RetainedFact> facts, Predicate predicate) {
  for (auto it = facts.rbegin(); it != facts.rend(); ++it) {
    if (const auto *event = std::get_if<T>(&it->event.body);
        event && predicate(*event)) {
      return event;
    }
  }
  return nullptr;
}
bool plans(const DecisionEvent &decision, InvocationId invocation) {
  return std::find(decision.planned_invocations.begin(),
                   decision.planned_invocations.end(),
                   invocation) != decision.planned_invocations.end();
}
class TransactionGuard {
public:
  explicit TransactionGuard(bool &active) : active_(active) { active_ = true; }
  ~TransactionGuard() { active_ = false; }
  TransactionGuard(const TransactionGuard &) = delete;
  TransactionGuard &operator=(const TransactionGuard &) = delete;

private:
  bool &active_;
};
} // namespace

void RetainedState::Snapshot::swap(Snapshot &other) noexcept {
  facts.swap(other.facts);
  sources.swap(other.sources);
  uncertain_facts.swap(other.uncertain_facts);
  provisional_originals.swap(other.provisional_originals);
  std::swap(issuer_namespace, other.issuer_namespace);
  std::swap(counter, other.counter);
}
RetainedState::RetainedState(std::unique_ptr<JournalDirectory> directory,
                             JournalCapacity capacity)
    : directory_(std::move(directory)), capacity_(capacity),
      max_history_entries_(capacity.max_records) {
  committed_.facts.reserve(capacity.max_records);
  committed_.sources.reserve(capacity.max_records);
}
Result<std::unique_ptr<RetainedState>>
RetainedState::allocate(std::unique_ptr<JournalDirectory> directory,
                        JournalCapacity capacity) {
  if (!directory || capacity.max_records == 0 ||
      capacity.max_records > std::vector<RetainedFact>{}.max_size() ||
      capacity.max_records > std::vector<PhysicalJournalRecord>{}.max_size()) {
    return Result<std::unique_ptr<RetainedState>>::failure({ErrorCode::invalid_range});
  }
  try {
    return Result<std::unique_ptr<RetainedState>>::success(
        std::unique_ptr<RetainedState>{
            new RetainedState{std::move(directory), capacity}});
  } catch (const std::bad_alloc &) {
    return Result<std::unique_ptr<RetainedState>>::failure({ErrorCode::allocation});
  }
}
Result<std::unique_ptr<RetainedState>>
RetainedState::create(std::unique_ptr<JournalDirectory> directory,
                      std::string_view name, JournalHeader header,
                      JournalCapacity capacity) {
  // The linked-chain factory will supply validated predecessors. This root
  // factory must not accidentally publish a standalone continuation as history.
  if (header.predecessor) {
    return Result<std::unique_ptr<RetainedState>>::failure({ErrorCode::unsupported});
  }
  auto allocated = allocate(std::move(directory), capacity);
  if (!allocated.has_value()) {
    return allocated;
  }
  auto owner = std::move(allocated).value();
  auto journal = FramedJournal::create(*owner->directory_, name, header, capacity);
  if (!journal.has_value()) {
    return Result<std::unique_ptr<RetainedState>>::failure(journal.error());
  }
  owner->journal_ = std::move(journal).value();
  owner->committed_.issuer_namespace = header.issuer_namespace;
  owner->pending_namespace_ = header.issuer_namespace;
  return Result<std::unique_ptr<RetainedState>>::success(std::move(owner));
}
Result<std::unique_ptr<RetainedState>>
RetainedState::open(std::unique_ptr<JournalDirectory> directory, std::string_view name,
                    JournalHeader header, JournalCapacity capacity) {
  if (header.predecessor) {
    return Result<std::unique_ptr<RetainedState>>::failure({ErrorCode::unsupported});
  }
  auto allocated = allocate(std::move(directory), capacity);
  if (!allocated.has_value()) {
    return allocated;
  }
  auto owner = std::move(allocated).value();
  auto journal = FramedJournal::open(*owner->directory_, name, header, capacity);
  if (!journal.has_value()) {
    return Result<std::unique_ptr<RetainedState>>::failure(journal.error());
  }
  owner->journal_ = std::move(journal).value();
  owner->pending_namespace_ = header.issuer_namespace;
  owner->reconciled_ = false;
  const auto rebuilt = owner->rebuild();
  if (!rebuilt.has_value()) {
    return Result<std::unique_ptr<RetainedState>>::failure(rebuilt.error());
  }
  return Result<std::unique_ptr<RetainedState>>::success(std::move(owner));
}
const RetainedState::Snapshot &RetainedState::visible() const noexcept {
  return prepared_ ? *prepared_ : committed_;
}
bool RetainedState::has_identity(const RetainedEvent &event) {
  const auto *observation = std::get_if<AttemptObservationEvent>(&event.body);
  return key(event.body).has_value() ||
         (observation && observation->phase == AttemptPhase::terminal);
}
const RetainedFact *
RetainedState::existing(const Snapshot &snapshot, const RetainedEvent &event,
                        std::optional<std::uint64_t> issuer_namespace) const {
  const auto identity =
      key(event.body, issuer_namespace.value_or(snapshot.issuer_namespace));
  if (identity) {
    for (const auto &fact : snapshot.facts) {
      const auto *origin = segment(fact.record.journal);
      if (origin &&
          key(fact.event.body, origin->header().issuer_namespace) == identity) {
        return &fact;
      }
    }
  }
  if (const auto *observation = std::get_if<AttemptObservationEvent>(&event.body);
      observation && observation->phase == AttemptPhase::terminal) {
    for (auto it = snapshot.facts.rbegin(); it != snapshot.facts.rend(); ++it) {
      const auto *previous = std::get_if<AttemptObservationEvent>(&it->event.body);
      if (previous && previous->attempt == observation->attempt &&
          previous->phase == AttemptPhase::terminal) {
        return &*it;
      }
    }
  }
  return nullptr;
}
const RetainedFact *
RetainedState::existing_uncertain(const Snapshot &snapshot, const RetainedEvent &event,
                                  std::optional<std::uint64_t> issuer_namespace) const {
  const auto identity =
      key(event.body, issuer_namespace.value_or(snapshot.issuer_namespace));
  for (const auto &fact : snapshot.uncertain_facts) {
    const auto *origin = segment(fact.record.journal);
    if (identity && origin &&
        key(fact.event.body, origin->header().issuer_namespace) == identity)
      return &fact;
    const auto *observation = std::get_if<AttemptObservationEvent>(&event.body);
    const auto *previous = std::get_if<AttemptObservationEvent>(&fact.event.body);
    if (observation && previous && observation->phase == AttemptPhase::terminal &&
        previous->phase == AttemptPhase::terminal &&
        observation->attempt == previous->attempt)
      return &fact;
    if (!has_identity(event) && fact.event == event)
      return &fact;
  }
  return nullptr;
}
FramedJournal *RetainedState::segment(AuditStreamId identity) const noexcept {
  if (journal_ && journal_->header().journal == identity)
    return journal_.get();
  for (const auto &journal : historical_)
    if (journal->header().journal == identity)
      return journal.get();
  return nullptr;
}
bool RetainedState::has_room(const Snapshot &snapshot,
                             std::size_t count) const noexcept {
  auto remaining = max_history_entries_;
  const std::array sizes{snapshot.facts.size(), snapshot.sources.size(),
                         snapshot.uncertain_facts.size(),
                         snapshot.provisional_originals.size()};
  for (const auto size : sizes) {
    if (size > remaining)
      return false;
    remaining -= size;
  }
  return count <= remaining;
}
Result<void> RetainedState::apply(Snapshot &snapshot, RetainedEvent event,
                                  RecordReference record, RetainedEvidence evidence) {
  if (std::holds_alternative<RecoveryChoiceEvent>(event.body) ||
      std::holds_alternative<ProvisionalCaptureEvent>(event.body)) {
    return Result<void>::failure({ErrorCode::unsupported});
  }
  if (const auto *previous = existing(snapshot, event)) {
    return previous->event == event ? Result<void>::success()
                                    : Result<void>::failure({ErrorCode::conflict});
  }
  if (existing_uncertain(snapshot, event))
    return Result<void>::failure({ErrorCode::conflict});
  if (!has_room(snapshot, 1))
    return Result<void>::failure({ErrorCode::capacity});
  for (const auto &dependency : event.dependencies) {
    const auto source = std::find_if(snapshot.sources.begin(), snapshot.sources.end(),
                                     [&](const auto &candidate) {
                                       return candidate.journal == dependency.journal &&
                                              candidate.sequence == dependency.sequence;
                                     });
    if (source == snapshot.sources.end()) {
      return Result<void>::failure({ErrorCode::conflict});
    }
  }
  const auto validated = std::visit(
      [&](const auto &body) -> Result<void> {
        using T = std::decay_t<decltype(body)>;
        if constexpr (std::is_same_v<T, IssuerReservationEvent>) {
          if (body.counter <= snapshot.counter) {
            return Result<void>::failure({ErrorCode::conflict});
          }
          snapshot.counter = body.counter;
        } else if constexpr (std::is_same_v<T, DecisionEvent>) {
          for (std::size_t i = 0; i < body.planned_invocations.size(); ++i) {
            for (std::size_t j = i + 1; j < body.planned_invocations.size(); ++j) {
              if (body.planned_invocations[i] == body.planned_invocations[j]) {
                return Result<void>::failure({ErrorCode::invalid_range});
              }
            }
          }
        } else if constexpr (std::is_same_v<T, InvocationEvent>) {
          const auto *decision =
              find_event<DecisionEvent>(snapshot.facts, [&](const auto &value) {
                return value.decision == body.decision;
              });
          if (!decision || !plans(*decision, body.invocation)) {
            return Result<void>::failure({ErrorCode::conflict});
          }
        } else if constexpr (std::is_same_v<T, AttemptAdmissionEvent> ||
                             std::is_same_v<T, RetryEvent>) {
          const auto *invocation =
              find_event<InvocationEvent>(snapshot.facts, [&](const auto &value) {
                return value.invocation == body.invocation;
              });
          const auto *decision =
              find_event<DecisionEvent>(snapshot.facts, [&](const auto &value) {
                return value.decision == body.decision;
              });
          if (!invocation || !decision || !plans(*decision, body.invocation)) {
            return Result<void>::failure({ErrorCode::conflict});
          }
          const auto *prior =
              find_event<AttemptAdmissionEvent>(snapshot.facts, [&](const auto &value) {
                return value.invocation == body.invocation;
              });
          if constexpr (std::is_same_v<T, RetryEvent>) {
            const auto *used = find_event<AttemptAdmissionEvent>(
                snapshot.facts,
                [&](const auto &value) { return value.attempt == body.attempt; });
            const auto *prior_retry =
                find_event<RetryEvent>(snapshot.facts, [&](const auto &value) {
                  return value.decision == body.decision &&
                         value.invocation == body.invocation;
                });
            if (!prior || used || prior_retry ||
                body.decision == invocation->decision) {
              return Result<void>::failure({ErrorCode::conflict});
            }
          } else {
            if (prior) {
              const auto *retry =
                  find_event<RetryEvent>(snapshot.facts, [&](const auto &value) {
                    return value.attempt == body.attempt &&
                           value.invocation == body.invocation &&
                           value.decision == body.decision;
                  });
              if (!retry) {
                return Result<void>::failure({ErrorCode::conflict});
              }
            } else if (body.decision != invocation->decision) {
              return Result<void>::failure({ErrorCode::conflict});
            }
          }
        } else if constexpr (std::is_same_v<T, AdapterReceiptEvent>) {
          const auto *opened =
              find_event<AttemptOpenEvent>(snapshot.facts, [&](const auto &value) {
                return value.attempt == body.attempt;
              });
          if (!opened) {
            return Result<void>::failure({ErrorCode::conflict});
          }
        } else if constexpr (std::is_same_v<T, AttemptOpenEvent> ||
                             std::is_same_v<T, AttemptObservationEvent>) {
          const auto *admission =
              find_event<AttemptAdmissionEvent>(snapshot.facts, [&](const auto &value) {
                return value.attempt == body.attempt;
              });
          const auto *opened =
              find_event<AttemptOpenEvent>(snapshot.facts, [&](const auto &value) {
                return value.attempt == body.attempt;
              });
          const auto *observation = find_event<AttemptObservationEvent>(
              snapshot.facts,
              [&](const auto &value) { return value.attempt == body.attempt; });
          if (!admission ||
              (observation && observation->phase == AttemptPhase::terminal)) {
            return Result<void>::failure({ErrorCode::conflict});
          }
          if constexpr (std::is_same_v<T, AttemptOpenEvent>) {
            if (opened) {
              return Result<void>::failure({ErrorCode::conflict});
            }
          } else {
            if (!opened && !(body.phase == AttemptPhase::terminal &&
                             (body.disposition == AttemptDisposition::cancellation ||
                              body.disposition == AttemptDisposition::unknown))) {
              return Result<void>::failure({ErrorCode::conflict});
            }
            if (observation && observation->phase == AttemptPhase::settling &&
                body.phase == AttemptPhase::running) {
              return Result<void>::failure({ErrorCode::conflict});
            }
          }
        }
        return Result<void>::success();
      },
      event.body);
  if (!validated.has_value()) {
    return validated;
  }
  snapshot.facts.push_back({record, std::move(event), evidence});
  return Result<void>::success();
}

Result<void> RetainedState::rebuild() {
  try {
    auto &staging = prepared_.emplace(Snapshot{});
    staging.issuer_namespace = journal_->header().issuer_namespace;
    return replay(staging, *journal_);
  } catch (const std::bad_alloc &) {
    return Result<void>::failure({ErrorCode::allocation});
  }
}
Result<void> RetainedState::replay(Snapshot &staging, FramedJournal &journal,
                                   std::span<const SourceReference> capture_only,
                                   std::uint64_t maintenance_end) {
  try {
    const auto records = journal.staged_records();
    std::size_t first = 0;
    while (first < records.size()) {
      std::size_t end = first + 1;
      while (end < records.size() &&
             records[end].batch_first == records[first].batch_first) {
        ++end;
      }
      auto candidate = staging;
      for (std::size_t index = first; index < end; ++index) {
        if (records[index].kind == FrameKind::source) {
          const SourceReference reference{records[index].journal,
                                          records[index].sequence};
          if (std::find(capture_only.begin(), capture_only.end(), reference) !=
              capture_only.end())
            continue;
          if (!has_room(candidate, 1))
            return Result<void>::failure({ErrorCode::capacity});
          const auto checked = journal.read_payload(records[index]);
          if (!checked.has_value()) {
            return Result<void>::failure(checked.error());
          }
          candidate.sources.push_back(records[index]);
        }
      }
      std::optional<Error> invalid;
      for (std::size_t index = first; index < end; ++index) {
        if (records[index].kind != FrameKind::semantic ||
            records[index].sequence <= maintenance_end) {
          continue;
        }
        auto payload = journal.read_payload(records[index]);
        if (!payload.has_value()) {
          return Result<void>::failure(payload.error());
        }
        auto decoded =
            decode_retained_event(payload.value(), journal.header().limits.max_payload);
        if (!decoded.has_value()) {
          if (decoded.error().code == ErrorCode::allocation) {
            return Result<void>::failure(decoded.error());
          }
          invalid = decoded.error();
          break;
        }
        const auto applied = apply(candidate, std::move(decoded).value(),
                                   {records[index].journal, records[index].sequence},
                                   RetainedEvidence::recovered_pending);
        if (!applied.has_value()) {
          if (applied.error().code == ErrorCode::capacity)
            return applied;
          invalid = applied.error();
          break;
        }
      }
      if (invalid) {
        const auto rejected =
            journal.reject_recovery_batch(records[first].batch_first, *invalid);
        if (!rejected.has_value()) {
          return rejected;
        }
        break;
      }
      staging.swap(candidate);
      first = end;
    }
    for (const auto &record : journal.pending_records()) {
      if (record.kind != FrameKind::semantic) {
        continue;
      }
      auto bytes = journal.read_payload(record);
      if (!bytes.has_value()) {
        return Result<void>::failure(bytes.error());
      }
      auto decoded =
          decode_retained_event(bytes.value(), journal.header().limits.max_payload);
      if (decoded.has_value()) {
        if (const auto *reservation =
                std::get_if<IssuerReservationEvent>(&decoded.value().body)) {
          if (journal.header().issuer_namespace == pending_namespace_)
            pending_counter_ = std::max(pending_counter_, reservation->counter);
        }
      } else if (decoded.error().code == ErrorCode::allocation) {
        return Result<void>::failure(decoded.error());
      }
    }
    return Result<void>::success();
  } catch (const std::bad_alloc &) {
    return Result<void>::failure({ErrorCode::allocation});
  }
}

RetainedState::SettlementScope::~SettlementScope() {
  owner_.settlement_credit_.reset();
}
Result<std::unique_ptr<RetainedState::SettlementScope>>
RetainedState::protect_settlement(JournalCapacity credit) {
  using Outcome = Result<std::unique_ptr<SettlementScope>>;
  if (in_transaction_ || settlement_credit_)
    return Outcome::failure({ErrorCode::busy});
  if (state() != JournalWriterState::live)
    return Outcome::failure({ErrorCode::audit_unavailable});
  if (credit.max_file_bytes == 0 || credit.max_records == 0)
    return Outcome::failure({ErrorCode::invalid_range});
  const auto usage = journal_usage();
  if (usage.extent_error)
    return Outcome::failure(*usage.extent_error);
  const auto bytes = usage.remaining_bytes();
  if (!bytes || credit.max_file_bytes > *bytes ||
      credit.max_records > usage.remaining_records())
    return Outcome::failure({ErrorCode::capacity});
  try {
    auto scope = std::unique_ptr<SettlementScope>{new SettlementScope{*this}};
    settlement_credit_ = credit;
    return Outcome::success(std::move(scope));
  } catch (const std::bad_alloc &) {
    return Outcome::failure({ErrorCode::allocation});
  }
}
Result<Submission> RetainedState::submit_settlement(const RetainedEvent &event) {
  const auto *observation = std::get_if<AttemptObservationEvent>(&event.body);
  if (!std::holds_alternative<AdapterReceiptEvent>(event.body) &&
      !(observation && observation->phase == AttemptPhase::terminal))
    return Result<Submission>::failure({ErrorCode::invalid_range});
  return submit_impl(event, true);
}

Result<JournalCursor> RetainedState::append(JournalCursor expected,
                                            std::span<const ByteView> sources,
                                            std::span<const RetainedEvent> events) {
  return append_impl(expected, sources, events, false);
}
Result<JournalCursor> RetainedState::append_impl(JournalCursor expected,
                                                 std::span<const ByteView> sources,
                                                 std::span<const RetainedEvent> events,
                                                 bool diagnostic, bool settlement) {
  if (in_transaction_) {
    return Result<JournalCursor>::failure({ErrorCode::busy});
  }
  if (state() != JournalWriterState::live) {
    return Result<JournalCursor>::failure({ErrorCode::audit_unavailable});
  }
  const auto current = cursor();
  const bool stale = current.journal != expected.journal ||
                     current.sequence != expected.sequence ||
                     current.end_offset != expected.end_offset;
  if (sources.empty() && events.empty()) {
    return Result<JournalCursor>::failure({ErrorCode::invalid_range});
  }
  if (journal_->physical_records().size() > capacity_.max_records)
    return Result<JournalCursor>::failure({ErrorCode::capacity});
  const auto remaining = capacity_.max_records - journal_->physical_records().size();
  if (sources.size() > remaining || events.size() > remaining - sources.size()) {
    return Result<JournalCursor>::failure({ErrorCode::capacity});
  }
  const auto credit = settlement_credit_.value_or(JournalCapacity{0, 0});
  const auto count = sources.size() + events.size(); // Sum already bounded above.
  if (settlement_credit_) {
    if (settlement ? count > credit.max_records
                   : credit.max_records > remaining - count)
      return Result<JournalCursor>::failure({ErrorCode::capacity});
  }
  try {
    std::vector<std::vector<std::byte>> encoded;
    encoded.reserve(events.size());
    for (const auto &event : events) {
      auto bytes = encode_retained_event(event, journal_->header().limits.max_payload);
      if (!bytes.has_value()) {
        return Result<JournalCursor>::failure(bytes.error());
      }
      encoded.push_back(std::move(bytes).value());
    }
    if (stale) {
      if (!diagnostic) {
        const auto captured = retain_rejection(expected, sources, encoded,
                                               {ErrorCode::conflict}, nullptr);
        if (!captured.has_value()) {
          return Result<JournalCursor>::failure(captured.error());
        }
      }
      return Result<JournalCursor>::failure({ErrorCode::conflict});
    }
    if (sources.size() >= UINT64_MAX - current.sequence ||
        events.size() >= UINT64_MAX - current.sequence - sources.size()) {
      return Result<JournalCursor>::failure({ErrorCode::overflow});
    }
    auto candidate = committed_;
    auto offset = current.end_offset;
    auto sequence = current.sequence + 1;
    std::size_t batch_bytes = 56;
    const auto limits = journal_->header().limits;
    std::vector<JournalDraft> drafts;
    const auto add_size = [&](std::size_t size) {
      if (size > limits.max_payload || size > limits.max_batch_bytes - 32 ||
          batch_bytes > limits.max_batch_bytes - 32 - size) {
        return false;
      }
      batch_bytes += 32 + size;
      return true;
    };
    for (const auto source : sources) {
      if (!has_room(candidate, 1))
        return Result<JournalCursor>::failure({ErrorCode::capacity});
      if (!add_size(source.size())) {
        return Result<JournalCursor>::failure({ErrorCode::capacity});
      }
      candidate.sources.push_back({journal_->header().environment, current.journal,
                                   FrameKind::source, sequence++, current.sequence + 1,
                                   offset + journal_frame_header_size,
                                   static_cast<std::uint32_t>(source.size())});
      offset += journal_frame_header_size + source.size();
      drafts.push_back({FrameKind::source, source});
    }
    std::optional<Error> invalid;
    const RetainedEvent *conflict = nullptr;
    for (std::size_t index = 0; index < events.size(); ++index) {
      if (!add_size(encoded[index].size())) {
        return Result<JournalCursor>::failure({ErrorCode::capacity});
      }
      const auto applied =
          apply(candidate, events[index], {current.journal, sequence++},
                RetainedEvidence::uncertain);
      if (!applied.has_value()) {
        invalid = applied.error();
        const auto *previous = existing(candidate, events[index]);
        if (!previous)
          previous = existing_uncertain(candidate, events[index]);
        if (previous && previous->event != events[index])
          conflict = &events[index];
        break;
      }
      drafts.push_back({FrameKind::semantic, encoded[index]});
    }
    if (invalid) {
      if (invalid->code == ErrorCode::capacity)
        return Result<JournalCursor>::failure(*invalid);
      if (!diagnostic) {
        const auto retained =
            retain_rejection(expected, sources, encoded, *invalid, conflict);
        if (!retained.has_value()) {
          return Result<JournalCursor>::failure(retained.error());
        }
      }
      return Result<JournalCursor>::failure(*invalid);
    }
    if (current.end_offset > capacity_.max_file_bytes ||
        batch_bytes > capacity_.max_file_bytes - current.end_offset)
      return Result<JournalCursor>::failure({ErrorCode::capacity});
    if (settlement_credit_) {
      const auto bytes_left = capacity_.max_file_bytes - current.end_offset;
      if (settlement ? batch_bytes > credit.max_file_bytes
                     : credit.max_file_bytes > bytes_left - batch_bytes)
        return Result<JournalCursor>::failure({ErrorCode::capacity});
    }
    if (!diagnostic) {
      auto proposal = encode_retained_proposal(expected, sources, encoded,
                                               capacity_.max_file_bytes);
      if (!proposal.has_value()) {
        return Result<JournalCursor>::failure(proposal.error());
      }
      const auto length = proposal.value().size();
      pending_proposal_.emplace(
          PendingProposal{{current.journal, current.sequence + 1, length,
                           CapturePurpose::ordinary_submission},
                          std::move(proposal).value()});
    }
    TransactionGuard mutation{in_transaction_};
    const auto source_base = committed_.sources.size();
    const auto physical_base = journal_->physical_records().size();
    prepared_ = std::move(candidate);
    const auto written = journal_->append(drafts);
    if (!written.has_value()) {
      if (state() != JournalWriterState::poisoned) {
        prepared_.reset();
        if (!diagnostic) {
          pending_proposal_.reset();
        }
      }
      return written;
    }
    for (auto &fact : prepared_->facts) {
      if (fact.evidence == RetainedEvidence::uncertain) {
        fact.evidence = RetainedEvidence::live;
      }
    }
    const auto physical = journal_->physical_records();
    for (std::size_t index = 0; index < sources.size(); ++index) {
      prepared_->sources[source_base + index] = physical[physical_base + index];
    }
    if (settlement && settlement_credit_) {
      settlement_credit_->max_file_bytes -= batch_bytes;
      settlement_credit_->max_records -= count;
    }
    committed_.swap(*prepared_);
    prepared_.reset();
    if (!diagnostic) {
      pending_proposal_.reset();
    }
    return written;
  } catch (const std::bad_alloc &) {
    return Result<JournalCursor>::failure({ErrorCode::allocation});
  }
}

Result<void>
RetainedState::retain_rejection(JournalCursor expected,
                                std::span<const ByteView> sources,
                                std::span<const std::vector<std::byte>> events,
                                Error reason, const RetainedEvent *conflict) {
  // Exact diagnostic proposal: ARPROP01, expected journal/sequence/end,
  // source count + length/bytes, then event count + length/encoded event bytes.
  bool prefix_retained = false;
  try {
    const auto limits = journal_->header().limits;
    const auto chunk_limit = limits.max_payload;
    RetainedBody body = RejectedSubmissionEvent{reason};
    if (conflict) {
      const auto identity = key(conflict->body);
      if (identity && (identity->kind == RetainedKind::decision ||
                       identity->kind == RetainedKind::invocation ||
                       identity->kind == RetainedKind::attempt_admitted)) {
        body = IdentityConflictEvent{identity->kind, identity->identity, {}};
      }
    }
    std::array retained{RetainedEvent{{}, std::move(body)}};
    const auto empty_marker = encode_retained_event(retained[0], chunk_limit);
    if (!empty_marker.has_value()) {
      return Result<void>::failure(empty_marker.error());
    }
    const auto max_references = (chunk_limit - empty_marker.value().size()) / 24;
    const auto limit = std::min<std::uint64_t>(
        capacity_.max_file_bytes,
        static_cast<std::uint64_t>(max_references) * chunk_limit);
    auto serialized = encode_retained_proposal(expected, sources, events, limit);
    if (!serialized.has_value()) {
      return Result<void>::failure(serialized.error());
    }
    auto proposal = std::move(serialized).value();
    std::vector<ByteView> chunks;
    std::vector<SourceReference> references;
    const auto current = cursor();
    for (std::size_t offset = 0; offset < proposal.size();) {
      const auto count = std::min<std::size_t>(chunk_limit, proposal.size() - offset);
      references.push_back({current.journal, 1});
      chunks.push_back(ByteView{proposal}.subspan(offset, count));
      offset += count;
    }
    retained[0].dependencies = std::move(references);
    const auto marker = encode_retained_event(retained[0], chunk_limit);
    if (!marker.has_value()) {
      return Result<void>::failure(marker.error());
    }
    const auto source_bytes = static_cast<std::uint64_t>(proposal.size()) +
                              journal_frame_header_size * chunks.size();
    const auto marker_bytes = journal_frame_header_size + marker.value().size() + 56;
    const bool together = source_bytes + marker_bytes <= limits.max_batch_bytes;
    std::vector<std::size_t> groups;
    if (!together) {
      std::size_t count = 0;
      std::size_t batch_bytes = 56;
      for (const auto chunk : chunks) {
        const auto frame_bytes = journal_frame_header_size + chunk.size();
        if (frame_bytes > limits.max_batch_bytes - batch_bytes) {
          groups.push_back(count);
          count = 0;
          batch_bytes = 56;
        }
        ++count;
        batch_bytes += frame_bytes;
      }
      groups.push_back(count);
    }
    const auto frames = chunks.size() + groups.size() + 2;
    if (frames > UINT64_MAX - current.sequence) {
      return Result<void>::failure({ErrorCode::overflow});
    }
    const auto remaining = capacity_.max_records - journal_->physical_records().size();
    const auto total_bytes = source_bytes + marker_bytes + 56 * groups.size();
    if (chunks.size() >= remaining ||
        total_bytes > capacity_.max_file_bytes - current.end_offset) {
      return Result<void>::failure({ErrorCode::capacity});
    }
    auto sequence = current.sequence + 1;
    // Views into proposal remain valid when its allocation moves into the owner.
    // Inner source-only/marker append_impl calls never replace this outer packet.
    pending_proposal_.emplace(PendingProposal{
        {current.journal, sequence, proposal.size(), CapturePurpose::rejected_proposal},
        std::move(proposal)});
    if (together) {
      for (auto &reference : retained[0].dependencies) {
        reference.sequence = sequence++;
      }
      const auto result = append_impl(current, chunks, retained, true);
      if (result.has_value() || state() != JournalWriterState::poisoned) {
        pending_proposal_.reset();
      }
      return result.has_value() ? Result<void>::success()
                                : Result<void>::failure(result.error());
    }
    std::size_t index = 0;
    for (const auto count : groups) {
      for (std::size_t offset = 0; offset < count; ++offset) {
        retained[0].dependencies[index++].sequence = sequence++;
      }
      ++sequence; // Every intermediate commit consumes a sequence too.
    }
    index = 0;
    for (const auto count : groups) {
      const auto result =
          append_impl(cursor(), std::span{chunks}.subspan(index, count), {}, true);
      if (!result.has_value()) {
        recording_failed_ = prefix_retained;
        if (!prefix_retained && state() != JournalWriterState::poisoned) {
          pending_proposal_.reset();
        }
        return Result<void>::failure(result.error());
      }
      prefix_retained = true;
      index += count;
    }
    const auto result = append_impl(cursor(), {}, retained, true);
    if (!result.has_value()) {
      recording_failed_ = true;
    } else {
      pending_proposal_.reset();
    }
    return result.has_value() ? Result<void>::success()
                              : Result<void>::failure(result.error());
  } catch (const std::bad_alloc &) {
    if (prefix_retained) {
      recording_failed_ = true;
    } else if (state() != JournalWriterState::poisoned) {
      pending_proposal_.reset();
    }
    return Result<void>::failure({ErrorCode::allocation});
  }
}

Result<Submission> RetainedState::submit(const RetainedEvent &event) {
  return submit_impl(event, false);
}
Result<Submission> RetainedState::submit_impl(const RetainedEvent &event,
                                              bool settlement) {
  if (const auto *previous = existing(visible(), event);
      previous && previous->event == event) {
    return Result<Submission>::success({true, previous->record, previous->evidence});
  }
  if (const auto *previous = existing_uncertain(visible(), event);
      previous && previous->event == event) {
    return Result<Submission>::success(
        {true, previous->record, RetainedEvidence::uncertain});
  }
  try {
    const auto current = cursor();
    const std::array events{event};
    const auto written = append_impl(current, {}, events, false, settlement);
    if (!written.has_value()) {
      return Result<Submission>::failure(written.error());
    }
    return Result<Submission>::success(
        {false, {current.journal, current.sequence + 1}, RetainedEvidence::live});
  } catch (const std::bad_alloc &) {
    return Result<Submission>::failure({ErrorCode::allocation});
  }
}
Result<AttemptState> RetainedState::attempt(OperationAttemptId identity) const {
  try {
    const auto &snapshot = visible();
    for (const auto &fact : snapshot.facts) {
      const auto *admitted = std::get_if<AttemptAdmissionEvent>(&fact.event.body);
      if (!admitted || admitted->attempt != identity) {
        continue;
      }
      const auto *opened = find_event<AttemptOpenEvent>(
          snapshot.facts, [&](const auto &value) { return value.attempt == identity; });
      const auto *observed = find_event<AttemptObservationEvent>(
          snapshot.facts, [&](const auto &value) { return value.attempt == identity; });
      auto evidence = fact.evidence;
      bool uncertain_observation = false;
      for (const auto &related : snapshot.facts) {
        const bool matches = std::visit(
            [&](const auto &event) {
              using T = std::decay_t<decltype(event)>;
              if constexpr (std::is_same_v<T, AttemptOpenEvent> ||
                            std::is_same_v<T, AttemptObservationEvent> ||
                            std::is_same_v<T, AdapterReceiptEvent>) {
                return event.attempt == identity;
              }
              return false;
            },
            related.event.body);
        if (!matches) {
          continue;
        }
        if (related.evidence == RetainedEvidence::uncertain ||
            (evidence == RetainedEvidence::live &&
             related.evidence != RetainedEvidence::live)) {
          evidence = related.evidence;
        }
        if (std::get_if<AttemptObservationEvent>(&related.event.body) == observed) {
          uncertain_observation = related.evidence == RetainedEvidence::uncertain;
        }
      }
      const bool terminal = observed && !uncertain_observation &&
                            observed->phase == AttemptPhase::terminal;
      const auto *receipt = find_event<AdapterReceiptEvent>(
          snapshot.facts, [&](const auto &value) { return value.attempt == identity; });
      return Result<AttemptState>::success(
          {*admitted, opened != nullptr,
           observed ? std::optional<AttemptObservationEvent>{*observed} : std::nullopt,
           receipt ? std::optional<AdapterReceiptEvent>{*receipt} : std::nullopt,
           evidence,
           (!reconciled_ || recording_failed_ ||
            evidence == RetainedEvidence::uncertain) &&
               !terminal});
    }
    return Result<AttemptState>::failure({ErrorCode::stale_handle});
  } catch (const std::bad_alloc &) {
    return Result<AttemptState>::failure({ErrorCode::allocation});
  }
}
Result<DispatchReport> RetainedState::dispatch(OperationAttemptId identity,
                                               EffectBoundary &boundary) {
  if (state() != JournalWriterState::live || in_transaction_) {
    return Result<DispatchReport>::failure({ErrorCode::audit_unavailable});
  }
  auto checked = attempt(identity);
  if (!checked.has_value()) {
    return Result<DispatchReport>::failure(checked.error());
  }
  auto owned = std::move(checked).value();
  if (owned.opened || owned.evidence != RetainedEvidence::live ||
      (owned.observation && owned.observation->phase == AttemptPhase::terminal)) {
    return Result<DispatchReport>::success(
        {false, Result<void>::success(), Result<void>::success()});
  }
  const auto *checkpoint =
      find_event<DecisionEvent>(committed_.facts, [&](const auto &value) {
        return value.decision == owned.admission.decision;
      });
  if (!checkpoint) {
    return Result<DispatchReport>::failure({ErrorCode::corrupt});
  }
  const auto actor = checkpoint->actor;
  const auto admitted = submit({{}, AttemptOpenEvent{identity}});
  if (!admitted.has_value()) {
    return Result<DispatchReport>::failure(admitted.error());
  }
  // Open is published before calling the adapter: nested calls and restart can
  // never repeat this attempt. Input is owned across any adapter reentry.
  const auto effect =
      boundary.dispatch({journal_->header().environment, actor,
                         owned.admission.invocation, identity, owned.admission.input});
  const auto recorded = submit_settlement(
      {{},
       AdapterReceiptEvent{identity, effect.has_value()
                                         ? std::nullopt
                                         : std::optional<Error>{effect.error()}}});
  if (!recorded.has_value()) {
    recording_failed_ = true;
  }
  return Result<DispatchReport>::success(
      {true, effect,
       recorded.has_value() ? Result<void>::success()
                            : Result<void>::failure(recorded.error())});
}
Result<std::vector<std::byte>> RetainedState::source(SourceReference reference) {
  for (const auto &record : committed_.sources) {
    if (record.journal == reference.journal && record.sequence == reference.sequence) {
      auto *origin = segment(reference.journal);
      return origin
                 ? origin->read_payload(record)
                 : Result<std::vector<std::byte>>::failure({ErrorCode::stale_handle});
    }
  }
  for (const auto &record : visible().sources) {
    if (record.journal == reference.journal && record.sequence == reference.sequence) {
      return Result<std::vector<std::byte>>::failure({ErrorCode::audit_unavailable});
    }
  }
  return Result<std::vector<std::byte>>::failure({ErrorCode::stale_handle});
}
Result<void> RetainedState::confirm_recovery() {
  if (state() != JournalWriterState::recovery_pending || !prepared_) {
    return Result<void>::failure({ErrorCode::audit_unavailable});
  }
  const auto synchronized = journal_->confirm_recovery();
  if (!synchronized.has_value()) {
    return synchronized;
  }
  for (auto &fact : prepared_->facts) {
    fact.evidence = RetainedEvidence::recovered;
  }
  committed_.swap(*prepared_);
  prepared_.reset();
  return Result<void>::success();
}
Result<void> RetainedState::reconcile(CustodyVerifier &verifier) {
  if (state() != JournalWriterState::recovered) {
    return Result<void>::failure({ErrorCode::audit_unavailable});
  }
  try {
    std::vector<AttemptState> attempts;
    for (const auto &fact : committed_.facts) {
      if (const auto *admitted = std::get_if<AttemptAdmissionEvent>(&fact.event.body)) {
        auto inspected = attempt(admitted->attempt);
        if (!inspected.has_value()) {
          return Result<void>::failure(inspected.error());
        }
        if (inspected.value().reconciliation_required) {
          attempts.push_back(std::move(inspected).value());
        }
      }
    }
    const auto verified = verifier.verify(attempts);
    if (!verified.has_value()) {
      return verified;
    }
    const auto resumed = journal_->resume_after_reconciliation();
    if (resumed.has_value()) {
      reconciled_ = true;
    }
    return resumed;
  } catch (const std::bad_alloc &) {
    return Result<void>::failure({ErrorCode::allocation});
  }
}
std::uint64_t RetainedState::issuer_counter() const noexcept {
  return std::max(pending_namespace_ == visible().issuer_namespace ? pending_counter_
                                                                   : 0,
                  visible().counter);
}
Result<IdentityBytes> RetainedState::reserve_identity() {
  if (issuer_counter() == UINT64_MAX) {
    return Result<IdentityBytes>::failure({ErrorCode::overflow});
  }
  const auto counter = issuer_counter() + 1;
  const auto reserved = submit({{}, IssuerReservationEvent{counter}});
  if (!reserved.has_value()) {
    return Result<IdentityBytes>::failure(reserved.error());
  }
  IdentityBytes bytes{};
  auto ns = journal_->header().issuer_namespace;
  auto value = counter;
  for (std::size_t index = 0; index < 8; ++index) {
    bytes[index] = static_cast<std::byte>(ns & 255);
    bytes[index + 8] = static_cast<std::byte>(value & 255);
    ns >>= 8;
    value >>= 8;
  }
  return Result<IdentityBytes>::success(bytes);
}
} // namespace arconaut
