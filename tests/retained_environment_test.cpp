#include "arconaut/retained_environment.hpp"
#include "arconaut/retained_proposal.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <new>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace {
std::atomic<std::ptrdiff_t> allocation_cut{-1};
}
void *operator new(std::size_t size) {
  if (allocation_cut.load() >= 0 && allocation_cut.fetch_sub(1) == 0)
    throw std::bad_alloc{};
  if (void *result = std::malloc(size == 0 ? 1 : size))
    return result;
  throw std::bad_alloc{};
}
void operator delete(void *pointer) noexcept { std::free(pointer); }
void operator delete(void *pointer, std::size_t) noexcept { std::free(pointer); }
void *operator new[](std::size_t size) { return ::operator new(size); }
void operator delete[](void *pointer) noexcept { ::operator delete(pointer); }
void operator delete[](void *pointer, std::size_t) noexcept {
  ::operator delete(pointer);
}

using namespace arconaut;
namespace {
void check(bool condition, const char *expression, int line) {
  if (!condition)
    throw std::runtime_error(std::to_string(line) + ": " + expression);
}
#define CHECK(...) check((__VA_ARGS__), #__VA_ARGS__, __LINE__)
template <typename T> T require(Result<T> result) {
  CHECK(result.has_value());
  return std::move(result).value();
}
void require(Result<void> result) { CHECK(result.has_value()); }
template <typename T> void error_is(const Result<T> &result, ErrorCode code) {
  CHECK(!result.has_value());
  CHECK(result.error().code == code);
}
template <typename T> T id(unsigned char value) {
  IdentityBytes bytes{};
  bytes[15] = std::byte{value};
  return require(T::from_bytes(bytes));
}
struct TemporaryDirectory {
  TemporaryDirectory() {
    std::array<char, 48> pattern{};
    const std::string text = "/tmp/arconaut-environment-XXXXXX";
    std::copy(text.begin(), text.end(), pattern.begin());
    CHECK(::mkdtemp(pattern.data()) != nullptr);
    path = pattern.data();
  }
  ~TemporaryDirectory() {
    std::error_code ignored;
    std::filesystem::remove_all(path, ignored);
  }
  std::string path;
};
std::unique_ptr<JournalDirectory> directory(const TemporaryDirectory &temporary) {
  return std::make_unique<NativeJournalDirectory>(
      require(NativeJournalDirectory::open(temporary.path)));
}
JournalHeader root() {
  return {id<EnvironmentId>(1), id<AuditStreamId>(2), 3, {1024, 4096}, std::nullopt};
}
constexpr JournalCapacity root_capacity{4096, 8};
constexpr EnvironmentBounds bounds{{1024, 4096}, {32768, 64}, 8, 32, 32768};
const std::vector<std::byte> original{std::byte{0}, std::byte{255}, std::byte{10}};
DecisionEvent decision() {
  return {id<DecisionId>(4),
          id<ParticipantId>(5),
          id<ConversationId>(6),
          id<WorkflowId>(7),
          id<DefinitionGenerationId>(8),
          id<ContextRevisionId>(9),
          {id<InvocationId>(10)},
          original};
}
class Custody final : public CustodyVerifier {
public:
  std::size_t calls = 0;
  Result<void> verify(std::span<const AttemptState> attempts) override {
    ++calls;
    CHECK(attempts.empty());
    return Result<void>::success();
  }
};
class Effect final : public EffectBoundary {
public:
  std::size_t calls = 0;
  Result<void> dispatch(const EffectIntent &intent) override {
    ++calls;
    CHECK(intent.attempt == id<OperationAttemptId>(11));
    CHECK(std::equal(intent.input.begin(), intent.input.end(), original.begin(),
                     original.end()));
    return Result<void>::success();
  }
};
void cursor_is(JournalCursor cursor, AuditStreamId journal, std::uint64_t sequence,
               std::uint64_t end) {
  CHECK(cursor.journal == journal && cursor.sequence == sequence &&
        cursor.end_offset == end);
}
void root_owner() {
  TemporaryDirectory temporary;
  auto owner = require(
      RetainedEnvironment::create(directory(temporary), root(), root_capacity, bounds));
  CHECK(owner->state() == JournalWriterState::live);
  const std::array sources{ByteView{original}};
  const std::array events{RetainedEvent{{{root().journal, 1}}, decision()}};
  const auto result = require(owner->submit_proposal(owner->cursor(), sources, events));
  cursor_is(result.cursor, root().journal, 3, 390);
  CHECK(!result.existing && !result.capture && result.events.size() == 1);
  CHECK(!result.events[0].existing && result.events[0].record.sequence == 2 &&
        result.events[0].evidence == RetainedEvidence::live);
  const auto identity = require(owner->issue<DecisionId>());
  IdentityBytes expected{};
  expected[0] = std::byte{3};
  expected[8] = std::byte{1};
  CHECK(identity.bytes() == expected && owner->committed_facts().size() == 2);
  cursor_is(owner->cursor(), root().journal, 5, 494);
  error_is(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds),
      ErrorCode::busy);
  owner.reset();
  owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds));
  CHECK(owner->state() == JournalWriterState::recovery_pending);
  const auto pending_existing =
      require(owner->submit_proposal(owner->cursor(), {}, events));
  CHECK(pending_existing.existing && pending_existing.events[0].existing &&
        pending_existing.events[0].evidence == RetainedEvidence::recovered_pending);
  CHECK(owner->state() == JournalWriterState::recovery_pending &&
        owner->committed_facts().empty());
  require(owner->confirm_recovery());
  CHECK(require(owner->source({root().journal, 1})) == original);
  Custody custody;
  require(owner->reconcile(custody));
  CHECK(custody.calls == 1 && owner->state() == JournalWriterState::live);
  CHECK(require(owner->submit(events[0])).existing);
  cursor_is(owner->cursor(), root().journal, 5, 494);
  require(owner->submit({{{root().journal, 1}},
                         InvocationEvent{id<InvocationId>(10), id<DecisionId>(4),
                                         id<DefinitionGenerationId>(8), original}}));
  require(owner->submit(
      {{{root().journal, 1}},
       AttemptAdmissionEvent{id<OperationAttemptId>(11), id<InvocationId>(10),
                             id<DecisionId>(4), original}}));
  Effect effect;
  CHECK(require(owner->dispatch(id<OperationAttemptId>(11), effect)).dispatched);
  CHECK(!require(owner->dispatch(id<OperationAttemptId>(11), effect)).dispatched);
  CHECK(effect.calls == 1);
}
void duplicate_positions() {
  TemporaryDirectory temporary;
  auto owner = require(
      RetainedEnvironment::create(directory(temporary), root(), root_capacity, bounds));
  const RetainedEvent event{
      {}, ComplaintEvent{id<ComplaintId>(20), id<ParticipantId>(5), original}};
  const std::array events{event, event};
  const auto result = require(owner->submit_proposal(owner->cursor(), {}, events));
  cursor_is(result.cursor, root().journal, 3, 326);
  CHECK(result.events.size() == 2 && !result.events[0].existing &&
        result.events[1].existing);
  CHECK(result.events[0].record == RecordReference{root().journal, 1});
  CHECK(result.events[1].record == result.events[0].record);
  CHECK(owner->committed_facts().size() == 1);
  const auto unchanged = require(owner->submit_proposal(owner->cursor(), {}, events));
  CHECK(unchanged.existing && !unchanged.capture && unchanged.events[0].existing &&
        unchanged.events[1].existing);
  cursor_is(unchanged.cursor, root().journal, 3, 326);
  const RetainedEvent fresh{
      {}, ComplaintEvent{id<ComplaintId>(21), id<ParticipantId>(5), original}};
  const std::array mixed{event, fresh};
  const auto appended = require(owner->submit_proposal(owner->cursor(), {}, mixed));
  CHECK(!appended.existing && appended.events[0].existing &&
        !appended.events[1].existing);
  CHECK(appended.events[0].record == RecordReference{root().journal, 1});
  CHECK(appended.events[1].record == RecordReference{root().journal, 5});
  cursor_is(appended.cursor, root().journal, 6, 540);
  CHECK(owner->committed_facts().size() == 2);
  const auto old_bytes = require(owner->read_original_range(root().journal, 326, 79));
  const auto new_bytes = require(owner->read_original_range(root().journal, 405, 79));
  const auto old_frame = require(decode_journal_frame(old_bytes.bytes, root().limits));
  const auto new_frame = require(decode_journal_frame(new_bytes.bytes, root().limits));
  CHECK(old_frame.kind == FrameKind::semantic && old_frame.sequence == 4 &&
        old_frame.batch_first == 4);
  CHECK(new_frame.kind == FrameKind::semantic && new_frame.sequence == 5 &&
        new_frame.batch_first == 4);
  CHECK(require(decode_retained_event(old_frame.payload, 1024)) == event);
  CHECK(require(decode_retained_event(new_frame.payload, 1024)) == fresh);
}
void captured_empty_predecessor() {
  TemporaryDirectory temporary;
  auto head = require(
      EnvironmentHead::prepare(directory(temporary), root(), SyncStrength::full));
  auto first = require(FramedJournal::create(head->directory(),
                                             "journal.00000000000000000000000000000002",
                                             root(), root_capacity));
  cursor_is(first->cursor(), root().journal, 0, 112);
  require(head->publish_initial(SyncStrength::full));
  const RetainedEvent event{{{root().journal, 1}}, decision()};
  const std::array encoded{require(encode_retained_event(event, 1024))};
  const std::array sources{ByteView{original}};
  const auto packet =
      require(encode_retained_proposal(first->cursor(), sources, encoded, 32768));
  CHECK(packet.size() == 214);
  const ProvisionalCaptureEvent capture{root().journal, 1, 214,
                                        CapturePurpose::ordinary_submission};
  const JournalHeader child{root().environment, id<AuditStreamId>(12), 13,
                            root().limits, JournalPredecessor{root().journal, 0, 112}};
  auto second = require(FramedJournal::create(
      head->directory(), "journal.0000000000000000000000000000000c", child,
      {8192, 16}));
  const RecoveryChoiceEvent choice{*child.predecessor, 112,       112,
                                   root().limits,      {4096, 8}, {8192, 16},
                                   std::nullopt,       {},        {capture}};
  const auto choice_bytes = require(encode_retained_event({{}, choice}, 1024));
  const std::array choose{JournalDraft{FrameKind::semantic, choice_bytes}};
  cursor_is(require(second->append(choose)), child.journal, 2, 352);
  const auto marker =
      require(encode_retained_event({{{child.journal, 3}}, capture}, 1024));
  const std::array captured{JournalDraft{FrameKind::source, packet},
                            JournalDraft{FrameKind::semantic, marker}};
  cursor_is(require(second->append(captured)), child.journal, 5, 754);
  require(head->replace(head->selection(), child.journal, SyncStrength::full));
  second.reset();
  first.reset();
  head.reset();
  auto owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds));
  cursor_is(owner->cursor(), child.journal, 5, 754);
  CHECK(owner->state() == JournalWriterState::recovery_pending);
  CHECK(owner->committed_facts().empty());
  const auto originals = owner->provisional_originals();
  CHECK(originals.size() == 1 &&
        originals[0].marker == RecordReference{child.journal, 4});
  CHECK(originals[0].capture == capture && originals[0].bytes == packet);
  error_is(owner->source({child.journal, 3}), ErrorCode::stale_handle);
  const auto existing = require(owner->submit(event));
  CHECK(existing.existing && existing.record == RecordReference{root().journal, 2} &&
        existing.evidence == RetainedEvidence::uncertain);
  const std::array events{event};
  const auto whole =
      require(owner->submit_proposal({root().journal, 0, 112}, sources, events));
  CHECK(whole.existing && whole.capture == capture && whole.events.size() == 1);
  CHECK(whole.events[0].record == RecordReference{root().journal, 2} &&
        whole.events[0].evidence == RetainedEvidence::uncertain);
  cursor_is(owner->cursor(), child.journal, 5, 754);
  require(owner->confirm_recovery());
  CHECK(owner->committed_facts().size() == 2);
  CHECK(owner->provisional_originals()[0].bytes == packet);
  Custody custody;
  require(owner->reconcile(custody));
  CHECK(owner->state() == JournalWriterState::live && custody.calls == 1);
  CHECK(require(owner->submit(event)).evidence == RetainedEvidence::uncertain);
  const auto query = require(owner->submit_proposal(owner->cursor(), {}, events));
  CHECK(query.existing && query.events[0].evidence == RetainedEvidence::uncertain);
  cursor_is(owner->cursor(), child.journal, 5, 754);
  owner.reset();
  auto small_history = bounds;
  small_history.max_history_entries = 3;
  error_is(RetainedEnvironment::open(directory(temporary), root(), root_capacity,
                                     small_history),
           ErrorCode::capacity);
  auto small_bytes = bounds;
  small_bytes.max_capture_bytes = 213;
  error_is(RetainedEnvironment::open(directory(temporary), root(), root_capacity,
                                     small_bytes),
           ErrorCode::capacity);
  small_bytes.max_capture_bytes = 214;
  owner = require(RetainedEnvironment::open(directory(temporary), root(), root_capacity,
                                            small_bytes));
  require(owner->confirm_recovery());
  require(owner->reconcile(custody));
  Effect effect;
  error_is(owner->dispatch(id<OperationAttemptId>(11), effect),
           ErrorCode::stale_handle);
  CHECK(effect.calls == 0);
  const RetainedEvent fresh{
      {}, ComplaintEvent{id<ComplaintId>(20), id<ParticipantId>(5), original}};
  const std::array mixed{event, fresh};
  error_is(owner->submit_proposal(owner->cursor(), {}, mixed), ErrorCode::conflict);
  CHECK(owner->committed_facts().size() == 3);
  const auto &rejection = owner->committed_facts().back().event;
  CHECK(std::get<RejectedSubmissionEvent>(rejection.body).reason.code ==
        ErrorCode::conflict);
  CHECK(rejection.dependencies.size() == 1);
  const auto outer = require(decode_retained_proposal(
      require(owner->source(rejection.dependencies[0])), 32768));
  CHECK(outer.sources.empty() && outer.events.size() == 2 &&
        outer.events[0] == encoded[0]);
  CHECK(outer.events[1] == require(encode_retained_event(fresh, 1024)));
  CHECK(owner->provisional_originals()[0].bytes == packet);
  const RetainedEvent invocation{
      {},
      InvocationEvent{id<InvocationId>(10), id<DecisionId>(4),
                      id<DefinitionGenerationId>(8), original}};
  error_is(owner->submit(invocation), ErrorCode::conflict);
  CHECK(std::none_of(owner->committed_facts().begin(), owner->committed_facts().end(),
                     [](const auto &fact) {
                       return std::holds_alternative<InvocationEvent>(fact.event.body);
                     }));

  for (const bool changed : {false, true}) {
    auto submitted_source = original;
    if (changed)
      submitted_source[0] = std::byte{1};
    const std::array input_sources{ByteView{submitted_source}};
    const auto expected =
        changed ? JournalCursor{root().journal, 0, 112} : owner->cursor();
    error_is(owner->submit_proposal(expected, input_sources, events),
             ErrorCode::conflict);
    const auto &refused = owner->committed_facts().back().event;
    CHECK(std::holds_alternative<RejectedSubmissionEvent>(refused.body) &&
          refused.dependencies.size() == 1);
    const auto retained = require(decode_retained_proposal(
        require(owner->source(refused.dependencies[0])), 32768));
    CHECK(retained.expected.journal == expected.journal &&
          retained.expected.sequence == expected.sequence);
    CHECK(retained.sources.size() == 1 && retained.sources[0] == submitted_source &&
          retained.events.size() == 1 && retained.events[0] == encoded[0]);
    CHECK(owner->provisional_originals()[0].bytes == packet);
  }
}
enum class CaptureShape { source_only, decision, interrupted_region };
void split_capture_groups() {
  for (const auto shape : {CaptureShape::source_only, CaptureShape::decision,
                           CaptureShape::interrupted_region}) {
    TemporaryDirectory temporary;
    auto head = require(
        EnvironmentHead::prepare(directory(temporary), root(), SyncStrength::full));
    auto first = require(FramedJournal::create(
        head->directory(), "journal.00000000000000000000000000000002", root(),
        root_capacity));
    require(head->publish_initial(SyncStrength::full));
    const std::array sources{ByteView{original}};
    const std::array encoded{
        require(encode_retained_event({{{root().journal, 1}}, decision()}, 1024))};
    const auto event_views = shape == CaptureShape::source_only
                                 ? std::span<const std::vector<std::byte>>{}
                                 : std::span<const std::vector<std::byte>>{encoded};
    const auto packet =
        require(encode_retained_proposal(first->cursor(), sources, event_views, 32768));
    CHECK(packet.size() == (shape == CaptureShape::source_only ? 55U : 214U));
    const ProvisionalCaptureEvent capture{root().journal, 1, packet.size(),
                                          CapturePurpose::ordinary_submission};
    const JournalHeader child{root().environment, id<AuditStreamId>(12), 13,
                              root().limits,
                              JournalPredecessor{root().journal, 0, 112}};
    auto second = require(FramedJournal::create(
        head->directory(), "journal.0000000000000000000000000000000c", child,
        {8192, 16}));
    const RecoveryChoiceEvent choice{*child.predecessor, 112,       112,
                                     root().limits,      {4096, 8}, {8192, 16},
                                     std::nullopt,       {},        {capture}};
    const auto choice_bytes = require(encode_retained_event({{}, choice}, 1024));
    const std::array choose{JournalDraft{FrameKind::semantic, choice_bytes}};
    cursor_is(require(second->append(choose)), child.journal, 2, 352);
    if (shape == CaptureShape::interrupted_region) {
      const auto ordinary = require(encode_retained_event(
          {{}, ComplaintEvent{id<ComplaintId>(20), id<ParticipantId>(5), original}},
          1024));
      const std::array interruption{JournalDraft{FrameKind::semantic, ordinary}};
      cursor_is(require(second->append(interruption)), child.journal, 4, 487);
    }
    const auto split = shape == CaptureShape::source_only ? 20U : 100U;
    const auto first_sequence = second->cursor().sequence + 1;
    const std::array group1{
        JournalDraft{FrameKind::source, ByteView{packet}.first(split)}};
    require(second->append(group1));
    const auto second_sequence = second->cursor().sequence + 1;
    const std::array group2{
        JournalDraft{FrameKind::source, ByteView{packet}.subspan(split)}};
    require(second->append(group2));
    const auto marker = require(encode_retained_event(
        {{{child.journal, first_sequence}, {child.journal, second_sequence}}, capture},
        1024));
    const std::array mark{JournalDraft{FrameKind::semantic, marker}};
    const auto marked = require(second->append(mark));
    if (shape != CaptureShape::interrupted_region)
      cursor_is(marked, child.journal, 8,
                shape == CaptureShape::source_only ? 763U : 922U);
    require(head->replace(head->selection(), child.journal, SyncStrength::full));
    second.reset();
    first.reset();
    head.reset();
    auto opened =
        RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds);
    if (shape == CaptureShape::interrupted_region) {
      error_is(opened, ErrorCode::conflict);
      continue;
    }
    auto owner = require(std::move(opened));
    CHECK(owner->provisional_originals().size() == 1 &&
          owner->provisional_originals()[0].bytes == packet);
    error_is(owner->source({child.journal, 3}), ErrorCode::stale_handle);
    error_is(owner->source({child.journal, 5}), ErrorCode::stale_handle);
    if (shape == CaptureShape::source_only) {
      const auto existing =
          require(owner->submit_proposal({root().journal, 0, 112}, sources, {}));
      CHECK(existing.existing && existing.capture == capture &&
            existing.events.empty());
      cursor_is(existing.cursor, child.journal, 8, 763);
    }
  }
}
void captured_positions_under_narrow_profile() {
  TemporaryDirectory temporary;
  auto head = require(
      EnvironmentHead::prepare(directory(temporary), root(), SyncStrength::full));
  auto first = require(FramedJournal::create(head->directory(),
                                             "journal.00000000000000000000000000000002",
                                             root(), root_capacity));
  auto large_decision = decision();
  large_decision.continuation.assign(600, std::byte{42});
  const RetainedEvent known{{{root().journal, 1}}, large_decision};
  const auto known_bytes = require(encode_retained_event(known, 1024));
  CHECK(known_bytes.size() == 752);
  const std::array authoritative{JournalDraft{FrameKind::source, original},
                                 JournalDraft{FrameKind::semantic, known_bytes}};
  cursor_is(require(first->append(authoritative)), root().journal, 3, 987);
  require(head->publish_initial(SyncStrength::full));
  const RetainedEvent uncertain{
      {}, ComplaintEvent{id<ComplaintId>(20), id<ParticipantId>(5), original}};
  const auto uncertain_bytes = require(encode_retained_event(uncertain, 1024));
  const std::array encoded{known_bytes, uncertain_bytes, uncertain_bytes};
  const auto packet =
      require(encode_retained_proposal(first->cursor(), {}, encoded, 32768));
  CHECK(packet.size() == 906);
  const ProvisionalCaptureEvent capture{root().journal, 4, 906,
                                        CapturePurpose::ordinary_submission};
  const JournalHeader child{root().environment,
                            id<AuditStreamId>(12),
                            13,
                            {256, 4096},
                            JournalPredecessor{root().journal, 3, 987}};
  auto second = require(FramedJournal::create(
      head->directory(), "journal.0000000000000000000000000000000c", child,
      {8192, 16}));
  const RecoveryChoiceEvent choice{*child.predecessor, 987,       987,
                                   root().limits,      {4096, 8}, {8192, 16},
                                   std::nullopt,       {},        {capture}};
  const auto choice_bytes = require(encode_retained_event({{}, choice}, 256));
  const std::array choose{JournalDraft{FrameKind::semantic, choice_bytes}};
  cursor_is(require(second->append(choose)), child.journal, 2, 352);
  const auto marker = require(encode_retained_event(
      {{{child.journal, 3}, {child.journal, 4}, {child.journal, 5}, {child.journal, 6}},
       capture},
      256));
  const std::array captured{
      JournalDraft{FrameKind::source, ByteView{packet}.subspan(0, 250)},
      JournalDraft{FrameKind::source, ByteView{packet}.subspan(250, 250)},
      JournalDraft{FrameKind::source, ByteView{packet}.subspan(500, 250)},
      JournalDraft{FrameKind::source, ByteView{packet}.subspan(750)},
      JournalDraft{FrameKind::semantic, marker}};
  cursor_is(require(second->append(captured)), child.journal, 8, 1614);
  require(head->replace(head->selection(), child.journal, SyncStrength::full));
  second.reset();
  first.reset();
  head.reset();
  auto exact = bounds;
  exact.max_history_entries = 6;
  exact.max_capture_bytes = 906;
  auto owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, exact));
  const auto semantic_known = require(owner->submit(known));
  CHECK(semantic_known.record == RecordReference{root().journal, 2} &&
        semantic_known.evidence == RetainedEvidence::recovered_pending);
  const auto semantic_uncertain = require(owner->submit(uncertain));
  CHECK(semantic_uncertain.record == RecordReference{root().journal, 5} &&
        semantic_uncertain.evidence == RetainedEvidence::uncertain);
  const std::array events{known, uncertain, uncertain};
  const auto captured_result =
      require(owner->submit_proposal({root().journal, 3, 987}, {}, events));
  CHECK(captured_result.existing && captured_result.capture == capture &&
        captured_result.events.size() == 3);
  for (std::size_t i = 0; i < captured_result.events.size(); ++i) {
    CHECK(captured_result.events[i].existing &&
          captured_result.events[i].record == RecordReference{root().journal, 4 + i});
    CHECK(captured_result.events[i].evidence == RetainedEvidence::uncertain);
  }
  cursor_is(owner->cursor(), child.journal, 8, 1614);
  require(owner->confirm_recovery());
  Custody custody;
  require(owner->reconcile(custody));
  CHECK(owner->committed_facts().size() == 3 &&
        require(owner->source({root().journal, 1})) == original);
  CHECK(require(owner->submit(known)).evidence == RetainedEvidence::recovered);
  error_is(owner->source({child.journal, 3}), ErrorCode::stale_handle);
  owner.reset();
  exact.max_history_entries = 5;
  error_is(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, exact),
      ErrorCode::capacity);
}
void rejected_capture_has_no_uncertain_admission() {
  for (const auto purpose :
       {CapturePurpose::ordinary_submission, CapturePurpose::rejected_proposal}) {
    TemporaryDirectory temporary;
    auto head = require(
        EnvironmentHead::prepare(directory(temporary), root(), SyncStrength::full));
    auto first = require(FramedJournal::create(
        head->directory(), "journal.00000000000000000000000000000002", root(),
        root_capacity));
    require(head->publish_initial(SyncStrength::full));
    const std::vector<std::byte> oversized_source(1500, std::byte{42});
    const std::array sources{ByteView{oversized_source}};
    const RetainedEvent admission{{},
                                  AttemptAdmissionEvent{id<OperationAttemptId>(11),
                                                        id<InvocationId>(10),
                                                        id<DecisionId>(4), original}};
    const std::array encoded{require(encode_retained_event(admission, 1024))};
    const auto packet = require(encode_retained_proposal(
        {id<AuditStreamId>(99), UINT64_MAX, 7}, sources, encoded, 32768));
    CHECK(packet.size() == 1619);
    const ProvisionalCaptureEvent capture{root().journal, 1, 1619, purpose};
    const JournalHeader child{root().environment, id<AuditStreamId>(12), 13,
                              root().limits,
                              JournalPredecessor{root().journal, 0, 112}};
    auto second = require(FramedJournal::create(
        head->directory(), "journal.0000000000000000000000000000000c", child,
        {8192, 16}));
    const RecoveryChoiceEvent choice{*child.predecessor, 112,       112,
                                     root().limits,      {4096, 8}, {8192, 16},
                                     std::nullopt,       {},        {capture}};
    const auto choice_bytes = require(encode_retained_event({{}, choice}, 1024));
    const std::array choose{JournalDraft{FrameKind::semantic, choice_bytes}};
    cursor_is(require(second->append(choose)), child.journal, 2, 352);
    const auto marker = require(encode_retained_event(
        {{{child.journal, 3}, {child.journal, 4}}, capture}, 1024));
    const std::array captured{
        JournalDraft{FrameKind::source, ByteView{packet}.first(900)},
        JournalDraft{FrameKind::source, ByteView{packet}.subspan(900)},
        JournalDraft{FrameKind::semantic, marker}};
    cursor_is(require(second->append(captured)), child.journal, 6, 2215);
    require(head->replace(head->selection(), child.journal, SyncStrength::full));
    second.reset();
    first.reset();
    head.reset();
    auto opened =
        RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds);
    if (purpose == CapturePurpose::ordinary_submission) {
      error_is(opened, ErrorCode::conflict);
      continue;
    }
    auto owner = require(std::move(opened));
    CHECK(owner->provisional_originals().size() == 1 &&
          owner->provisional_originals()[0].bytes == packet);
    error_is(owner->attempt(id<OperationAttemptId>(11)), ErrorCode::stale_handle);
    error_is(owner->source({child.journal, 3}), ErrorCode::stale_handle);
    error_is(owner->source({child.journal, 4}), ErrorCode::stale_handle);
    require(owner->confirm_recovery());
    Custody custody;
    require(owner->reconcile(custody));
    CHECK(custody.calls == 1 && owner->committed_facts().size() == 2);
    error_is(owner->submit(admission), ErrorCode::conflict);
    CHECK(owner->committed_facts().size() == 3 &&
          owner->provisional_originals()[0].bytes == packet);
    error_is(owner->attempt(id<OperationAttemptId>(11)), ErrorCode::stale_handle);
  }
}
void captured_reservation_does_not_burn_active_namespace() {
  TemporaryDirectory temporary;
  auto head = require(
      EnvironmentHead::prepare(directory(temporary), root(), SyncStrength::full));
  auto first = require(FramedJournal::create(head->directory(),
                                             "journal.00000000000000000000000000000002",
                                             root(), root_capacity));
  require(head->publish_initial(SyncStrength::full));
  const RetainedEvent reservation{{}, IssuerReservationEvent{1}};
  const std::array encoded{require(encode_retained_event(reservation, 1024))};
  const auto packet =
      require(encode_retained_proposal(first->cursor(), {}, encoded, 32768));
  CHECK(packet.size() == 68);
  const ProvisionalCaptureEvent capture{root().journal, 1, 68,
                                        CapturePurpose::ordinary_submission};
  const JournalHeader child{root().environment, id<AuditStreamId>(12), 13,
                            root().limits, JournalPredecessor{root().journal, 0, 112}};
  auto second = require(FramedJournal::create(
      head->directory(), "journal.0000000000000000000000000000000c", child,
      {8192, 16}));
  const RecoveryChoiceEvent choice{*child.predecessor, 112,       112,
                                   root().limits,      {4096, 8}, {8192, 16},
                                   std::nullopt,       {},        {capture}};
  const auto choice_bytes = require(encode_retained_event({{}, choice}, 1024));
  const std::array choose{JournalDraft{FrameKind::semantic, choice_bytes}};
  cursor_is(require(second->append(choose)), child.journal, 2, 352);
  const auto marker =
      require(encode_retained_event({{{child.journal, 3}}, capture}, 1024));
  const std::array captured{JournalDraft{FrameKind::source, packet},
                            JournalDraft{FrameKind::semantic, marker}};
  cursor_is(require(second->append(captured)), child.journal, 5, 608);
  require(head->replace(head->selection(), child.journal, SyncStrength::full));
  second.reset();
  first.reset();
  head.reset();
  auto owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds));
  require(owner->confirm_recovery());
  Custody custody;
  require(owner->reconcile(custody));
  const auto issued = require(owner->issue<DecisionId>());
  IdentityBytes expected{};
  expected[0] = std::byte{13};
  expected[8] = std::byte{1};
  CHECK(issued.bytes() == expected);
  const auto active = require(owner->submit(reservation));
  CHECK(active.existing && active.record == RecordReference{child.journal, 6} &&
        active.evidence == RetainedEvidence::live);
  const std::array events{reservation};
  const auto original_result =
      require(owner->submit_proposal({root().journal, 0, 112}, {}, events));
  CHECK(original_result.existing && original_result.capture == capture &&
        original_result.events.size() == 1);
  CHECK(original_result.events[0].record == RecordReference{root().journal, 1} &&
        original_result.events[0].evidence == RetainedEvidence::uncertain);
  cursor_is(owner->cursor(), child.journal, 7, 712);
}
enum class CaptureFault {
  none,
  undeclared_marker,
  extra_marker,
  wrong_reference,
  reversed_references,
  foreign_reference,
  length_low,
  length_high,
  wrong_origin,
  sequence_overflow,
  malformed_packet,
  trailing_packet,
  origin_profile,
  changed_committed,
  changed_uncertain,
  later_keyed_active,
  later_keyed_historical,
  later_nonkeyed_active,
  later_nonkeyed_historical
};
JournalHeader malformed_capture_fixture(const TemporaryDirectory &temporary,
                                        CaptureFault fault) {
  auto declaration = root();
  if (fault == CaptureFault::origin_profile)
    declaration.limits.max_payload = 128;
  auto head = require(
      EnvironmentHead::prepare(directory(temporary), declaration, SyncStrength::full));
  auto first = require(FramedJournal::create(head->directory(),
                                             "journal.00000000000000000000000000000002",
                                             declaration, root_capacity));
  const bool later_keyed = fault == CaptureFault::later_keyed_active ||
                           fault == CaptureFault::later_keyed_historical;
  const bool later_nonkeyed = fault == CaptureFault::later_nonkeyed_active ||
                              fault == CaptureFault::later_nonkeyed_historical;
  RetainedEvent valid{{{root().journal, 1}}, decision()};
  if (later_keyed)
    valid = {{}, ComplaintEvent{id<ComplaintId>(20), id<ParticipantId>(5), original}};
  if (later_nonkeyed)
    valid = {{}, RejectedSubmissionEvent{{ErrorCode::conflict}}};
  const auto valid_bytes = require(encode_retained_event(valid, 1024));
  if (fault == CaptureFault::changed_committed) {
    const std::array prior{JournalDraft{FrameKind::source, original},
                           JournalDraft{FrameKind::semantic, valid_bytes}};
    cursor_is(require(first->append(prior)), root().journal, 3, 390);
  }
  require(head->publish_initial(SyncStrength::full));
  auto captured_event = valid;
  if (fault == CaptureFault::changed_committed)
    std::get<DecisionEvent>(captured_event.body).continuation[0] = std::byte{1};
  const std::array encoded{require(encode_retained_event(captured_event, 1024))};
  const std::array sources{ByteView{original}};
  auto expected = first->cursor();
  if (fault == CaptureFault::sequence_overflow)
    expected.sequence = UINT64_MAX;
  auto packet = require(encode_retained_proposal(expected, sources, encoded, 32768));
  if (fault == CaptureFault::malformed_packet)
    packet[0] = std::byte{0};
  if (fault == CaptureFault::trailing_packet)
    packet.push_back(std::byte{0});
  const auto length = packet.size() + (fault == CaptureFault::length_high ? 1U : 0U) -
                      (fault == CaptureFault::length_low ? 1U : 0U);
  const ProvisionalCaptureEvent capture{root().journal, first->cursor().sequence + 1,
                                        length, CapturePurpose::ordinary_submission};
  const JournalHeader child{root().environment,
                            id<AuditStreamId>(12),
                            13,
                            {1024, 4096},
                            JournalPredecessor{root().journal, first->cursor().sequence,
                                               first->cursor().end_offset}};
  auto second = require(FramedJournal::create(
      head->directory(), "journal.0000000000000000000000000000000c", child,
      {8192, 16}));
  RecoveryChoiceEvent choice{*child.predecessor,
                             first->cursor().end_offset,
                             first->cursor().end_offset,
                             declaration.limits,
                             {4096, 8},
                             {8192, 16},
                             std::nullopt,
                             {},
                             {capture}};
  if (fault == CaptureFault::undeclared_marker)
    choice.required_captures.clear();
  const auto choice_bytes = require(encode_retained_event({{}, choice}, 1024));
  const std::array choose{JournalDraft{FrameKind::semantic, choice_bytes}};
  require(second->append(choose));
  std::vector<SourceReference> references{{child.journal, 3}, {child.journal, 4}};
  if (fault == CaptureFault::wrong_reference)
    references[0].sequence = 99;
  if (fault == CaptureFault::reversed_references)
    std::swap(references[0], references[1]);
  if (fault == CaptureFault::foreign_reference)
    references[0].journal = id<AuditStreamId>(99);
  auto marker_capture = capture;
  if (fault == CaptureFault::wrong_origin)
    marker_capture.origin = id<AuditStreamId>(99);
  const auto marker =
      require(encode_retained_event({references, marker_capture}, 1024));
  const auto split = std::min<std::size_t>(100, packet.size() - 1);
  const std::array captured{
      JournalDraft{FrameKind::source, ByteView{packet}.first(split)},
      JournalDraft{FrameKind::source, ByteView{packet}.subspan(split)},
      JournalDraft{FrameKind::semantic, marker}};
  require(second->append(captured));
  if (fault == CaptureFault::extra_marker) {
    const std::array extra{JournalDraft{FrameKind::semantic, marker}};
    require(second->append(extra));
  }
  require(head->replace(head->selection(), child.journal, SyncStrength::full));
  if (later_keyed || later_nonkeyed) {
    const std::array duplicate{JournalDraft{FrameKind::semantic, valid_bytes}};
    const auto later = require(second->append(duplicate));
    cursor_is(later, child.journal, 8, later_keyed ? 837U : 783U);
    if (fault == CaptureFault::later_keyed_historical ||
        fault == CaptureFault::later_nonkeyed_historical) {
      const JournalHeader last{
          root().environment, id<AuditStreamId>(14), 15, child.limits,
          JournalPredecessor{child.journal, later.sequence, later.end_offset}};
      auto third = require(FramedJournal::create(
          head->directory(), "journal.0000000000000000000000000000000e", last,
          {16384, 32}));
      const RecoveryChoiceEvent next{*last.predecessor,
                                     later.end_offset,
                                     later.end_offset,
                                     child.limits,
                                     {8192, 16},
                                     {16384, 32},
                                     std::nullopt,
                                     {},
                                     {}};
      const auto next_bytes = require(encode_retained_event({{}, next}, 1024));
      const std::array choose_next{JournalDraft{FrameKind::semantic, next_bytes}};
      require(third->append(choose_next));
      require(head->replace(head->selection(), last.journal, SyncStrength::full));
    }
  }
  if (fault == CaptureFault::changed_uncertain) {
    auto changed = valid;
    std::get<DecisionEvent>(changed.body).continuation[0] = std::byte{1};
    const std::array changed_bytes{require(encode_retained_event(changed, 1024))};
    const auto later_packet =
        require(encode_retained_proposal(second->cursor(), {}, changed_bytes, 32768));
    const ProvisionalCaptureEvent later_capture{
        child.journal, second->cursor().sequence + 1, later_packet.size(),
        CapturePurpose::ordinary_submission};
    const JournalHeader last{
        root().environment, id<AuditStreamId>(14), 15, child.limits,
        JournalPredecessor{child.journal, second->cursor().sequence,
                           second->cursor().end_offset}};
    auto third = require(FramedJournal::create(
        head->directory(), "journal.0000000000000000000000000000000e", last,
        {16384, 32}));
    const RecoveryChoiceEvent later_choice{*last.predecessor,
                                           second->cursor().end_offset,
                                           second->cursor().end_offset,
                                           child.limits,
                                           {8192, 16},
                                           {16384, 32},
                                           std::nullopt,
                                           {},
                                           {later_capture}};
    const auto later_choice_bytes =
        require(encode_retained_event({{}, later_choice}, 1024));
    const std::array later_choose{
        JournalDraft{FrameKind::semantic, later_choice_bytes}};
    require(third->append(later_choose));
    const auto later_marker =
        require(encode_retained_event({{{last.journal, 3}}, later_capture}, 1024));
    const std::array later{JournalDraft{FrameKind::source, later_packet},
                           JournalDraft{FrameKind::semantic, later_marker}};
    require(third->append(later));
    require(head->replace(head->selection(), last.journal, SyncStrength::full));
  }
  return declaration;
}
void capture_negative_cases() {
  for (const auto fault :
       {CaptureFault::undeclared_marker, CaptureFault::extra_marker,
        CaptureFault::wrong_reference, CaptureFault::reversed_references,
        CaptureFault::foreign_reference, CaptureFault::length_low,
        CaptureFault::length_high, CaptureFault::wrong_origin,
        CaptureFault::sequence_overflow, CaptureFault::malformed_packet,
        CaptureFault::trailing_packet, CaptureFault::origin_profile,
        CaptureFault::changed_committed, CaptureFault::changed_uncertain}) {
    TemporaryDirectory temporary;
    const auto declaration = malformed_capture_fixture(temporary, fault);
    const auto expected = fault == CaptureFault::malformed_packet ||
                                  fault == CaptureFault::trailing_packet
                              ? ErrorCode::corrupt
                          : fault == CaptureFault::origin_profile ? ErrorCode::capacity
                                                                  : ErrorCode::conflict;
    error_is(RetainedEnvironment::open(directory(temporary), declaration, root_capacity,
                                       bounds),
             expected);
  }
}
void later_uncertain_frame_is_not_promotion() {
  for (const auto fault :
       {CaptureFault::later_keyed_active, CaptureFault::later_keyed_historical,
        CaptureFault::later_nonkeyed_active, CaptureFault::later_nonkeyed_historical}) {
    TemporaryDirectory temporary;
    const auto declaration = malformed_capture_fixture(temporary, fault);
    auto opened = RetainedEnvironment::open(directory(temporary), declaration,
                                            root_capacity, bounds);
    if (fault == CaptureFault::later_keyed_historical ||
        fault == CaptureFault::later_nonkeyed_historical) {
      error_is(opened, ErrorCode::corrupt);
      continue;
    }
    auto owner = require(std::move(opened));
    const bool keyed = fault == CaptureFault::later_keyed_active;
    cursor_is(owner->cursor(), id<AuditStreamId>(12), 6, keyed ? 702U : 675U);
    const auto problem = owner->recovery_report().problem;
    CHECK(problem && problem->code == ErrorCode::conflict);
    const RetainedEvent event =
        keyed ? RetainedEvent{{},
                              ComplaintEvent{id<ComplaintId>(20), id<ParticipantId>(5),
                                             original}}
              : RetainedEvent{{}, RejectedSubmissionEvent{{ErrorCode::conflict}}};
    const auto existing = require(owner->submit(event));
    CHECK(existing.record == RecordReference{root().journal, 2} &&
          existing.evidence == RetainedEvidence::uncertain);
    require(owner->confirm_recovery());
    CHECK(owner->committed_facts().size() == 2 &&
          owner->state() == JournalWriterState::blocked);
    Custody custody;
    error_is(owner->reconcile(custody), ErrorCode::audit_unavailable);
    CHECK(custody.calls == 0 && owner->provisional_originals().size() == 1);
  }
}
std::vector<std::byte> native_bytes(const TemporaryDirectory &temporary,
                                    std::string_view name) {
  auto storage = directory(temporary);
  auto file = require(storage->open_existing(name, FileAccess::read_only));
  const auto size = require(file->extent());
  CHECK(size <= 32768);
  std::vector<std::byte> bytes(static_cast<std::size_t>(size));
  std::size_t done = 0;
  while (done < bytes.size()) {
    const auto amount =
        require(file->read_at(done, MutableByteView{bytes}.subspan(done)));
    CHECK(amount > 0 && amount <= bytes.size() - done);
    done += amount;
  }
  return bytes;
}
void capture_allocation_and_marker_damage() {
  TemporaryDirectory temporary;
  const auto declaration = malformed_capture_fixture(temporary, CaptureFault::none);
  const auto root_before =
      native_bytes(temporary, "journal.00000000000000000000000000000002");
  const auto child_before =
      native_bytes(temporary, "journal.0000000000000000000000000000000c");
  CHECK(child_before.size() == 810);
  std::unique_ptr<RetainedEnvironment> owner;
  std::ptrdiff_t successful_cut = -1;
  for (std::ptrdiff_t cut = 0; cut < 512; ++cut) {
    auto storage = directory(temporary);
    allocation_cut.store(cut);
    auto opened = RetainedEnvironment::open(std::move(storage), declaration,
                                            root_capacity, bounds);
    allocation_cut.store(-1);
    if (opened.has_value()) {
      owner = std::move(opened).value();
      successful_cut = cut;
      break;
    }
    error_is(opened, ErrorCode::allocation);
  }
  CHECK(successful_cut > 0 && owner != nullptr);
  CHECK(native_bytes(temporary, "journal.00000000000000000000000000000002") ==
        root_before);
  CHECK(native_bytes(temporary, "journal.0000000000000000000000000000000c") ==
        child_before);
  CHECK(owner->committed_facts().empty() && owner->provisional_originals().size() == 1);
  const auto packet = owner->provisional_originals()[0].bytes;
  const std::array sources{ByteView{original}};
  const std::array events{RetainedEvent{{{root().journal, 1}}, decision()}};
  successful_cut = -1;
  for (std::ptrdiff_t cut = 0; cut < 128; ++cut) {
    allocation_cut.store(cut);
    auto query = owner->submit_proposal({root().journal, 0, 112}, sources, events);
    allocation_cut.store(-1);
    cursor_is(owner->cursor(), id<AuditStreamId>(12), 6, 810);
    CHECK(owner->state() == JournalWriterState::recovery_pending &&
          owner->committed_facts().empty());
    CHECK(owner->provisional_originals()[0].bytes == packet);
    if (query.has_value()) {
      CHECK(query.value().existing && query.value().events.size() == 1);
      successful_cut = cut;
      break;
    }
    error_is(query, ErrorCode::allocation);
  }
  CHECK(successful_cut > 0);
  CHECK(native_bytes(temporary, "journal.0000000000000000000000000000000c") ==
        child_before);
  owner.reset();
  auto storage = directory(temporary);
  auto file = require(storage->open_existing("journal.0000000000000000000000000000000c",
                                             FileAccess::read_write));
  // Marker frame starts630, payload662: mutate capture origin at662+8+48.
  const std::array bad{std::byte{99}};
  CHECK(require(file->write_at(718, bad)) == 1);
  file.reset();
  storage.reset();
  error_is(RetainedEnvironment::open(directory(temporary), declaration, root_capacity,
                                     bounds),
           ErrorCode::conflict);
  CHECK(native_bytes(temporary, "journal.00000000000000000000000000000002") ==
        root_before);
}
// Build the selected files through the already qualified low-level wire/storage
// API. Their semantic validity and exact positions are independent expectations.
enum class RangeFault { none, available_before_prefix, diagnostic_after_available };
enum class SemanticFault { none, historical, active };
struct HistoryFaults {
  bool wrong_old_capacity = false;
  bool repeated_namespace = false;
  bool missing_custody = false;
  RangeFault range = RangeFault::none;
  SemanticFault semantic = SemanticFault::none;
  bool missing_capture = false;
};
void history_fixture(const TemporaryDirectory &temporary, HistoryFaults faults = {}) {
  auto head = require(
      EnvironmentHead::prepare(directory(temporary), root(), SyncStrength::full));
  auto first = require(FramedJournal::create(head->directory(),
                                             "journal.00000000000000000000000000000002",
                                             root(), root_capacity));
  const auto payload =
      require(encode_retained_event({{{root().journal, 1}}, decision()}, 1024));
  const std::array drafts{JournalDraft{FrameKind::source, original},
                          JournalDraft{FrameKind::semantic, payload}};
  cursor_is(require(first->append(drafts)), root().journal, 3, 390);
  const auto reservation =
      require(encode_retained_event({{}, IssuerReservationEvent{1}}, 1024));
  const std::array reserve{JournalDraft{FrameKind::semantic, reservation}};
  cursor_is(require(first->append(reserve)), root().journal, 5, 494);
  if (faults.missing_custody) {
    const auto prior_invocation = require(encode_retained_event(
        {{{root().journal, 1}},
         InvocationEvent{id<InvocationId>(10), id<DecisionId>(4),
                         id<DefinitionGenerationId>(8), original}},
        1024));
    const auto prior_admission = require(encode_retained_event(
        {{{root().journal, 1}},
         AttemptAdmissionEvent{id<OperationAttemptId>(11), id<InvocationId>(10),
                               id<DecisionId>(4), original}},
        1024));
    const std::array prior{JournalDraft{FrameKind::semantic, prior_invocation},
                           JournalDraft{FrameKind::semantic, prior_admission}};
    cursor_is(require(first->append(prior)), root().journal, 8, 788);
  }
  require(head->publish_initial(SyncStrength::full));
  JournalHeader child{root().environment, id<AuditStreamId>(12),
                      faults.repeated_namespace ? 3U : 13U, root().limits,
                      JournalPredecessor{root().journal,
                                         faults.missing_custody ? 8U : 5U,
                                         faults.missing_custody ? 788U : 494U}};
  auto second = require(FramedJournal::create(
      head->directory(), "journal.0000000000000000000000000000000c", child,
      {8192, 16}));
  const auto prior_end = faults.missing_custody ? 788U : 494U;
  RecoveryChoiceEvent choice{*child.predecessor, prior_end, prior_end,
                             root().limits,      {4096, 8}, {8192, 16},
                             std::nullopt,       {},        {}};
  if (faults.range == RangeFault::available_before_prefix) {
    choice.available_end = 200;
    choice.diagnostic_offset = 112;
  } else if (faults.range == RangeFault::diagnostic_after_available) {
    choice.diagnostic_offset = prior_end + 1;
  }
  if (faults.missing_capture)
    choice.required_captures.push_back(
        {root().journal, 6, 64, CapturePurpose::ordinary_submission});
  auto bytes = require(encode_retained_event({{}, choice}, 1024));
  const std::array choose{JournalDraft{FrameKind::semantic, bytes}};
  cursor_is(require(second->append(choose)), child.journal, 2,
            faults.missing_capture ? 352U : 316U);
  bytes = require(encode_retained_event(
      {{{root().journal, 1}},
       InvocationEvent{
           id<InvocationId>(10),
           id<DecisionId>(faults.semantic == SemanticFault::historical ? 17 : 4),
           id<DefinitionGenerationId>(8), original}},
      1024));
  const std::array invocation{JournalDraft{FrameKind::source, original},
                              JournalDraft{FrameKind::semantic, bytes}};
  cursor_is(require(second->append(invocation)), child.journal, 5,
            faults.missing_capture ? 562U : 526U);
  cursor_is(require(second->append(reserve)), child.journal, 7,
            faults.missing_capture ? 666U : 630U);
  require(head->replace(head->selection(), child.journal, SyncStrength::full));
  JournalHeader last{
      root().environment, id<AuditStreamId>(14), 15, root().limits,
      JournalPredecessor{child.journal, 7, faults.missing_capture ? 666U : 630U}};
  auto third = require(FramedJournal::create(head->directory(),
                                             "journal.0000000000000000000000000000000e",
                                             last, {16384, 32}));
  choice = {*last.predecessor,
            faults.missing_capture ? 666U : 630U,
            faults.missing_capture ? 666U : 630U,
            child.limits,
            {8192, faults.wrong_old_capacity ? 15U : 16U},
            {16384, 32},
            std::nullopt,
            {},
            {}};
  bytes = require(encode_retained_event({{}, choice}, 1024));
  const std::array next_choice{JournalDraft{FrameKind::semantic, bytes}};
  cursor_is(require(third->append(next_choice)), last.journal, 2, 316);
  bytes = require(encode_retained_event(
      {{{child.journal, 3}},
       AttemptAdmissionEvent{
           id<OperationAttemptId>(11), id<InvocationId>(10),
           id<DecisionId>(faults.semantic == SemanticFault::active ? 17 : 4),
           original}},
      1024));
  const std::array admission{JournalDraft{FrameKind::source, original},
                             JournalDraft{FrameKind::semantic, bytes}};
  cursor_is(require(third->append(admission)), last.journal, 5, 526);
  require(head->replace(head->selection(), last.journal, SyncStrength::full));
}
void selected_history() {
  TemporaryDirectory temporary;
  history_fixture(temporary);
  auto exact = bounds;
  exact.max_history_entries = 10;
  auto owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, exact));
  cursor_is(owner->cursor(), id<AuditStreamId>(14), 5, 526);
  CHECK(owner->committed_facts().empty());
  require(owner->confirm_recovery());
  CHECK(owner->committed_facts().size() == 7);
  for (const auto ref :
       {SourceReference{root().journal, 1}, SourceReference{id<AuditStreamId>(12), 3},
        SourceReference{id<AuditStreamId>(14), 3}})
    CHECK(require(owner->source(ref)) == original);
  error_is(owner->source({root().journal, 3}), ErrorCode::stale_handle);
  error_is(owner->source({id<AuditStreamId>(12), 1}), ErrorCode::stale_handle);
  error_is(owner->source({id<AuditStreamId>(99), 3}), ErrorCode::stale_handle);
  Custody custody;
  // One recovered admission is nonterminal and therefore must be verified.
  class AdmissionCustody final : public CustodyVerifier {
  public:
    Result<void> verify(std::span<const AttemptState> attempts) override {
      CHECK(attempts.size() == 1 && !attempts[0].opened &&
            attempts[0].admission.attempt == id<OperationAttemptId>(11));
      return Result<void>::success();
    }
  } admission_custody;
  require(owner->reconcile(admission_custody));
  Effect effect;
  CHECK(!require(owner->dispatch(id<OperationAttemptId>(11), effect)).dispatched);
  CHECK(effect.calls == 0); // Reconciliation never grants old dispatch permission.
  error_is(owner->submit({{}, AttemptOpenEvent{id<OperationAttemptId>(11)}}),
           ErrorCode::capacity);
  CHECK(effect.calls == 0 && owner->committed_facts().size() == 7);
  owner.reset();
  exact.max_history_entries = 9;
  error_is(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, exact),
      ErrorCode::capacity);
  error_is(RetainedEnvironment::open(directory(temporary), root(), {4096, 7}, bounds),
           ErrorCode::conflict);
  exact = bounds;
  exact.max_segments = 2;
  error_is(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, exact),
      ErrorCode::capacity);
  exact = bounds;
  exact.framing.max_payload = 512;
  error_is(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, exact),
      ErrorCode::capacity);
  for (const auto kind : {0, 1}) {
    TemporaryDirectory invalid;
    history_fixture(invalid,
                    {.wrong_old_capacity = kind == 0, .repeated_namespace = kind == 1});
    error_is(
        RetainedEnvironment::open(directory(invalid), root(), root_capacity, bounds),
        ErrorCode::conflict);
  }
}
void historical_corruption_closes_admission() {
  TemporaryDirectory temporary;
  history_fixture(temporary);
  auto owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds));
  require(owner->confirm_recovery());
  class Verified final : public CustodyVerifier {
  public:
    Result<void> verify(std::span<const AttemptState> attempts) override {
      CHECK(attempts.size() == 1 && !attempts[0].opened);
      return Result<void>::success();
    }
  } verified;
  require(owner->reconcile(verified));
  auto raw = directory(temporary);
  auto file = require(raw->open_existing("journal.00000000000000000000000000000002",
                                         FileAccess::read_write));
  const std::array changed{std::byte{1}};
  CHECK(require(file->write_at(144, changed)) == 1);
  error_is(owner->source({root().journal, 1}), ErrorCode::corrupt);
  CHECK(owner->state() == JournalWriterState::blocked);
  error_is(owner->issue<OperationAttemptId>(), ErrorCode::audit_unavailable);
  CHECK(require(owner->read_original_range(root().journal, 144, 1)).bytes[0] ==
        changed[0]);
}
void active_namespace_and_small_budget() {
  TemporaryDirectory temporary;
  history_fixture(temporary);
  auto owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds));
  require(owner->confirm_recovery());
  class Verified final : public CustodyVerifier {
  public:
    Result<void> verify(std::span<const AttemptState> attempts) override {
      CHECK(attempts.size() == 1 && !attempts[0].opened);
      return Result<void>::success();
    }
  } verified;
  require(owner->reconcile(verified));
  IdentityBytes expected{};
  expected[0] = std::byte{15};
  expected[8] = std::byte{1};
  CHECK(require(owner->issue<DecisionId>()).bytes() == expected);
  CHECK(owner->committed_facts().size() == 8);

  TemporaryDirectory limited;
  auto small = bounds;
  small.max_history_entries = 1;
  owner = require(
      RetainedEnvironment::create(directory(limited), root(), root_capacity, small));
  const std::array sources{ByteView{original}};
  const std::array events{RetainedEvent{{{root().journal, 1}}, decision()}};
  error_is(owner->submit_proposal(owner->cursor(), sources, events),
           ErrorCode::capacity);
  cursor_is(owner->cursor(), root().journal, 0, 112);
  CHECK(owner->committed_facts().empty());
  small.framing.max_payload = 512;
  TemporaryDirectory invalid;
  error_is(
      RetainedEnvironment::create(directory(invalid), root(), root_capacity, small),
      ErrorCode::capacity);
  CHECK(std::filesystem::is_empty(invalid.path));
  auto invalid_root = root();
  invalid_root.issuer_namespace = 0;
  error_is(RetainedEnvironment::create(directory(invalid), invalid_root, root_capacity,
                                       bounds),
           ErrorCode::invalid_identity);
  CHECK(std::filesystem::is_empty(invalid.path));
}
void reviewed_choice_and_batch_cases() {
  for (const auto range :
       {RangeFault::available_before_prefix, RangeFault::diagnostic_after_available}) {
    TemporaryDirectory temporary;
    history_fixture(temporary, {.range = range});
    error_is(
        RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds),
        ErrorCode::conflict);
  }
  TemporaryDirectory unsupported;
  history_fixture(unsupported, {.missing_capture = true});
  error_is(
      RetainedEnvironment::open(directory(unsupported), root(), root_capacity, bounds),
      ErrorCode::conflict);
  {
    TemporaryDirectory checked;
    auto head = require(
        EnvironmentHead::prepare(directory(checked), root(), SyncStrength::full));
    auto first = require(FramedJournal::create(
        head->directory(), "journal.00000000000000000000000000000002", root(),
        root_capacity));
    require(head->publish_initial(SyncStrength::full));
    JournalHeader child{root().environment, id<AuditStreamId>(12), 13, root().limits,
                        JournalPredecessor{root().journal, 0, 112}};
    auto second = require(FramedJournal::create(
        head->directory(), "journal.0000000000000000000000000000000c", child,
        {8192, 16}));
    const RecoveryChoiceEvent choice{
        *child.predecessor,
        112,
        112,
        root().limits,
        {4096, 8},
        {8192, 16},
        std::nullopt,
        {{id<OperationAttemptId>(11), false, std::nullopt, AttemptDisposition::none,
          RetainedEvidence::recovered}},
        {}};
    const auto payload = require(encode_retained_event({{}, choice}, 1024));
    const std::array draft{JournalDraft{FrameKind::semantic, payload}};
    cursor_is(require(second->append(draft)), child.journal, 2, 336);
    require(head->replace(head->selection(), child.journal, SyncStrength::full));
    first.reset();
    second.reset();
    head.reset();
    error_is(
        RetainedEnvironment::open(directory(checked), root(), root_capacity, bounds),
        ErrorCode::unsupported);
  }
  TemporaryDirectory historical;
  history_fixture(historical, {.semantic = SemanticFault::historical});
  error_is(
      RetainedEnvironment::open(directory(historical), root(), root_capacity, bounds),
      ErrorCode::corrupt);
  TemporaryDirectory active;
  history_fixture(active, {.semantic = SemanticFault::active});
  auto owner = require(
      RetainedEnvironment::open(directory(active), root(), root_capacity, bounds));
  cursor_is(owner->cursor(), id<AuditStreamId>(14), 2, 316);
  CHECK(owner->recovery_report().problem &&
        owner->recovery_report().problem->code == ErrorCode::conflict);
  require(owner->confirm_recovery());
  CHECK(owner->state() == JournalWriterState::blocked &&
        owner->committed_facts().size() == 6);
  error_is(owner->source({id<AuditStreamId>(14), 3}), ErrorCode::stale_handle);
  CHECK(require(owner->read_original_range(id<AuditStreamId>(14), 348, 3)).bytes ==
        original);
  CHECK(require(owner->source({root().journal, 1})) == original);
  Custody custody;
  error_is(owner->reconcile(custody), ErrorCode::audit_unavailable);
  CHECK(custody.calls == 0);
}
void dedup_before_narrowed_profile() {
  TemporaryDirectory temporary;
  auto head = require(
      EnvironmentHead::prepare(directory(temporary), root(), SyncStrength::full));
  auto first = require(FramedJournal::create(head->directory(),
                                             "journal.00000000000000000000000000000002",
                                             root(), root_capacity));
  auto long_decision = decision();
  long_decision.continuation.assign(600, std::byte{42});
  const RetainedEvent event{{{root().journal, 1}}, long_decision};
  const auto payload = require(encode_retained_event(event, 1024));
  CHECK(payload.size() == 752);
  const std::array drafts{JournalDraft{FrameKind::source, original},
                          JournalDraft{FrameKind::semantic, payload}};
  cursor_is(require(first->append(drafts)), root().journal, 3, 987);
  require(head->publish_initial(SyncStrength::full));
  JournalHeader child{root().environment,
                      id<AuditStreamId>(12),
                      13,
                      {256, 4096},
                      JournalPredecessor{root().journal, 3, 987}};
  auto second = require(FramedJournal::create(
      head->directory(), "journal.0000000000000000000000000000000c", child,
      {8192, 16}));
  const RecoveryChoiceEvent choice{*child.predecessor, 987,       987,
                                   root().limits,      {4096, 8}, {8192, 16},
                                   std::nullopt,       {},        {}};
  const auto choice_payload = require(encode_retained_event({{}, choice}, 256));
  const std::array choose{JournalDraft{FrameKind::semantic, choice_payload}};
  cursor_is(require(second->append(choose)), child.journal, 2, 316);
  require(head->replace(head->selection(), child.journal, SyncStrength::full));
  first.reset();
  second.reset();
  head.reset();
  auto owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds));
  require(owner->confirm_recovery());
  Custody custody;
  require(owner->reconcile(custody));
  const std::array events{event};
  const auto existing = require(owner->submit_proposal(owner->cursor(), {}, events));
  CHECK(existing.existing && existing.events.size() == 1 &&
        existing.events[0].existing);
  CHECK(existing.events[0].record == RecordReference{root().journal, 2});
  cursor_is(existing.cursor, child.journal, 2, 316);
  long_decision.decision = id<DecisionId>(21);
  error_is(owner->submit({{{root().journal, 1}}, long_decision}), ErrorCode::capacity);
  cursor_is(owner->cursor(), child.journal, 2, 316);
}
void selected_headers_and_unselected_suffix() {
  TemporaryDirectory temporary;
  history_fixture(temporary);
  auto changed_root = root();
  changed_root.limits.max_payload = 512;
  error_is(RetainedEnvironment::open(directory(temporary), changed_root, root_capacity,
                                     bounds),
           ErrorCode::conflict);
  {
    auto raw = directory(temporary);
    auto first = require(FramedJournal::open(
        *raw, "journal.00000000000000000000000000000002", root(), bounds.segment));
    require(first->confirm_recovery());
    require(first->resume_after_reconciliation());
    const std::array surplus{std::byte{42}, std::byte{43}, std::byte{44}};
    const std::array drafts{JournalDraft{FrameKind::source, surplus}};
    cursor_is(require(first->append(drafts)), root().journal, 7, 585);
    first.reset();
    auto file = require(raw->open_existing("journal.00000000000000000000000000000002",
                                           FileAccess::read_write));
    const std::array corrupt{std::byte{99}};
    CHECK(require(file->write_at(526, corrupt)) == 1);
  }
  auto owner = require(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds));
  require(owner->confirm_recovery());
  CHECK(require(owner->source({root().journal, 1})) == original);
  error_is(owner->source({root().journal, 6}), ErrorCode::stale_handle);
  const auto raw_bytes = require(owner->read_original_range(root().journal, 526, 3));
  CHECK(raw_bytes.observed_extent == 585 &&
        raw_bytes.bytes ==
            std::vector<std::byte>{std::byte{99}, std::byte{43}, std::byte{44}});
  CHECK(owner->committed_facts().size() == 7);
  owner.reset();
  CHECK(std::filesystem::remove(temporary.path +
                                "/journal.0000000000000000000000000000000e"));
  error_is(
      RetainedEnvironment::open(directory(temporary), root(), root_capacity, bounds),
      ErrorCode::io);

  TemporaryDirectory cycle;
  history_fixture(cycle);
  auto raw = directory(cycle);
  auto file = require(raw->open_existing("journal.0000000000000000000000000000000c",
                                         FileAccess::read_write));
  std::array<std::byte, journal_header_size> bytes{};
  CHECK(require(file->read_at(0, bytes)) == bytes.size());
  auto header = require(decode_journal_header(bytes));
  header.predecessor = JournalPredecessor{id<AuditStreamId>(14), 5, 526};
  const auto encoded = require(encode_journal_header(header));
  CHECK(require(file->write_at(0, encoded)) == encoded.size());
  file.reset();
  raw.reset();
  error_is(RetainedEnvironment::open(directory(cycle), root(), root_capacity, bounds),
           ErrorCode::conflict);
}
} // namespace
int main() {
  try {
    root_owner();
    duplicate_positions();
    captured_empty_predecessor();
    split_capture_groups();
    captured_positions_under_narrow_profile();
    rejected_capture_has_no_uncertain_admission();
    captured_reservation_does_not_burn_active_namespace();
    capture_negative_cases();
    later_uncertain_frame_is_not_promotion();
    capture_allocation_and_marker_damage();
    selected_history();
    historical_corruption_closes_admission();
    active_namespace_and_small_budget();
    reviewed_choice_and_batch_cases();
    dedup_before_narrowed_profile();
    selected_headers_and_unselected_suffix();
    TemporaryDirectory missing;
    history_fixture(missing, {.missing_custody = true});
    error_is(
        RetainedEnvironment::open(directory(missing), root(), root_capacity, bounds),
        ErrorCode::conflict);
  } catch (const std::exception &error) {
    std::fprintf(stderr, "%s\n", error.what());
    return 1;
  }
}
