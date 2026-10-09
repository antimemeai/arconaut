#include "blackbird/context.hpp"
#include "blackbird/journal_writer.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
using namespace blackbird;
#define CHECK(x)                                                                       \
  do {                                                                                 \
    if (!(x))                                                                          \
      throw std::runtime_error("scratch cleanup line " + std::to_string(__LINE__));    \
  } while (false)
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
int main() {
  char pattern[] = "/tmp/blackbird-scratch-XXXXXX";
  const auto path = mkdtemp(pattern);
  if (!path)
    return 2;
  try {
    const JournalHeader h{id<EnvironmentId>(1),
                          id<AuditStreamId>(2),
                          3,
                          {1024 * 1024, 4 * 1024 * 1024},
                          std::nullopt};
    const JournalCapacity capacity{16 * 1024 * 1024, 10000};
    auto directory = unwrap(NativeJournalDirectory::open(path));
    auto journal = unwrap(FramedJournal::create(directory, "audit", h, capacity));
    auto create = [&](const std::string &name) {
      auto file = unwrap(directory.create_exclusive(name));
      const std::byte byte{42};
      CHECK(unwrap(file->write_at(0, std::span{&byte, 1})) == 1);
    };
    auto exists = [&](const std::string &name) {
      return std::filesystem::symlink_status(std::string{path} + "/" + name).type() !=
             std::filesystem::file_type::not_found;
    };
    for (const auto name :
         {"audit.state.0", "audit.archive.1", "audit.scan.0", "audit.other.tmp.1",
          "audit.state.0.tmp.x", "audit.scan.tmp.1.2", "audit.scan.tmp.1.2.3.4",
          "other.state.0.tmp.1"})
      create(name);
    create("audit.state.1.tmp.12");
    create("audit.archive.0.tmp.13");
    create("audit.scan.tmp.1.14.1");
    create("protected");
    std::filesystem::create_symlink("protected",
                                    std::string{path} + "/audit.state.0.tmp.15");
    std::filesystem::create_directory(std::string{path} + "/audit.archive.0.tmp.16");
    create("audit.state.0.tmp.17");
    std::filesystem::permissions(std::string{path} + "/audit.state.0.tmp.17",
                                 std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::add);
    create("audit.state.0.tmp.18");
    std::filesystem::create_hard_link(std::string{path} + "/audit.state.0.tmp.18",
                                      std::string{path} + "/linked");
    const auto cleaned = unwrap(directory.reclaim_publication_scratch("audit"));
    CHECK(cleaned.complete && cleaned.removed == 3 && cleaned.scanned <= 128);
    for (const auto name :
         {"audit.state.1.tmp.12", "audit.archive.0.tmp.13", "audit.scan.tmp.1.14.1"})
      CHECK(!exists(name));
    for (const auto name :
         {"audit", "audit.state.0", "audit.archive.1", "audit.scan.0",
          "audit.other.tmp.1", "audit.state.0.tmp.x", "audit.scan.tmp.1.2",
          "audit.scan.tmp.1.2.3.4", "other.state.0.tmp.1", "protected",
          "audit.state.0.tmp.15", "audit.archive.0.tmp.16", "audit.state.0.tmp.17",
          "audit.state.0.tmp.18", "linked"})
      CHECK(exists(name));
    CHECK(unwrap(directory.reclaim_publication_scratch("audit")).removed == 0);
    for (unsigned i = 0; i < 20; ++i)
      create("audit.state.0.tmp." + std::to_string(100 + i));
    const auto first = unwrap(directory.reclaim_publication_scratch("audit", 128, 4));
    CHECK(first.removed == 4 && !first.complete);
    CHECK(unwrap(directory.reclaim_publication_scratch("audit", 1, 16)).scanned == 1);
    bool complete = false;
    for (unsigned pass = 0; pass < 4 && !complete; ++pass)
      complete = unwrap(directory.reclaim_publication_scratch("audit")).complete;
    CHECK(complete);
    CHECK(!directory.reclaim_publication_scratch("audit", 129, 1).has_value());
    CHECK(!directory.reclaim_publication_scratch("../audit").has_value());
    // A different journal with an active writer is outside this lease's scope.
    auto other = unwrap(FramedJournal::create(directory, "other", h, capacity));
    create("other.archive.0.tmp.200");
    CHECK(unwrap(directory.reclaim_publication_scratch("audit")).removed == 0);
    CHECK(exists("other.archive.0.tmp.200"));
    auto lookalike =
        unwrap(FramedJournal::create(directory, "audit.state.0.tmp.201", h, capacity));
    CHECK(unwrap(directory.reclaim_publication_scratch("audit")).removed == 0);
    CHECK(exists("audit.state.0.tmp.201"));
    lookalike.reset();
    CHECK(unwrap(directory.reclaim_publication_scratch("audit")).removed == 0);
    CHECK(exists("audit.state.0.tmp.201"));
    create("audit.archive.1.tmp.202");
    auto locked_scratch = unwrap(
        directory.open_existing("audit.archive.1.tmp.202", FileAccess::read_write));
    unwrap(locked_scratch->lock_writer());
    CHECK(unwrap(directory.reclaim_publication_scratch("audit")).removed == 0);
    CHECK(exists("audit.archive.1.tmp.202"));
    locked_scratch.reset();
    CHECK(unwrap(directory.reclaim_publication_scratch("audit")).removed == 1);
    // Advance beyond unrelated names without rescanning the same first window.
    for (unsigned i = 0; i < 300; ++i)
      create("unrelated-" + std::to_string(i));
    for (unsigned i = 0; i < 40; ++i)
      create("audit.state.0.tmp." + std::to_string(500 + i));
    for (unsigned pass = 0; pass < 32; ++pass) {
      const auto progress =
          unwrap(directory.reclaim_publication_scratch("audit", 64, 4));
      CHECK(progress.scanned <= 64 && progress.removed <= 4);
      CHECK(directory.scratch_cleanup_status().scanned == progress.scanned);
    }
    for (unsigned i = 0; i < 40; ++i)
      CHECK(!exists("audit.state.0.tmp." + std::to_string(500 + i)));
    // A new sweep must see files created after an earlier EOF.
    other.reset();
    journal.reset();
    // Real interrupted process: writer lease and file are abandoned, then native
    // FramedJournal reopen reclaims the reserved temporary under its new lease.
    const auto child = fork();
    CHECK(child >= 0);
    if (child == 0) {
      auto opened = unwrap(FramedJournal::open(directory, "audit", h, capacity));
      create("audit.state.1.tmp.300");
      _exit(0);
    }
    int status = 0;
    CHECK(waitpid(child, &status, 0) == child);
    CHECK(WIFEXITED(status) && WEXITSTATUS(status) == 0 &&
          exists("audit.state.1.tmp.300"));
    journal = unwrap(FramedJournal::open(directory, "audit", h, capacity));
    for (unsigned pass = 0; pass < 8 && exists("audit.state.1.tmp.300"); ++pass)
      unwrap(directory.reclaim_publication_scratch("audit"));
    CHECK(!exists("audit.state.1.tmp.300") && exists("audit.state.0") &&
          exists("audit.archive.1"));
    std::cout << "reserved scratch reclaimed; unrelated/selected/linked/symlink files "
                 "preserved; bounded passes and interrupted reopen pass\n";
    std::filesystem::remove_all(path);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    std::filesystem::remove_all(path);
    return 1;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << ':' << e.detail << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
}
