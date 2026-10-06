#include "arconaut/journal_writer.hpp"

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
#define CHECK(expression) check((expression), #expression, __LINE__)
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
    std::array<char, 40> pattern{};
    const std::string text = "/tmp/arconaut-crash-XXXXXX";
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
enum class CrashPoint {
  write_cut,
  before_sync,
  after_sync,
  before_directory_sync,
  after_directory_sync,
  after_return
};
struct CrashControl {
  CrashPoint point;
  std::uint64_t cut;
  bool armed = false;
};
[[noreturn]] void crash() {
  ::kill(::getpid(), SIGKILL);
  ::_exit(98); // Parent requires SIGKILL; this fallback is a failing outcome.
}
// This wrapper cuts actual native writes and kills the writing process. It does
// not synthesize disk contents or claim to simulate a filesystem power failure.
class CrashFile final : public JournalFile {
public:
  CrashFile(std::unique_ptr<JournalFile> file, CrashControl &control)
      : file_(std::move(file)), control_(control) {}
  Result<void> lock_writer() override { return file_->lock_writer(); }
  Result<std::uint64_t> extent() override { return file_->extent(); }
  Result<std::size_t> read_at(std::uint64_t offset, MutableByteView output) override {
    return file_->read_at(offset, output);
  }
  Result<std::size_t> write_at(std::uint64_t offset, ByteView input) override {
    if (control_.armed && control_.point == CrashPoint::write_cut) {
      if (offset >= control_.cut) {
        crash();
      }
      const auto remaining = control_.cut - offset;
      const auto count = std::min(input.size(), static_cast<std::size_t>(remaining));
      auto result = file_->write_at(offset, input.first(count));
      if (result.has_value() && result.value() == remaining) {
        crash();
      }
      return result;
    }
    return file_->write_at(offset, input);
  }
  Result<void> synchronize(SyncStrength strength) override {
    if (control_.armed && control_.point == CrashPoint::before_sync) {
      crash();
    }
    auto result = file_->synchronize(strength);
    if (result.has_value() && control_.armed &&
        control_.point == CrashPoint::after_sync) {
      crash();
    }
    return result;
  }

private:
  std::unique_ptr<JournalFile> file_;
  CrashControl &control_;
};
class CrashDirectory final : public JournalDirectory {
public:
  CrashDirectory(NativeJournalDirectory directory, CrashControl &control)
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
    if (control_.armed && control_.point == CrashPoint::before_directory_sync) {
      crash();
    }
    auto result = directory_.synchronize_directory(strength);
    if (result.has_value() && control_.armed &&
        control_.point == CrashPoint::after_directory_sync) {
      crash();
    }
    return result;
  }

