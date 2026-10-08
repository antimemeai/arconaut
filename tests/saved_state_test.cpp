#include "blackbird/context.hpp"
#include "blackbird/saved_state.hpp"
#include "blackbird/session.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
#define CHECK(x)                                                                       \
  do {                                                                                 \
    if (!(x))                                                                          \
      throw std::runtime_error("saved-state line " + std::to_string(__LINE__));        \
  } while (false)
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
struct Fault {
  bool enabled = false;
  unsigned phase = 0;
};
class FaultFile final : public JournalFile {
public:
  FaultFile(std::unique_ptr<JournalFile> file, Fault &fault)
      : file_(std::move(file)), fault_(fault) {}
  Result<std::size_t> read_at(std::uint64_t offset, MutableByteView bytes) override {
    return file_->read_at(offset, bytes);
  }
  Result<std::size_t> write_at(std::uint64_t offset, ByteView bytes) override {
    if (fault_.enabled && fault_.phase == 1) {
      if (offset >= 17)
        return Result<std::size_t>::failure({ErrorCode::io});
      return file_->write_at(offset,
                             bytes.first(std::min<std::size_t>(17, bytes.size())));
    }
    return file_->write_at(offset, bytes);
  }
  Result<std::uint64_t> extent() override { return file_->extent(); }
  Result<void> synchronize(SyncStrength strength) override {
    if (fault_.enabled && fault_.phase == 2)
      return Result<void>::failure({ErrorCode::io});
    return file_->synchronize(strength);
  }
  Result<void> lock_writer() override { return file_->lock_writer(); }
  void release_writer() noexcept override { file_->release_writer(); }

private:
  std::unique_ptr<JournalFile> file_;
  Fault &fault_;
};
class FaultDirectory final : public JournalDirectory {
public:
  FaultDirectory(std::unique_ptr<JournalDirectory> directory, Fault &fault)
      : directory_(std::move(directory)), fault_(fault) {}
  Result<std::unique_ptr<JournalFile>>
  create_exclusive(std::string_view name) override {
    auto created = directory_->create_exclusive(name);
    if (!created.has_value())
      return created;
    return Result<std::unique_ptr<JournalFile>>::success(
        std::make_unique<FaultFile>(std::move(created).value(), fault_));
  }
  Result<std::unique_ptr<JournalFile>> open_existing(std::string_view name,
                                                     FileAccess access) override {
    return directory_->open_existing(name, access);
  }
  Result<void> replace_file(std::string_view from, std::string_view to) override {
    if (fault_.enabled && fault_.phase == 3)
      return Result<void>::failure({ErrorCode::io});
    return directory_->replace_file(from, to);
  }
  Result<void> synchronize_directory(SyncStrength strength) override {
    if (fault_.enabled && fault_.phase == 4)
      return Result<void>::failure({ErrorCode::io});
    return directory_->synchronize_directory(strength);
  }

private:
  std::unique_ptr<JournalDirectory> directory_;
  Fault &fault_;
};
struct Empty final : CustodyVerifier {
  Result<void> verify(std::span<const AttemptState> attempts) override {
    return attempts.empty() ? Result<void>::success()
                            : Result<void>::failure({ErrorCode::external_unknown});
  }
};
int main() {
  char pattern[] = "/tmp/bb-saved-XXXXXX";
  const auto path = mkdtemp(pattern);
  if (!path)
    return 2;
  const JournalHeader h{id<EnvironmentId>(1),
                        id<AuditStreamId>(2),
                        3,
                        {1024 * 1024, 4 * 1024 * 1024},
                        std::nullopt};
  const JournalCapacity cap{64 * 1024 * 1024, 4096};
  Fault fault;
  auto native = [&]() {
    return std::make_unique<NativeJournalDirectory>(
        unwrap(NativeJournalDirectory::open(path)));
  };
  auto directory = [&]() { return std::make_unique<FaultDirectory>(native(), fault); };
  const auto history_query =
      Json::object({{"kind", Json{"history"}}, {"limit", Json{JsonNumber{"65536"}}}});
  try {
    Json view, originals, history, programs;
    std::string archived;
    std::uint64_t highwater = 0;
    {
      auto root = unwrap(RetainedState::create(directory(), "audit", h, cap));
      AuditLog log{*root};
      ContextStore ctx{log};
      ctx.append({Json::object({{"role", Json{"user"}},
                                {"content", Json{"exact old original"}}})},
                 "test");
      archived = string_field(ctx.view().find("entries")->array().front(), "id");
      CHECK(field(ctx.edit(Json::object(
                      {{"base", Json{ctx.head()}}, {"entries", Json{Json::Array{}}}})),
                  "accepted") == Json{true});
      ctx.append(
          {Json::object({{"role", Json{"assistant"}}, {"content", Json{"current"}}})},
          "test");
      for (unsigned i = 0; i < 45; ++i)
        ctx.edit(Json::object(
            {{"base", Json{"stale"}}, {"candidate", Json{std::string(1024, 'x')}}}));
      SessionStore session{log};
      session.save({"model-test", "high", "native"});
      session.restart("exact resume note");
      CHECK(session.resume(ctx));
      CHECK(!session.resume(ctx));
      programs = Json{root->current_programs()};
      unwrap(ctx.checkpoint());
      CHECK(root->saved_state() && root->saved_state()->unresolved.empty());
      CHECK(std::filesystem::file_size(std::string(path) + "/audit.state." +
                                       std::to_string(root->saved_state()->slot)) <
            16384);
      view = ctx.view();
      originals = ctx.originals();
      history = ctx.inspect(history_query);
      highwater = root->issuer_counter();
      // Each failed derived publication must leave the acknowledged journal live.
      for (unsigned phase = 1; phase <= 4; ++phase) {
        fault.enabled = true;
        fault.phase = phase;
        CHECK(!ctx.checkpoint().has_value());
        CHECK(root->state() == JournalWriterState::live);
        fault.enabled = false;
      }
    }
    {
      auto root = unwrap(RetainedState::open(directory(), "audit", h, cap));
      CHECK(root->saved_state());
      unwrap(root->confirm_recovery());
      Empty custody;
      unwrap(root->reconcile(custody));
      AuditLog log{*root};
      ContextStore ctx{log};
      CHECK(ctx.view() == view);
      CHECK(root->issuer_counter() == highwater);
      CHECK(Json{root->current_programs()} == programs);
      CHECK(ctx.originals() == originals && ctx.inspect(history_query) == history);
      SessionStore session{log};
      CHECK(session.settings().model == "model-test");
      CHECK(!session.resume(ctx));
      const auto issued = unwrap(root->issue<ApplicationRecordId>());
      CHECK(root->issuer_counter() > highwater);
      (void)issued;
      // Exact archived original remains restorable, not replaced by its edited live
      // item.
      ctx.restore(archived);
      CHECK(ctx.view().find("entries")->array().size() ==
            view.find("entries")->array().size() + 1);
      unwrap(ctx.checkpoint());
      // Accepted uncheckpointed suffix uses boundary-limited disk original predicates.
      ctx.append({Json::object({{"role", Json{"user"}}, {"content", Json{"tail"}}})},
                 "tail");
      view = ctx.view();
      originals = ctx.originals();
      history = ctx.inspect(history_query);
    }
    {
      auto root = unwrap(RetainedState::open(directory(), "audit", h, cap));
      unwrap(root->confirm_recovery());
      Empty custody;
      unwrap(root->reconcile(custody));
      AuditLog log{*root};
      ContextStore ctx{log};
      CHECK(ctx.view() == view);
      CHECK(ctx.originals() == originals);
      CHECK(ctx.inspect(history_query) == history);
      unwrap(ctx.checkpoint());
    }
    // Both unusable roots select full replay, preserving exact useful/history state.
    for (unsigned slot = 0; slot < 2; ++slot) {
      std::fstream f(std::string(path) + "/audit.state." + std::to_string(slot),
                     std::ios::binary | std::ios::in | std::ios::out);
      f.seekp(12);
      const char bad = '!';
      f.write(&bad, 1);
    }
    {
      auto root = unwrap(RetainedState::open(directory(), "audit", h, cap));
      CHECK(!root->saved_state());
      unwrap(root->confirm_recovery());
      Empty custody;
      unwrap(root->reconcile(custody));
      AuditLog log{*root};
      ContextStore ctx{log};
      CHECK(ctx.view() == view);
      CHECK(ctx.originals() == originals);
      CHECK(ctx.inspect(history_query) == history);
    }
    // Full-ledger custody is still authoritative. The saved closure preserves an
    // open effect and does not turn reopening into dispatch permission.
    const auto effect = id<OperationAttemptId>(51);
    {
      auto root = unwrap(RetainedState::open(directory(), "audit", h, cap));
      unwrap(root->confirm_recovery());
      Empty custody;
      unwrap(root->reconcile(custody));
      AuditLog log{*root};
      ContextStore ctx{log};
      log.record(ApplicationChannel::program,
                 Json::object({{"label", Json{"workflow-config-staged-v1"}},
                               {"model", Json{"must-not-activate"}}}));
      log.record(ApplicationChannel::program,
                 Json::object({{"label", Json{"workflow-config-failed-v1"}},
                               {"model", Json{"must-not-activate"}}}));
      CHECK(Json{root->current_programs()} == programs);
      const ImmutableBytes input{std::byte{0}, std::byte{255}, std::byte{10}};
      const std::array events{
          RetainedEvent{{},
                        DecisionEvent{id<DecisionId>(50),
                                      id<ParticipantId>(52),
                                      id<ConversationId>(53),
                                      id<WorkflowId>(54),
                                      id<DefinitionGenerationId>(55),
                                      id<ContextRevisionId>(56),
                                      {id<InvocationId>(57)},
                                      input}},
          RetainedEvent{{},
                        InvocationEvent{id<InvocationId>(57), id<DecisionId>(50),
                                        id<DefinitionGenerationId>(55), input}},
          RetainedEvent{{},
                        AttemptAdmissionEvent{effect, id<InvocationId>(57),
                                              id<DecisionId>(50), input}},
          RetainedEvent{{}, AttemptOpenEvent{effect}}};
      unwrap(root->append(root->cursor(), {}, events));
      unwrap(root->submit(
          {{},
           AttemptObservationEvent{
               effect, AttemptPhase::running, AttemptDisposition::none, {}}}));
      unwrap(root->submit(
          {{},
           AttemptObservationEvent{
               effect, AttemptPhase::settling, AttemptDisposition::none, {}}}));
      unwrap(ctx.checkpoint());
      CHECK(root->saved_state()->unresolved.size() == 5);
      CHECK(std::get<AttemptObservationEvent>(
                root->saved_state()->unresolved.back().event.body)
                .phase == AttemptPhase::settling);
      // Catalog ordinal and source queries fetch exact historical bytes.
      for (std::size_t i = 0; i < root->fact_count(); ++i)
        CHECK(unwrap(root->fact(i)).event == root->committed_facts()[i].event);
      highwater = root->issuer_counter();
    }
    {
      auto root = unwrap(RetainedState::open(directory(), "audit", h, cap));
      CHECK(root->saved_state() && root->saved_state()->unresolved.size() == 5);
      unwrap(root->confirm_recovery());
      CHECK(unwrap(root->attempt(effect)).reconciliation_required);
      Empty custody;
      CHECK(!root->reconcile(custody).has_value());
      CHECK(root->issuer_counter() == highwater);
      CHECK(!root->issue<ApplicationRecordId>().has_value());
    }
    // Independent settled lineage exercises actual compact selection, suffix,
    // metadata renewal, immutable readers, original repair and evidence.
    {
      const auto dirpath = std::string(path) + "/compact";
      std::filesystem::create_directory(dirpath);
      std::filesystem::permissions(dirpath, std::filesystem::perms::owner_all);
      auto dir = [&] {
        return std::make_unique<NativeJournalDirectory>(
            unwrap(NativeJournalDirectory::open(dirpath)));
      };
      std::string first;
      Json expected;
      std::size_t count = 0;
      {
        auto root = unwrap(RetainedState::create(dir(), "audit", h, cap));
        AuditLog log{*root};
        ContextStore ctx{log};
        (void)session_identity(log);
        log.record(
            ApplicationChannel::program,
            Json::object({{"label", Json{"station.profile"}}, {"adapter", Json{""}}}));
        ctx.append({Json::object(
                       {{"role", Json{"user"}}, {"content", Json{"kept original"}}})},
                   "compact-test");
        first = string_field(ctx.view().find("entries")->array()[0], "id");
        unwrap(ctx.checkpoint());
        ctx.append({Json::object(
                       {{"role", Json{"user"}}, {"content", Json{"accepted tail"}}})},
                   "suffix");
        expected = ctx.view();
        count = root->fact_count();
      }
      {
        auto root = unwrap(RetainedState::open(dir(), "audit", h, cap));
        CHECK(root->compact_recovery());
        unwrap(root->confirm_recovery());
        Empty e;
        unwrap(root->reconcile(e));
        AuditLog log{*root};
        ContextStore ctx{log};
        CHECK(ctx.view() == expected && root->fact_count() == count);
        const auto old = root->history_snapshot();
        ctx.append({Json::object({{"role", Json{"user"}},
                                  {"content", Json{"live after reopen"}}})},
                   "new");
        const auto last = root->fact_count() - 1;
        CHECK(unwrap(root->fact(last)).evidence == RetainedEvidence::live);
        unwrap(ctx.checkpoint());
        CHECK(root->tail_facts().empty());
        {
          const auto remaining = root->journal_usage().remaining_records();
          auto reserved = unwrap(root->protect_settlement({4096, remaining}));
          bool denied = false;
          try {
            log.record(ApplicationChannel::program,
                       Json::object({{"label", Json{"ordinary-capacity-probe"}}}));
          } catch (const Error &error) {
            denied = error.code == ErrorCode::capacity;
          }
          CHECK(denied);
        }
        CHECK(unwrap(root->history_snapshot().read(last)).evidence ==
              RetainedEvidence::live);
        CHECK(old.count == count &&
              unwrap(old.read(count - 1)).evidence == RetainedEvidence::recovered);
        const auto removed = ctx.edit(Json::object(
            {{"base", Json{ctx.head()}}, {"entries", Json{Json::Array{}}}}));
        (void)removed;
        ctx.restore(first);
        CHECK(string_field(ctx.items()[0], "content") == "kept original");
        unwrap(ctx.checkpoint());
        expected = ctx.view();
        count = root->fact_count();
      }
      {
        auto root = unwrap(RetainedState::open(dir(), "audit", h, cap));
        CHECK(root->compact_recovery());
        unwrap(root->confirm_recovery());
        Empty e;
        unwrap(root->reconcile(e));
        AuditLog log{*root};
        ContextStore ctx{log};
        CHECK(ctx.view() == expected && root->fact_count() == count);
        CHECK(unwrap(root->fact(count - 1)).evidence == RetainedEvidence::recovered);
        log.record(ApplicationChannel::program,
                   Json::object({{"label", Json{"station.control"}},
                                 {"command", Json{"pause"}}}));
        unwrap(ctx.checkpoint());
      }
      {
        auto root = unwrap(RetainedState::open(dir(), "audit", h, cap));
        CHECK(!root->compact_recovery());
      }
    }
    std::filesystem::remove_all(path);
    std::cout
        << "saved current boundary/tail/history/RRC/unresolved/staged/publication "
           "failures/corrupt fallback passed\n";
    return 0;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  std::filesystem::remove_all(path);
  return 1;
}
