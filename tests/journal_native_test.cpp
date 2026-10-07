#include "blackbird/journal_writer.hpp"

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace blackbird;

namespace {
void check(bool condition, const char *expression, int line) {
  if (!condition) {
    throw std::runtime_error(std::to_string(line) + ": " + expression);
  }
}
#define CHECK(expression) check((expression), #expression, __LINE__)
template <typename T> T require(Result<T> result) {
  CHECK(result.has_value());
  return std::move(result).value();
}
void require(Result<void> result) { CHECK(result.has_value()); }
template <typename T> void error_is(const Result<T> &result, ErrorCode code) {
  CHECK(!result.has_value());
  CHECK(result.error().code == code);
}
class TemporaryDirectory {
public:
  TemporaryDirectory() {
    std::array<char, 32> pattern{};
    const std::string text = "/tmp/arconaut-native-XXXXXX";
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

void native_cases() {
  const TemporaryDirectory workspace;
  auto directory = require(NativeJournalDirectory::open(workspace.path));
  auto file = require(directory.create_exclusive("journal"));
  error_is(directory.create_exclusive("journal"), ErrorCode::conflict);
  error_is(directory.create_exclusive("../escape"), ErrorCode::invalid_range);
  error_is(directory.open_existing("missing", FileAccess::read_only), ErrorCode::io);
  const std::array input{std::byte{0}, std::byte{255}, std::byte{13}, std::byte{10}};
  CHECK(require(file->write_at(0, input)) == 4);
  CHECK(require(file->extent()) == 4);
  require(file->synchronize(SyncStrength::data));
  require(file->synchronize(SyncStrength::full));
  require(directory.synchronize_directory(SyncStrength::full));
  std::array<std::byte, 8> output{};
  CHECK(require(file->read_at(0, output)) == 4);
  CHECK(std::equal(input.begin(), input.end(), output.begin()));
  CHECK(require(file->read_at(4, output)) == 0);
  error_is(file->write_at(UINT64_MAX, input), ErrorCode::invalid_range);
  require(file->lock_writer());
  auto contender = require(directory.open_existing("journal", FileAccess::read_write));
  error_is(contender->lock_writer(), ErrorCode::busy);
  const auto child = ::fork();
  CHECK(child >= 0);
  if (child == 0) {
    // Independent open, not the inherited shared open-file description.
    auto child_directory = NativeJournalDirectory::open(workspace.path);
    if (!child_directory.has_value()) {
      ::_exit(2);
    }
    auto opener = std::move(child_directory).value();
    auto attempted = opener.open_existing("journal", FileAccess::read_write);
    if (!attempted.has_value()) {
      ::_exit(3);
    }
    auto independent = std::move(attempted).value();
    const auto locked = independent->lock_writer();
    ::_exit(!locked.has_value() && locked.error().code == ErrorCode::busy ? 0 : 4);
  }
  int status = 0;
  pid_t waited = 0;
  do {
    waited = ::waitpid(child, &status, 0);
  } while (waited == -1 && errno == EINTR);
  CHECK(waited == child && WIFEXITED(status) && WEXITSTATUS(status) == 0);
  file.reset();
  require(contender->lock_writer());
  auto read_only = require(directory.open_existing("journal", FileAccess::read_only));
  error_is(read_only->lock_writer(), ErrorCode::busy);
  const auto failure = read_only->write_at(0, input);
  error_is(failure, ErrorCode::io);
  CHECK(failure.error().detail == EBADF);
  CHECK(::truncate((workspace.path + "/journal").c_str(), 2) == 0);
  output.fill(std::byte{170});
  CHECK(require(read_only->read_at(0, output)) == 2);
  CHECK(output[0] == std::byte{0} && output[1] == std::byte{255});
  for (std::size_t i = 2; i < output.size(); ++i) {
    CHECK(output[i] == std::byte{170});
  }
  CHECK(::symlink("journal", (workspace.path + "/alias").c_str()) == 0);
  const auto linked = directory.open_existing("alias", FileAccess::read_only);
  error_is(linked, ErrorCode::io);
  CHECK(linked.error().detail == ELOOP);
  auto move_owner = require(directory.create_exclusive("move"));
  auto moved = std::move(static_cast<NativeJournalFile &>(*move_owner));
  error_is(move_owner->extent(), ErrorCode::io);
  CHECK(require(moved.write_at(0, input)) == 4);
  CHECK(require(moved.extent()) == 4);
  struct stat metadata{};
  CHECK(::stat((workspace.path + "/journal").c_str(), &metadata) == 0);
  CHECK((metadata.st_mode & 0777) == 0600);
  CHECK(::chmod((workspace.path + "/journal").c_str(), 0644) == 0);
  error_is(directory.open_existing("journal", FileAccess::read_only), ErrorCode::io);
  CHECK(::mkfifo((workspace.path + "/fifo").c_str(), 0600) == 0);
  // Unsupported nonregular paths must reject without waiting for a writer.
  error_is(directory.open_existing("fifo", FileAccess::read_only), ErrorCode::io);
}
void restage_keeps_native_lease() {
  const TemporaryDirectory workspace;
  auto directory = require(NativeJournalDirectory::open(workspace.path));
  IdentityBytes environment_bytes{}, journal_bytes{};
  environment_bytes[15] = std::byte{1};
  journal_bytes[15] = std::byte{2};
  const JournalHeader header{require(EnvironmentId::from_bytes(environment_bytes)),
                             require(AuditStreamId::from_bytes(journal_bytes)),
                             3,
                             {64, 512},
                             std::nullopt};
  auto writer =
      require(FramedJournal::create(directory, "journal", header, {4096, 32}));
  const std::array payload{std::byte{1}};
  const std::array drafts{JournalDraft{FrameKind::source, payload}};
  require(writer->append(drafts));
  auto contender = require(directory.open_existing("journal", FileAccess::read_write));
  error_is(contender->lock_writer(), ErrorCode::busy);
  require(writer->restage());
  error_is(contender->lock_writer(), ErrorCode::busy);
  require(writer->select_recovered_prefix({header.journal, 2, 201}));
  require(writer->confirm_recovery());
  error_is(contender->lock_writer(), ErrorCode::busy);
  error_is(writer->resume_after_reconciliation(), ErrorCode::audit_unavailable);
  writer.reset();
  require(contender->lock_writer());
}
} // namespace

int main() {
  try {
    native_cases();
    restage_keeps_native_lease();
    std::puts("PASS native restricted storage, binary I/O, sync and writer contention");
    return 0;
  } catch (const std::exception &error) {
    std::fprintf(stderr, "FAIL native journal storage: %s\n", error.what());
    return 1;
  }
}