private:
  Result<std::unique_ptr<JournalFile>>
  wrap(Result<std::unique_ptr<JournalFile>> result) {
    if (!result.has_value()) {
      return Result<std::unique_ptr<JournalFile>>::failure(result.error());
    }
    return Result<std::unique_ptr<JournalFile>>::success(
        std::make_unique<CrashFile>(std::move(result).value(), control_));
  }
  NativeJournalDirectory directory_;
  CrashControl &control_;
};
JournalHeader header() {
  return {id<EnvironmentId>(1), id<AuditStreamId>(2), 3, {64, 512}, std::nullopt};
}
constexpr JournalCapacity capacity{4096, 32};
const std::array original{std::byte{0}, std::byte{255}, std::byte{10}};
const std::array drafts{JournalDraft{FrameKind::source, original}};
std::vector<std::byte> read_file(JournalDirectory &directory) {
  auto file = require(directory.open_existing("journal", FileAccess::read_only));
  const auto extent = require(file->extent());
  CHECK(extent <= capacity.max_file_bytes);
  std::vector<std::byte> bytes(static_cast<std::size_t>(extent));
  std::size_t done = 0;
  while (done != bytes.size()) {
    const auto count =
        require(file->read_at(done, MutableByteView{bytes}.subspan(done)));
    CHECK(count != 0 && count <= bytes.size() - done);
    done += count;
  }
  return bytes;
}
void crash_case(CrashPoint point, std::uint64_t cut) {
  const TemporaryDirectory workspace;
  std::vector<std::byte> acknowledged;
  {
    auto directory = require(NativeJournalDirectory::open(workspace.path));
    auto writer =
        require(FramedJournal::create(directory, "journal", header(), capacity));
    CHECK(require(writer->append(drafts)).end_offset == 203);
    acknowledged = read_file(directory);
  }
  const auto child = ::fork();
  CHECK(child >= 0);
  if (child == 0) {
    try {
      CrashControl control{point, cut};
      CrashDirectory directory{require(NativeJournalDirectory::open(workspace.path)),
                               control};
      auto writer =
          require(FramedJournal::open(directory, "journal", header(), capacity));
      require(writer->confirm_recovery());
      require(writer->resume_after_reconciliation());
      control.armed = true;
      CHECK(require(writer->append(drafts)).end_offset == 294);
      CHECK(point == CrashPoint::after_return);
      crash();
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
  auto directory = require(NativeJournalDirectory::open(workspace.path));
  const auto surviving = read_file(directory);
  const auto end = point == CrashPoint::write_cut ? cut : 294;
  CHECK(surviving.size() == end);
  CHECK(std::equal(acknowledged.begin(), acknowledged.end(), surviving.begin()));
  auto recovered =
      require(FramedJournal::open(directory, "journal", header(), capacity));
  const bool complete = end == 294;
  CHECK(recovered->physical_records().empty());
  CHECK(recovered->staged_records().size() == (complete ? 2U : 1U));
  CHECK(recovered->cursor().sequence == (complete ? 4U : 2U));
  CHECK(recovered->cursor().end_offset == (complete ? 294U : 203U));
  CHECK(recovered->recovery_report().clean() == (end == 203 || complete));
  CHECK(recovered->recovery_report().available_end == end);
  CHECK(recovered->pending_records().size() == (!complete && end >= 238 ? 1U : 0U));
  if (!complete && end >= 238) {
    const auto payload =
        require(recovered->read_payload(recovered->pending_records()[0]));
    CHECK(std::equal(original.begin(), original.end(), payload.begin(), payload.end()));
  }
  const auto closed = recovered->append(drafts);
  CHECK(!closed.has_value() && closed.error().code == ErrorCode::audit_unavailable);
  require(recovered->confirm_recovery());
  CHECK(recovered->physical_records().size() == (complete ? 2U : 1U));
  for (const auto &record : recovered->physical_records()) {
    const auto payload = require(recovered->read_payload(record));
    CHECK(std::equal(original.begin(), original.end(), payload.begin(), payload.end()));
  }
  CHECK(read_file(directory) == surviving); // Recovery never truncates originals.
}

void creation_crash(CrashPoint point, std::uint64_t cut) {
  const TemporaryDirectory workspace;
  const auto child = ::fork();
  CHECK(child >= 0);
  if (child == 0) {
    try {
      CrashControl control{point, cut, true};
      CrashDirectory directory{require(NativeJournalDirectory::open(workspace.path)),
                               control};
      auto writer =
          require(FramedJournal::create(directory, "journal", header(), capacity));
      CHECK(writer->state() == JournalWriterState::live);
      CHECK(point == CrashPoint::after_return);
      crash();
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
  auto directory = require(NativeJournalDirectory::open(workspace.path));
  const auto surviving = read_file(directory);
  const auto extent = point == CrashPoint::write_cut ? cut : 112;
  CHECK(surviving.size() == extent);
  const auto replacement =
      FramedJournal::create(directory, "journal", header(), capacity);
  CHECK(!replacement.has_value() && replacement.error().code == ErrorCode::conflict);
  auto opened = FramedJournal::open(directory, "journal", header(), capacity);
  if (extent < 112) {
    CHECK(!opened.has_value() && opened.error().code == ErrorCode::incomplete);
  } else {
    CHECK(opened.has_value());
    auto recovered = std::move(opened).value();
    CHECK(recovered->state() == JournalWriterState::recovery_pending);
    CHECK(recovered->cursor().sequence == 0 && recovered->cursor().end_offset == 112);
    CHECK(recovered->physical_records().empty() && recovered->staged_records().empty());
    const auto closed = recovered->append(drafts);
    CHECK(!closed.has_value() && closed.error().code == ErrorCode::audit_unavailable);
    require(recovered->confirm_recovery());
    CHECK(recovered->state() == JournalWriterState::recovered);
  }
  CHECK(read_file(directory) == surviving);
}
} // namespace

int main() {
  try {
    // First acknowledged batch ends at 203. Second source ends at 238; the
    // following 32-byte commit header ends at 270 and its payload ends at 294.
    for (const auto cut : {203U, 220U, 235U, 236U, 238U, 260U, 270U, 293U, 294U}) {
      crash_case(CrashPoint::write_cut, cut);
    }
    crash_case(CrashPoint::before_sync, 294);
    crash_case(CrashPoint::after_sync, 294);
    crash_case(CrashPoint::after_return, 294);
    for (const auto cut : {0U, 60U, 111U, 112U}) {
      creation_crash(CrashPoint::write_cut, cut);
    }
    creation_crash(CrashPoint::before_sync, 112);
    creation_crash(CrashPoint::after_sync, 112);
    creation_crash(CrashPoint::before_directory_sync, 112);
    creation_crash(CrashPoint::after_directory_sync, 112);
    creation_crash(CrashPoint::after_return, 112);
    std::puts("PASS native SIGKILL cuts, retained prefix and fenced recovery");
    return 0;
  } catch (const std::exception &error) {
    std::fprintf(stderr, "FAIL journal process crash: %s\n", error.what());
    return 1;
  }
}
