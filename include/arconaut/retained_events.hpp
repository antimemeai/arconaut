#pragma once

#include "arconaut/journal.hpp"

namespace arconaut {

struct DecisionTag;
struct ComplaintTag;
using DecisionId = Id<DecisionTag>;
using ComplaintId = Id<ComplaintTag>;
struct ApplicationRecordTag;
using ApplicationRecordId = Id<ApplicationRecordTag>;
enum class ApplicationChannel : std::uint16_t { log = 1, context = 2, program = 3 };
struct ApplicationRecordEvent {
  ApplicationRecordId identity;
  ApplicationChannel channel;
  std::vector<std::byte> payload;
  bool operator==(const ApplicationRecordEvent &) const = default;
};
struct SourceReference {
  AuditStreamId journal;
  std::uint64_t sequence;
  bool operator==(const SourceReference &) const = default;
};
enum class RetainedKind : std::uint16_t {
  reservation = 1,
  decision = 2,
  invocation = 3,
  attempt_admitted = 4,
  attempt_open = 5,
  observation = 6,
  retry = 7,
  identity_conflict = 8,
  complaint = 9,
  rejected_submission = 10,
  adapter_receipt = 11,
  recovery_choice = 12,
  provisional_capture = 13,
  application = 14
};
struct IssuerReservationEvent {
  std::uint64_t counter;
  bool operator==(const IssuerReservationEvent &) const = default;
};
struct DecisionEvent {
  DecisionId decision;
  ParticipantId actor;
  ConversationId conversation;
  WorkflowId workflow;
  DefinitionGenerationId definition;
  ContextRevisionId context;
  std::vector<InvocationId> planned_invocations;
  std::vector<std::byte> continuation;
  bool operator==(const DecisionEvent &) const = default;
};
struct InvocationEvent {
  InvocationId invocation;
  DecisionId decision;
  DefinitionGenerationId definition;
  std::vector<std::byte> input;
  bool operator==(const InvocationEvent &) const = default;
};
struct AttemptAdmissionEvent {
  OperationAttemptId attempt;
  InvocationId invocation;
  DecisionId decision;
  std::vector<std::byte> input;
  bool operator==(const AttemptAdmissionEvent &) const = default;
};
struct AttemptOpenEvent {
  OperationAttemptId attempt;
  bool operator==(const AttemptOpenEvent &) const = default;
};
enum class AttemptPhase : std::uint8_t { running = 1, settling = 2, terminal = 3 };
enum class AttemptDisposition : std::uint8_t {
  none = 0,
  success = 1,
  failure = 2,
  cancellation = 3,
  unknown = 4
};
struct AttemptObservationEvent {
  OperationAttemptId attempt;
  AttemptPhase phase;
  AttemptDisposition disposition;
  std::vector<std::byte> observation;
  bool operator==(const AttemptObservationEvent &) const = default;
};
struct RetryEvent {
  DecisionId decision;
  InvocationId invocation;
  OperationAttemptId attempt;
  bool operator==(const RetryEvent &) const = default;
};
struct IdentityConflictEvent {
  RetainedKind disputed_kind;
  IdentityBytes disputed_identity;
  std::vector<std::byte> proposal;
  bool operator==(const IdentityConflictEvent &) const = default;
};
struct ComplaintEvent {
  ComplaintId complaint;
  ParticipantId actor;
  std::vector<std::byte> detail;
  bool operator==(const ComplaintEvent &) const = default;
};
struct RejectedSubmissionEvent {
  Error reason;
  bool operator==(const RejectedSubmissionEvent &) const = default;
};
struct AdapterReceiptEvent {
  OperationAttemptId attempt;
  std::optional<Error> failure;
  bool operator==(const AdapterReceiptEvent &) const = default;
};
enum class RetainedEvidence : std::uint8_t {
  live,
  recovered_pending,
  recovered,
  uncertain
};
enum class CapturePurpose : std::uint16_t {
  ordinary_submission = 1,
  rejected_proposal = 2
};
struct ProvisionalCaptureEvent {
  AuditStreamId origin;
  std::uint64_t first_sequence;
  std::uint64_t proposal_length;
  CapturePurpose purpose;
  bool operator==(const ProvisionalCaptureEvent &) const = default;
};
struct CheckedAttempt {
  OperationAttemptId attempt;
  bool opened;
  std::optional<AttemptPhase> observed_phase;
  AttemptDisposition disposition;
  RetainedEvidence prior_evidence;
  bool operator==(const CheckedAttempt &) const = default;
};
// Wire declarations describe policy; replay never uses untrusted capacities to
// choose an allocation bound. The selected-chain owner validates actual limits.
struct RetainedCapacityDeclaration {
  std::uint64_t max_file_bytes;
  std::uint64_t max_records;
  bool operator==(const RetainedCapacityDeclaration &) const = default;
};
struct RecoveryChoiceEvent {
  JournalPredecessor predecessor;
  std::uint64_t diagnostic_offset;
  std::uint64_t available_end;
  JournalLimits old_limits;
  RetainedCapacityDeclaration old_capacity;
  RetainedCapacityDeclaration new_capacity;
  std::optional<Error> problem;
  std::vector<CheckedAttempt> checked_attempts;
  std::vector<ProvisionalCaptureEvent> required_captures;
  bool operator==(const RecoveryChoiceEvent &) const = default;
};
using RetainedBody =
    std::variant<IssuerReservationEvent, DecisionEvent, InvocationEvent,
                 AttemptAdmissionEvent, AttemptOpenEvent, AttemptObservationEvent,
                 RetryEvent, IdentityConflictEvent, ComplaintEvent,
                 RejectedSubmissionEvent, AdapterReceiptEvent, RecoveryChoiceEvent,
                 ProvisionalCaptureEvent, ApplicationRecordEvent>;
struct RetainedEvent {
  std::vector<SourceReference> dependencies;
  RetainedBody body;
  bool operator==(const RetainedEvent &) const = default;
};

RetainedKind retained_kind(const RetainedBody &body) noexcept;
// Owned, exact wire records. Successful decoding does not validate a ledger
// transition or authorize a source dependency. RetainedState owns root checks;
// the selected-chain owner alone validates maintenance placement and capture sets.
Result<std::vector<std::byte>> encode_retained_event(const RetainedEvent &event,
                                                     std::uint32_t max_payload);
Result<RetainedEvent> decode_retained_event(ByteView bytes, std::uint32_t max_payload);

} // namespace arconaut
