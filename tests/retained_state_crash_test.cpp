#include "arconaut/retained_state.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

using namespace arconaut;
namespace {
void check(bool condition, const char *expression, int line) {
  if (!condition) {
    throw std::runtime_error(std::to_string(line) + ": " + expression);
  }
}
#define CHECK(...) check((__VA_ARGS__), #__VA_ARGS__, __LINE__)
template <typename T> T require(Result<T> result) {
  CHECK(result.has_value());
  return std::move(result).value();
}
void require(Result<void> result) { CHECK(result.has_value()); }
template <typename T> T id(unsigned char value) {
  IdentityBytes bytes{};
  bytes[15] = std::byte{value};
  return require(T::from_bytes(bytes));
}
class TemporaryDirectory {
public:
  TemporaryDirectory() {
    std::array<char, 48> pattern{};
    const std::string text = "/tmp/arconaut-state-crash-XXXXXX";
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
JournalHeader header() {
  return {id<EnvironmentId>(1), id<AuditStreamId>(2), 3, {1024, 4096}, std::nullopt};
}
constexpr JournalCapacity capacity{1 << 20, 1024};
const std::vector<std::byte> original{std::byte{0}, std::byte{255}, std::byte{10}};
DecisionEvent decision(unsigned char identity) {
  return {id<DecisionId>(identity),      id<ParticipantId>(4),
          id<ConversationId>(5),         id<WorkflowId>(6),
          id<DefinitionGenerationId>(7), id<ContextRevisionId>(8),
          {id<InvocationId>(9)},         original};
}
class Custody final : public CustodyVerifier {
public:
  std::size_t seen = 0;
  Result<void> verify(std::span<const AttemptState> attempts) override {
    seen = attempts.size();
    for (const auto &attempt : attempts) {
      CHECK(attempt.admission.attempt == id<OperationAttemptId>(11));
      CHECK(attempt.opened && !attempt.receipt && attempt.reconciliation_required);
    }
    // The effect below owns no living resource after its writing process dies.
    return Result<void>::success();
  }
};
enum class Cut { none, before_effect, after_effect };
[[noreturn]] void crash() {
  ::kill(::getpid(), SIGKILL);
  ::_exit(98);
}
class FileEffect final : public EffectBoundary {
public:
  FileEffect(JournalDirectory &directory, Cut cut) : directory_(directory), cut_(cut) {}
  std::size_t calls = 0;
  Result<void> dispatch(const EffectIntent &intent) override {
    ++calls;
    CHECK(intent.environment == header().environment &&
          intent.actor == id<ParticipantId>(4));
    CHECK(intent.invocation == id<InvocationId>(9));
    CHECK(std::equal(intent.input.begin(), intent.input.end(), original.begin(),
                     original.end()));
    if (cut_ == Cut::before_effect) {
      crash();
    }
    auto file = require(directory_.open_existing("effect", FileAccess::read_write));
    require(file->lock_writer());
    std::vector<std::byte> record(intent.attempt.bytes().begin(),
                                  intent.attempt.bytes().end());
    record.insert(record.end(), intent.input.begin(), intent.input.end());
    const auto offset = require(file->extent());
    std::size_t done = 0;
    while (done != record.size()) {
      const auto count =
          require(file->write_at(offset + done, ByteView{record}.subspan(done)));
      CHECK(count != 0 && count <= record.size() - done);
      done += count;
    }
    require(file->synchronize(SyncStrength::full));
    if (cut_ == Cut::after_effect) {
      crash();
    }
    return Result<void>::success();
  }

private:
  JournalDirectory &directory_;
  Cut cut_;
};
std::vector<std::byte> read_effect(JournalDirectory &directory) {
  auto file = require(directory.open_existing("effect", FileAccess::read_only));
  const auto size = require(file->extent());
  CHECK(size <= 38);
  std::vector<std::byte> bytes(static_cast<std::size_t>(size));
  std::size_t done = 0;
  while (done != bytes.size()) {
    const auto count =
        require(file->read_at(done, MutableByteView{bytes}.subspan(done)));
    CHECK(count != 0 && count <= bytes.size() - done);
    done += count;
  }
  return bytes;
}
struct ChunkCrash {
  bool armed = false;
  std::size_t syncs = 0;
};
class ChunkCrashFile final : public JournalFile {
public:
  ChunkCrashFile(std::unique_ptr<JournalFile> file, ChunkCrash &control)
      : file_(std::move(file)), control_(control) {}
  Result<void> lock_writer() override { return file_->lock_writer(); }
  Result<std::uint64_t> extent() override { return file_->extent(); }
  Result<std::size_t> read_at(std::uint64_t offset, MutableByteView bytes) override {
    return file_->read_at(offset, bytes);
  }
  Result<std::size_t> write_at(std::uint64_t offset, ByteView bytes) override {
    return file_->write_at(offset, bytes);
  }
  Result<void> synchronize(SyncStrength strength) override {
    const auto result = file_->synchronize(strength);
    if (result.has_value() && control_.armed && ++control_.syncs == 2) {
      crash();
    }
    return result;
  }

private:
  std::unique_ptr<JournalFile> file_;
  ChunkCrash &control_;
};
class ChunkCrashDirectory final : public JournalDirectory {
public:
  ChunkCrashDirectory(NativeJournalDirectory directory, ChunkCrash &control)
      : directory_(std::move(directory)), control_(control) {}
  Result<std::unique_ptr<JournalFile>>
  create_exclusive(std::string_view name) override {
    return wrap(directory_.create_exclusive(name));
  }
  Result<std::unique_ptr<JournalFile>> open_existing(std::string_view name,
                                                     FileAccess access) override {
    return wrap(directory_.open_existing(name, access));
  }
  Result<void> synchronize_directory(SyncStrength strength) override {
    return directory_.synchronize_directory(strength);
  }

private:
  Result<std::unique_ptr<JournalFile>>
  wrap(Result<std::unique_ptr<JournalFile>> result) {
    if (!result.has_value()) {
      return result;
    }
    return Result<std::unique_ptr<JournalFile>>::success(
        std::make_unique<ChunkCrashFile>(std::move(result).value(), control_));
  }
  NativeJournalDirectory directory_;
  ChunkCrash &control_;
};
void rejection_crash_case() {
  const TemporaryDirectory workspace;
  const auto child = ::fork();
  CHECK(child >= 0);
  if (child == 0) {
    try {
      ChunkCrash control;
      auto directory = std::make_unique<ChunkCrashDirectory>(
          require(NativeJournalDirectory::open(workspace.path)), control);
      auto state = require(
          RetainedState::create(std::move(directory), "journal", header(), capacity));
      require(
          state->submit({{},
                         ComplaintEvent{id<ComplaintId>(31), id<ParticipantId>(4),
                                        std::vector<std::byte>(934, std::byte{33})}}));
      const auto complaint = [](unsigned char identity) {
        return RetainedEvent{
            {},
            ComplaintEvent{id<ComplaintId>(identity), id<ParticipantId>(4),
                           std::vector<std::byte>(934, std::byte{44})}};
      };
      const std::array requests{complaint(31), complaint(32), complaint(33),
                                complaint(34)};
      control.armed = true;
      static_cast<void>(state->append(state->cursor(), {}, requests));
      ::_exit(96);
    } catch (const std::exception &) {
      ::_exit(97);
    }
  }
  int status = 0;
  pid_t waited = 0;
  do {
    waited = ::waitpid(child, &status, 0);
  } while (waited == -1 && errno == EINTR);
  CHECK(waited == child && WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL);
  auto external = require(NativeJournalDirectory::open(workspace.path));
  auto raw = require(external.open_existing("journal", FileAccess::read_only));
  CHECK(require(raw->extent()) == 5394);
  auto directory = std::make_unique<NativeJournalDirectory>(
      require(NativeJournalDirectory::open(workspace.path)));
  auto state =
      require(RetainedState::open(std::move(directory), "journal", header(), capacity));
  CHECK(state->recovery_report().clean());
  CHECK(state->cursor().sequence == 8 && state->cursor().end_offset == 5394);
  require(state->confirm_recovery());
  CHECK(state->committed_facts().size() == 1);
  CHECK(std::get<ComplaintEvent>(state->committed_facts()[0].event.body).complaint ==
        id<ComplaintId>(31));
  std::vector<std::byte> proposal;
  for (const auto sequence : std::array<std::uint64_t, 4>{3, 4, 5, 7}) {
    const auto chunk = require(state->source({header().journal, sequence}));
    CHECK(chunk.size() == (sequence == 7 ? 904 : 1024));
    proposal.insert(proposal.end(), chunk.begin(), chunk.end());
  }
  CHECK(proposal.size() == 3976);
  const auto magic = std::as_bytes(std::span{"ARPROP01", 8});
  CHECK(std::equal(magic.begin(), magic.end(), proposal.begin()));
  CHECK(proposal[44] == std::byte{4} && proposal.back() == std::byte{44});
  Custody custody;
  require(state->reconcile(custody));
  CHECK(custody.seen == 0 && state->state() == JournalWriterState::live);
  require(state->submit({{}, decision(3)}));
  CHECK(require(raw->extent()) > 5394);
}
void crash_case(Cut cut) {
  const TemporaryDirectory workspace;
  {
    auto directory = require(NativeJournalDirectory::open(workspace.path));
    auto file = require(directory.create_exclusive("effect"));
    require(file->synchronize(SyncStrength::full));
    require(directory.synchronize_directory(SyncStrength::full));
  }
  const auto child = ::fork();
  CHECK(child >= 0);
  if (child == 0) {
    try {
      auto directory = std::make_unique<NativeJournalDirectory>(
          require(NativeJournalDirectory::open(workspace.path)));
      auto state = require(
          RetainedState::create(std::move(directory), "journal", header(), capacity));
      require(state->submit({{}, decision(3)}));
      require(
          state->submit({{},
                         InvocationEvent{id<InvocationId>(9), id<DecisionId>(3),
                                         id<DefinitionGenerationId>(10), original}}));
      require(state->submit(
          {{},
           AttemptAdmissionEvent{id<OperationAttemptId>(11), id<InvocationId>(9),
                                 id<DecisionId>(3), original}}));
      auto external = require(NativeJournalDirectory::open(workspace.path));
      FileEffect effect{external, cut};
      require(state->dispatch(id<OperationAttemptId>(11), effect));
      ::_exit(96); // Reaching the return is a failure, never a successful crash case.
    } catch (const std::exception &) {
      ::_exit(97);
    }
  }
  int status = 0;
  pid_t waited = 0;
  do {
    waited = ::waitpid(child, &status, 0);
  } while (waited == -1 && errno == EINTR);
  CHECK(waited == child && WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL);
  auto external = require(NativeJournalDirectory::open(workspace.path));
  const auto survived = read_effect(external);
  std::vector<std::byte> expected;
  if (cut == Cut::after_effect) {
    expected.resize(19);
    expected[15] = std::byte{11};
    expected[16] = std::byte{0};
    expected[17] = std::byte{255};
    expected[18] = std::byte{10};
  }
  CHECK(survived == expected);
  auto directory = std::make_unique<NativeJournalDirectory>(
      require(NativeJournalDirectory::open(workspace.path)));
  auto state =
      require(RetainedState::open(std::move(directory), "journal", header(), capacity));
  CHECK(state->committed_facts().empty());
  const auto recovered = require(state->attempt(id<OperationAttemptId>(11)));
  CHECK(recovered.opened && !recovered.receipt && recovered.reconciliation_required);
  require(state->confirm_recovery());
  Custody custody;
  require(state->reconcile(custody));
  CHECK(custody.seen == 1);
  FileEffect effect{external, Cut::none};
  const auto resubmitted = require(state->submit(
      {{},
       AttemptAdmissionEvent{id<OperationAttemptId>(11), id<InvocationId>(9),
                             id<DecisionId>(3), original}}));
  CHECK(resubmitted.existing && resubmitted.evidence == RetainedEvidence::recovered);
  CHECK(read_effect(external) == survived);
  CHECK(!require(state->dispatch(id<OperationAttemptId>(11), effect)).dispatched);
  CHECK(effect.calls == 0 && read_effect(external) == survived);
  require(state->submit({{}, decision(13)}));
  require(state->submit({{},
                         RetryEvent{id<DecisionId>(13), id<InvocationId>(9),
                                    id<OperationAttemptId>(12)}}));
  require(state->submit(
      {{},
       AttemptAdmissionEvent{id<OperationAttemptId>(12), id<InvocationId>(9),
                             id<DecisionId>(13), original}}));
  CHECK(require(state->dispatch(id<OperationAttemptId>(12), effect)).dispatched);
  CHECK(effect.calls == 1);
  expected.resize(expected.size() + 19);
  const auto base = expected.size() - 19;
  expected[base + 15] = std::byte{12};
  expected[base + 17] = std::byte{255};
  expected[base + 18] = std::byte{10};
  CHECK(read_effect(external) == expected);
}
} // namespace
int main() {
  try {
    crash_case(Cut::before_effect);
    crash_case(Cut::after_effect);
    rejection_crash_case();
    std::puts("PASS actual process-crash execution consumption and explicit retry");
    return 0;
  } catch (const std::exception &error) {
    std::fprintf(stderr, "FAIL retained execution crash: %s\n", error.what());
    return 1;
  }
}
