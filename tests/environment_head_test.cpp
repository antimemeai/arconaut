#include "arconaut/environment_head.hpp"
#include "arconaut/journal_writer.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <map>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace arconaut;
namespace {
std::atomic<std::ptrdiff_t> allocation_cut{-1};
}
void *operator new(std::size_t size) {
  if (allocation_cut.load() >= 0 && allocation_cut.fetch_sub(1) == 0)
    throw std::bad_alloc{};
  if (void *p = std::malloc(size == 0 ? 1 : size))
    return p;
  throw std::bad_alloc{};
}
void operator delete(void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }
void *operator new[](std::size_t n) { return ::operator new(n); }
void operator delete[](void *p) noexcept { ::operator delete(p); }
void operator delete[](void *p, std::size_t) noexcept { ::operator delete(p); }
namespace {
void check(bool ok, const char *expression, int line) {
  if (!ok)
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
template <typename T> T id(unsigned char byte) {
  IdentityBytes bytes{};
  bytes[0] = std::byte{byte};
  return require(T::from_bytes(bytes));
}
JournalHeader root() {
  return {id<EnvironmentId>(1), id<AuditStreamId>(2), 3, {1024, 4096}, std::nullopt};
}
std::vector<std::byte> hex(std::string_view text) {
  std::vector<std::byte> out;
  const auto digit = [](char v) { return v <= '9' ? v - '0' : v - 'a' + 10; };
  for (std::size_t i = 0; i < text.size(); i += 2)
    out.push_back(static_cast<std::byte>((digit(text[i]) << 4) | digit(text[i + 1])));
  return out;
}
// Spec-derived CRC recomputation for intentionally invalid, CRC-valid fields.
void recalculate(std::vector<std::byte> &bytes) {
  std::uint32_t crc = 0xffffffff;
  for (std::size_t i = 0; i < 68; ++i) {
    crc ^= std::to_integer<std::uint32_t>(bytes[i]);
    for (int bit = 0; bit < 8; ++bit)
      crc = (crc >> 1) ^ ((crc & 1) ? 0x82f63b78 : 0);
  }
  crc ^= 0xffffffff;
  for (std::size_t i = 0; i < 4; ++i)
    bytes[68 + i] = static_cast<std::byte>((crc >> (i * 8)) & 255);
}
void codec_cases() {
  const HeadSelector first{root().environment, root().journal, root().journal, 1};
  const auto golden =
      hex("4152434845414431010000000000000000000000000000000200000000000000000000000000"
          "00000200000000000000000000000000000001000000000000000000000032145466");
  const auto encoded = require(encode_head_selector(first));
  CHECK(std::equal(encoded.begin(), encoded.end(), golden.begin(), golden.end()));
  CHECK(require(decode_head_selector(golden)) == first);
  for (std::size_t n = 0; n < golden.size(); ++n)
    CHECK(!decode_head_selector(ByteView{golden}.first(n)).has_value());
  auto broken = golden;
  broken.push_back(std::byte{0});
  CHECK(!decode_head_selector(broken).has_value());
  for (std::size_t n = 0; n < golden.size(); ++n) {
    broken = golden;
    broken[n] ^= std::byte{1};
    CHECK(!decode_head_selector(broken).has_value());
  }
  for (const std::size_t at : {std::size_t{0}, std::size_t{64}, std::size_t{8},
                               std::size_t{24}, std::size_t{40}, std::size_t{56}}) {
    broken = golden;
    if (at == 0 || at == 64)
      broken[at] = std::byte{255};
    else
      std::fill_n(broken.begin() + static_cast<std::ptrdiff_t>(at), at == 56 ? 8 : 16,
                  std::byte{0});
    recalculate(broken);
    error_is(decode_head_selector(broken), ErrorCode::corrupt);
  }
  broken = golden;
  broken[40] = std::byte{4};
  recalculate(broken);
  error_is(decode_head_selector(broken), ErrorCode::corrupt);
  broken = golden;
  broken[56] = std::byte{2};
  recalculate(broken);
  error_is(decode_head_selector(broken), ErrorCode::corrupt);
  auto bad = first;
  bad.generation = 0;
  error_is(encode_head_selector(bad), ErrorCode::invalid_range);
  bad = first;
  bad.active = id<AuditStreamId>(4);
  error_is(encode_head_selector(bad), ErrorCode::invalid_range);
  bad = first;
  bad.generation = 2;
  error_is(encode_head_selector(bad), ErrorCode::invalid_range);
  bad.active = id<AuditStreamId>(4);
  CHECK(require(decode_head_selector(require(encode_head_selector(bad)))) == bad);
}
struct Bytes {
  std::vector<std::byte> value;
  bool locked = false;
};
struct Script {
  std::map<std::string, std::shared_ptr<Bytes>> files;
  std::vector<std::string> calls;
  int fail = 0;
  int mutations = 0;
  bool short_io = false;
  bool interrupt = false;
  bool zero = false;
  bool impossible = false;
  bool throw_write = false;
  int lock_interrupts = 0;
  int read_interrupts = 0;
  int extent_interrupts = 0;
  int sync_interrupts = 0;
  bool allocate_after_directory_sync = false;
  std::optional<Error> head_open_error;
  bool read_zero = false;
  bool read_impossible = false;
  std::function<void()> callback;
  bool cut() { return fail != 0 && ++mutations == fail; }
};
class MemoryFile final : public JournalFile {
public:
  MemoryFile(std::shared_ptr<Script> script, std::string name,
             std::shared_ptr<Bytes> bytes)
      : script_(std::move(script)), name_(std::move(name)), bytes_(std::move(bytes)) {}
  ~MemoryFile() override {
    if (locked_)
      bytes_->locked = false;
  }
  Result<void> lock_writer() override {
    script_->calls.push_back("lock:" + name_);
    if (script_->lock_interrupts > 0) {
      --script_->lock_interrupts;
      return Result<void>::failure({ErrorCode::interrupted});
    }
    if (bytes_->locked)
      return Result<void>::failure({ErrorCode::busy});
    bytes_->locked = true;
    locked_ = true;
    return Result<void>::success();
  }
  Result<std::uint64_t> extent() override {
    script_->calls.push_back("extent:" + name_);
    if (script_->extent_interrupts > 0) {
      --script_->extent_interrupts;
      return Result<std::uint64_t>::failure({ErrorCode::interrupted});
    }
    return Result<std::uint64_t>::success(bytes_->value.size());
  }
  Result<std::size_t> read_at(std::uint64_t offset, MutableByteView out) override {
    script_->calls.push_back("read:" + name_);
    if (script_->interrupt) {
      script_->interrupt = false;
      return Result<std::size_t>::failure({ErrorCode::interrupted});
    }
    if (script_->read_interrupts > 0) {
      --script_->read_interrupts;
      return Result<std::size_t>::failure({ErrorCode::interrupted});
    }
    if (script_->read_zero)
      return Result<std::size_t>::success(0);
    if (script_->read_impossible)
      return Result<std::size_t>::success(out.size() + 1);
    const auto start = static_cast<std::size_t>(offset);
    const auto n = start < bytes_->value.size()
                       ? std::min(out.size(), bytes_->value.size() - start)
                       : 0;
    const auto count = script_->short_io ? std::min<std::size_t>(n, 7) : n;
    std::copy_n(bytes_->value.begin() + static_cast<std::ptrdiff_t>(start), count,
                out.begin());
    return Result<std::size_t>::success(count);
  }
  Result<std::size_t> write_at(std::uint64_t offset, ByteView in) override {
    script_->calls.push_back("write:" + name_);
    if (script_->callback)
      script_->callback();
    if (script_->throw_write)
      throw std::bad_alloc{};
    if (script_->cut())
      return Result<std::size_t>::failure({ErrorCode::io, ENOSPC});
    if (script_->interrupt) {
      script_->interrupt = false;
      return Result<std::size_t>::failure({ErrorCode::interrupted});
    }
    if (script_->zero)
      return Result<std::size_t>::success(0);
    if (script_->impossible)
      return Result<std::size_t>::success(in.size() + 1);
    const auto n = script_->short_io ? std::min<std::size_t>(in.size(), 7) : in.size();
    const auto start = static_cast<std::size_t>(offset);
    bytes_->value.resize(start + n);
    std::copy_n(in.begin(), n,
                bytes_->value.begin() + static_cast<std::ptrdiff_t>(start));
    return Result<std::size_t>::success(n);
  }
  Result<void> synchronize(SyncStrength strength) override {
    CHECK(strength == SyncStrength::full);
    script_->calls.push_back("sync:" + name_);
    if (script_->sync_interrupts > 0) {
      --script_->sync_interrupts;
      return Result<void>::failure({ErrorCode::interrupted});
    }
    return script_->cut() ? Result<void>::failure({ErrorCode::io, ENOSPC})
                          : Result<void>::success();
  }

private:
  std::shared_ptr<Script> script_;
  std::string name_;
  std::shared_ptr<Bytes> bytes_;
  bool locked_ = false;
};
class MemoryDirectory final : public JournalDirectory {
public:
  explicit MemoryDirectory(std::shared_ptr<Script> script)
      : script_(std::move(script)) {}
  Result<std::unique_ptr<JournalFile>>
  create_exclusive(std::string_view name) override {
    script_->calls.push_back("create:" + std::string{name});
    if (script_->files.contains(std::string{name}))
      return Result<std::unique_ptr<JournalFile>>::failure({ErrorCode::conflict});
    auto bytes = std::make_shared<Bytes>();
    script_->files.emplace(name, bytes);
    return Result<std::unique_ptr<JournalFile>>::success(
        std::make_unique<MemoryFile>(script_, std::string{name}, bytes));
  }
  Result<std::unique_ptr<JournalFile>> open_existing(std::string_view name,
                                                     FileAccess) override {
    script_->calls.push_back("open:" + std::string{name});
    if (name == "head" && script_->head_open_error)
      return Result<std::unique_ptr<JournalFile>>::failure(*script_->head_open_error);
    const auto found = script_->files.find(std::string{name});
    if (found == script_->files.end())
      return Result<std::unique_ptr<JournalFile>>::failure({ErrorCode::io, ENOENT});
    return Result<std::unique_ptr<JournalFile>>::success(
        std::make_unique<MemoryFile>(script_, std::string{name}, found->second));
  }
  Result<void> synchronize_directory(SyncStrength strength) override {
    CHECK(strength == SyncStrength::full);
    script_->calls.push_back("dirsync");
    if (script_->cut())
      return Result<void>::failure({ErrorCode::io, ENOSPC});
    if (script_->allocate_after_directory_sync)
      allocation_cut.store(0);
    return Result<void>::success();
  }
  Result<void> replace_file(std::string_view from, std::string_view to) override {
    if (from == to)
      return Result<void>::failure({ErrorCode::invalid_range});
    script_->calls.push_back("rename");
    if (script_->cut())
      return Result<void>::failure({ErrorCode::io});
    const auto f = script_->files.find(std::string{from});
    CHECK(f != script_->files.end());
    script_->files[std::string{to}] = f->second;
    script_->files.erase(f);
    return Result<void>::success();
  }

private:
  std::shared_ptr<Script> script_;
};
auto directory(const std::shared_ptr<Script> &script) {
  return std::make_unique<MemoryDirectory>(script);
}
void prepared_creation_cases() {
  auto s = std::make_shared<Script>();
  auto prepared =
      require(EnvironmentHead::prepare(directory(s), root(), SyncStrength::full));
  CHECK(!prepared->healthy());
  CHECK(s->files.contains("authority") && s->files.at("authority")->locked);
  CHECK(!s->files.contains("head"));
  CHECK(require(decode_journal_header(s->files.at("authority")->value)) == root());
  error_is(prepared->replace(prepared->selection(), id<AuditStreamId>(4),
                             SyncStrength::full),
           ErrorCode::audit_unavailable);
  error_is(EnvironmentHead::open(directory(s), root(), SyncStrength::full),
           ErrorCode::busy);
  error_is(EnvironmentHead::prepare(directory(s), root(), SyncStrength::full),
           ErrorCode::conflict);
  CHECK(s->files.size() == 1); // Second creator cannot touch a root journal.
  auto journal =
      require(FramedJournal::create(prepared->directory(), "root", root(), {4096, 8}));
  CHECK(!prepared->healthy() && !s->files.contains("head"));
  bool reentered = false;
  s->callback = [&] {
    reentered = true;
    CHECK(!prepared->healthy());
    error_is(prepared->publish_initial(SyncStrength::full), ErrorCode::busy);
    error_is(prepared->replace(prepared->selection(), id<AuditStreamId>(4),
                               SyncStrength::full),
             ErrorCode::busy);
  };
  require(prepared->publish_initial(SyncStrength::full));
  s->callback = {};
  CHECK(reentered && prepared->healthy());
  CHECK(require(decode_head_selector(s->files.at("head")->value)) ==
        prepared->selection());
  const auto published_calls = s->calls.size();
  error_is(prepared->publish_initial(SyncStrength::full), ErrorCode::audit_unavailable);
  CHECK(s->calls.size() == published_calls);
  journal.reset(); // Borrowing segment dies before directory/head.
  prepared.reset();
  require(EnvironmentHead::open(directory(s), root(), SyncStrength::full));

  // Every publication boundary consumes this initial operation on failure;
  // only fresh open establishes whether the selector rename actually landed.
  for (int cut = 1; cut <= 4; ++cut) {
    s = std::make_shared<Script>();
    prepared =
        require(EnvironmentHead::prepare(directory(s), root(), SyncStrength::full));
    journal = require(
        FramedJournal::create(prepared->directory(), "root", root(), {4096, 8}));
    const auto original_authority = s->files.at("authority")->value;
    const auto original_root = s->files.at("root")->value;
    s->mutations = 0;
    s->fail = cut;
    error_is(prepared->publish_initial(SyncStrength::full), ErrorCode::io);
    CHECK(!prepared->healthy() && s->files.contains("head") == (cut == 4));
    const auto calls = s->calls.size();
    error_is(prepared->publish_initial(SyncStrength::full),
             ErrorCode::audit_unavailable);
    CHECK(s->calls.size() == calls &&
          s->files.at("authority")->value == original_authority);
    CHECK(s->files.at("root")->value == original_root);
    CHECK(require(journal->read_original_range(0, 64)).bytes ==
          std::vector<std::byte>(original_root.begin(), original_root.begin() + 64));
    journal.reset();
    prepared.reset();
    s->fail = 0;
    if (cut == 4) {
      auto reopened =
          require(EnvironmentHead::open(directory(s), root(), SyncStrength::full));
      CHECK(reopened->healthy() && reopened->selection().generation == 1);
    } else {
      error_is(EnvironmentHead::open(directory(s), root(), SyncStrength::full),
               ErrorCode::io);
    }
  }
  s = std::make_shared<Script>();
  auto invalid = root();
  invalid.issuer_namespace = 0;
  error_is(EnvironmentHead::prepare(directory(s), invalid, SyncStrength::full),
           ErrorCode::invalid_identity);
  CHECK(s->files.empty() && s->calls.empty());
  prepared =
      require(EnvironmentHead::prepare(directory(s), root(), SyncStrength::full));
  auto unexpected = std::make_shared<Bytes>(Bytes{{std::byte{255}}, false});
  s->files.emplace("head", unexpected);
  const auto before = s->calls.size();
  error_is(prepared->publish_initial(SyncStrength::full), ErrorCode::conflict);
  CHECK(s->calls.size() == before + 1 &&
        unexpected->value == std::vector{std::byte{255}});
  CHECK(!prepared->healthy());
  prepared.reset();

  s = std::make_shared<Script>();
  prepared =
      require(EnvironmentHead::prepare(directory(s), root(), SyncStrength::full));
  s->allocate_after_directory_sync = true;
  const auto published = prepared->publish_initial(SyncStrength::full);
  allocation_cut.store(-1);
  require(published);
  CHECK(prepared->healthy());

  s = std::make_shared<Script>();
  prepared =
      require(EnvironmentHead::prepare(directory(s), root(), SyncStrength::full));
  const auto strength_calls = s->calls.size();
  error_is(prepared->publish_initial(static_cast<SyncStrength>(99)),
           ErrorCode::invalid_range);
  CHECK(!prepared->healthy() && s->calls.size() == strength_calls);
  require(prepared->publish_initial(SyncStrength::full));
  prepared.reset();
  for (const bool during_publish : {false, true}) {
    s = std::make_shared<Script>();
    const Error concrete{ErrorCode::io, EACCES};
    if (during_publish) {
      prepared =
          require(EnvironmentHead::prepare(directory(s), root(), SyncStrength::full));
      s->head_open_error = concrete;
      const auto refused = prepared->publish_initial(SyncStrength::full);
      CHECK(!refused.has_value() && refused.error() == concrete);
      CHECK(!prepared->healthy());
      prepared.reset();
    } else {
      s->head_open_error = concrete;
      const auto refused =
          EnvironmentHead::prepare(directory(s), root(), SyncStrength::full);
      CHECK(!refused.has_value() && refused.error() == concrete);
    }
    CHECK(!s->files.contains("head"));
  }
  bool reached_success = false;
  for (std::ptrdiff_t cut = 0; cut < 64; ++cut) {
    s = std::make_shared<Script>();
    prepared =
        require(EnvironmentHead::prepare(directory(s), root(), SyncStrength::full));
    journal = require(
        FramedJournal::create(prepared->directory(), "root", root(), {4096, 8}));
    const auto original_root = s->files.at("root")->value;
    const auto original_authority = s->files.at("authority")->value;
    allocation_cut.store(cut);
    const auto result = prepared->publish_initial(SyncStrength::full);
    allocation_cut.store(-1);
    CHECK(s->files.at("root")->value == original_root &&
          s->files.at("authority")->value == original_authority);
    if (result.has_value()) {
      reached_success = true;
      CHECK(prepared->healthy());
    } else {
      error_is(result, ErrorCode::allocation);
      CHECK(!prepared->healthy());
      const auto no_retry_calls = s->calls.size();
      error_is(prepared->publish_initial(SyncStrength::full),
               ErrorCode::audit_unavailable);
      CHECK(s->calls.size() == no_retry_calls);
    }
    journal.reset();
    prepared.reset();
    if (s->files.contains("head")) {
      CHECK(require(EnvironmentHead::open(directory(s), root(), SyncStrength::full))
                ->healthy());
    } else {
      error_is(EnvironmentHead::open(directory(s), root(), SyncStrength::full),
               ErrorCode::io);
    }
    if (reached_success)
      break;
  }
  CHECK(reached_success);
}
void scripted_cases() {
  auto s = std::make_shared<Script>();
  s->files.emplace("head", std::make_shared<Bytes>(Bytes{{std::byte{255}}, false}));
  error_is(EnvironmentHead::create(directory(s), root(), SyncStrength::full),
           ErrorCode::conflict);
  CHECK(s->files.at("head")->value == std::vector{std::byte{255}});
  s = std::make_shared<Script>();
  auto owner =
      require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
  const auto first = owner->selection();
  s->calls.clear();
  auto wrong = first;
  wrong.generation = 3;
  error_is(owner->replace(wrong, id<AuditStreamId>(4), SyncStrength::full),
           ErrorCode::conflict);
  error_is(owner->replace(first, first.active, SyncStrength::full),
           ErrorCode::conflict);
  CHECK(s->calls.empty());
  CHECK(owner->healthy());
  require(owner->replace(first, id<AuditStreamId>(4), SyncStrength::full));
  CHECK(owner->selection().generation == 2);
  CHECK(owner->selection().active == id<AuditStreamId>(4));
  const auto temp = "head.04000000000000000000000000000000";
  CHECK(s->calls == std::vector<std::string>{
                        "open:head", "extent:head", "read:head",
                        "create:" + std::string{temp}, "write:" + std::string{temp},
                        "sync:" + std::string{temp}, "rename", "dirsync"});
  error_is(EnvironmentHead::open(directory(s), root(), SyncStrength::full),
           ErrorCode::busy);
  owner.reset();
  s->calls.clear();
  owner = require(EnvironmentHead::open(directory(s), root(), SyncStrength::full));
  CHECK(owner->selection().generation == 2);
  CHECK(s->calls.size() > 3 && s->calls[0] == "open:authority" &&
        s->calls[1] == "lock:authority" && s->calls[2] == "extent:authority");
  owner.reset();
  for (int cut = 1; cut <= 4; ++cut) {
    s = std::make_shared<Script>();
    owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
    s->fail = cut;
    s->mutations = 0;
    CHECK(!owner->replace(first, id<AuditStreamId>(4), SyncStrength::full).has_value());
    CHECK(!owner->healthy());
    CHECK(owner->selection() == first);
    const auto calls = s->calls.size();
    error_is(owner->replace(first, id<AuditStreamId>(5), SyncStrength::full),
             ErrorCode::audit_unavailable);
    CHECK(s->calls.size() == calls);
    owner.reset();
    s->fail = 0;
    owner = require(EnvironmentHead::open(directory(s), root(), SyncStrength::full));
    CHECK(owner->selection().active == id<AuditStreamId>(cut == 4 ? 4 : 2));
    CHECK(owner->selection().generation ==
          static_cast<std::uint64_t>(cut == 4 ? 2 : 1));
    CHECK(owner->healthy());
    owner.reset();
  }
  for (int fault = 0; fault < 3; ++fault) {
    s = std::make_shared<Script>();
    owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
    s->zero = fault == 0;
    s->impossible = fault == 1;
    s->throw_write = fault == 2;
    CHECK(!owner->replace(first, id<AuditStreamId>(4), SyncStrength::full).has_value());
    CHECK(!owner->healthy());
    CHECK(owner->selection() == first);
    owner.reset();
  }
  s = std::make_shared<Script>();
  owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
  s->callback = [&owner, first] {
    error_is(owner->replace(first, id<AuditStreamId>(5), SyncStrength::full),
             ErrorCode::busy);
  };
  require(owner->replace(first, id<AuditStreamId>(4), SyncStrength::full));
  CHECK(owner->healthy());
  owner.reset();
  bool reached_success = false;
  for (std::ptrdiff_t cut = 0; cut < 96; ++cut) {
    s = std::make_shared<Script>();
    owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
    allocation_cut = cut;
    const auto result = owner->replace(first, id<AuditStreamId>(4), SyncStrength::full);
    allocation_cut = -1;
    if (result.has_value()) {
      reached_success = true;
      CHECK(owner->selection().active == id<AuditStreamId>(4));
      break;
    }
    error_is(result, ErrorCode::allocation);
    CHECK(owner->selection() == first);
    if (!owner->healthy()) {
      const auto n = s->calls.size();
      error_is(owner->replace(first, id<AuditStreamId>(5), SyncStrength::full),
               ErrorCode::audit_unavailable);
      CHECK(s->calls.size() == n);
    }
    owner.reset();
  }
  CHECK(reached_success);
  owner.reset();
  s = std::make_shared<Script>();
  s->short_io = true;
  s->interrupt = true;
  owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
  s->interrupt = true;
  require(owner->replace(first, id<AuditStreamId>(4), SyncStrength::full));
  owner.reset();
  s->interrupt = true;
  owner = require(EnvironmentHead::open(directory(s), root(), SyncStrength::full));
  CHECK(owner->selection().active == id<AuditStreamId>(4));
  owner.reset();
  s->short_io = false;
  auto wrong_root = root();
  wrong_root.environment = id<EnvironmentId>(9);
  error_is(EnvironmentHead::open(directory(s), wrong_root, SyncStrength::full),
           ErrorCode::wrong_environment);
  wrong_root = root();
  wrong_root.issuer_namespace = 99;
  error_is(EnvironmentHead::open(directory(s), wrong_root, SyncStrength::full),
           ErrorCode::conflict);
  s->files.at("head")->value[0] ^= std::byte{1};
  error_is(EnvironmentHead::open(directory(s), root(), SyncStrength::full),
           ErrorCode::corrupt);
  s->files.erase("head");
  CHECK(!EnvironmentHead::open(directory(s), root(), SyncStrength::full).has_value());
  CHECK(!s->files.contains("head"));
  s = std::make_shared<Script>();
  owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
  s->files.emplace(temp, std::make_shared<Bytes>(Bytes{{std::byte{255}}, false}));
  error_is(owner->replace(first, id<AuditStreamId>(4), SyncStrength::full),
           ErrorCode::conflict);
  CHECK(owner->healthy());
  CHECK(s->files.at(temp)->value == std::vector{std::byte{255}});
  owner.reset();
  // Loss of a selector while this owner is alive must not recreate it.
  s = std::make_shared<Script>();
  owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
  s->files.erase("head");
  CHECK(!owner->replace(first, id<AuditStreamId>(4), SyncStrength::full).has_value());
  CHECK(!owner->healthy());
  CHECK(!s->files.contains("head"));
  owner.reset();
  s = std::make_shared<Script>();
  owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
  owner.reset();
  for (int fault = 0; fault < 4; ++fault) {
    s = std::make_shared<Script>();
    owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
    owner.reset();
    s->lock_interrupts = fault == 0 ? 8 : 1;
    s->read_interrupts = fault == 1 ? 8 : 1;
    s->read_zero = fault == 2;
    s->read_impossible = fault == 3;
    const auto failed = EnvironmentHead::open(directory(s), root(), SyncStrength::full);
    error_is(failed, fault < 2    ? ErrorCode::interrupted
                     : fault == 2 ? ErrorCode::incomplete
                                  : ErrorCode::corrupt);
  }
  s = std::make_shared<Script>();
  owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
  owner.reset();
  s->lock_interrupts = 1;
  s->read_interrupts = 1;
  owner = require(EnvironmentHead::open(directory(s), root(), SyncStrength::full));
  owner.reset();
  for (const bool is_extent : {false, true}) {
    s = std::make_shared<Script>();
    owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
    owner.reset();
    s->extent_interrupts = is_extent ? 8 : 0;
    s->sync_interrupts = is_extent ? 0 : 8;
    error_is(EnvironmentHead::open(directory(s), root(), SyncStrength::full),
             ErrorCode::interrupted);
    s->extent_interrupts = 1;
    s->sync_interrupts = 1;
    owner = require(EnvironmentHead::open(directory(s), root(), SyncStrength::full));
    owner.reset();
  }
  for (const bool is_environment : {false, true}) {
    s = std::make_shared<Script>();
    owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
    owner.reset();
    auto &stored = s->files.at("head")->value;
    stored[is_environment ? 8 : 24] = std::byte{9};
    if (!is_environment) {
      stored[40] = std::byte{4};
      stored[56] = std::byte{2};
    }
    recalculate(stored);
    error_is(EnvironmentHead::open(directory(s), root(), SyncStrength::full),
             is_environment ? ErrorCode::wrong_environment : ErrorCode::conflict);
  }
  s = std::make_shared<Script>();
  owner = require(EnvironmentHead::create(directory(s), root(), SyncStrength::full));
  owner.reset();
  const HeadSelector exhausted{root().environment, root().journal, id<AuditStreamId>(4),
                               UINT64_MAX};
  const auto encoded = require(encode_head_selector(exhausted));
  s->files.at("head")->value.assign(encoded.begin(), encoded.end());
  owner = require(EnvironmentHead::open(directory(s), root(), SyncStrength::full));
  s->calls.clear();
  error_is(owner->replace(exhausted, id<AuditStreamId>(5), SyncStrength::full),
           ErrorCode::overflow);
  CHECK(s->calls.empty());
  CHECK(owner->healthy());
}
class TemporaryDirectory {
public:
  TemporaryDirectory() {
    std::array<char, 32> pattern{};
    const std::string p = "/tmp/arco-head-XXXXXX";
    std::copy(p.begin(), p.end(), pattern.begin());
    CHECK(::mkdtemp(pattern.data()) != nullptr);
    path = pattern.data();
  }
  ~TemporaryDirectory() {
    std::error_code ignored;
    std::filesystem::remove_all(path, ignored);
  }
  std::string path;
};
auto native(const std::string &path) {
  return std::make_unique<NativeJournalDirectory>(
      require(NativeJournalDirectory::open(path)));
}
enum class ExpectedExit { normal, killed };
void wait_child(pid_t child, ExpectedExit expected) {
  int status = 0;
  pid_t waited = 0;
  do {
    waited = ::waitpid(child, &status, 0);
  } while (waited == -1 && errno == EINTR);
  CHECK(waited == child);
  if (expected == ExpectedExit::killed)
    CHECK(WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL);
  else
    CHECK(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}
void contender(const std::string &path) {
  const auto child = ::fork();
  CHECK(child >= 0);
  if (child == 0) {
    const auto attempt =
        EnvironmentHead::open(native(path), root(), SyncStrength::full);
    ::_exit(!attempt.has_value() && attempt.error().code == ErrorCode::busy ? 0 : 3);
  }
  wait_child(child, ExpectedExit::normal);
}
class CrashDirectory final : public JournalDirectory {
public:
  CrashDirectory(std::unique_ptr<JournalDirectory> owner, int cut)
      : owner_(std::move(owner)), cut_(cut) {}
  Result<std::unique_ptr<JournalFile>> create_exclusive(std::string_view n) override {
    return owner_->create_exclusive(n);
  }
  Result<std::unique_ptr<JournalFile>> open_existing(std::string_view n,
                                                     FileAccess a) override {
    return owner_->open_existing(n, a);
  }
  Result<void> synchronize_directory(SyncStrength s) override {
    const auto result = owner_->synchronize_directory(s);
    if (replacing_ && cut_ == 2 && result.has_value()) {
      CHECK(::kill(::getpid(), SIGKILL) == 0);
      ::_exit(8);
    }
    return result;
  }
  Result<void> replace_file(std::string_view from, std::string_view to) override {
    replacing_ = true;
    if (cut_ == 0) {
      CHECK(::kill(::getpid(), SIGKILL) == 0);
      ::_exit(4);
    }
    const auto result = owner_->replace_file(from, to);
    if (!result.has_value())
      ::_exit(5);
    if (cut_ == 1) {
      CHECK(::kill(::getpid(), SIGKILL) == 0);
      ::_exit(6);
    }
    return result;
  }

private:
  std::unique_ptr<JournalDirectory> owner_;
  int cut_;
  bool replacing_ = false;
};
void native_cases() {
  const TemporaryDirectory workspace;
  {
    auto dir = native(workspace.path);
    auto a = require(dir->create_exclusive("a"));
    const std::array value{std::byte{0}, std::byte{255}, std::byte{10}};
    CHECK(require(a->write_at(0, value)) == 3);
    error_is(dir->replace_file("../a", "b"), ErrorCode::invalid_range);
    error_is(dir->replace_file("a", "a"), ErrorCode::invalid_range);
    CHECK(::symlink("a", (workspace.path + "/alias").c_str()) == 0);
    CHECK(!dir->replace_file("alias", "b").has_value());
    CHECK(!dir->replace_file("a", "alias").has_value());
    CHECK(::link((workspace.path + "/a").c_str(), (workspace.path + "/hard").c_str()) ==
          0);
    error_is(dir->replace_file("a", "hard"), ErrorCode::conflict);
    require(dir->replace_file("a", "b"));
    CHECK(require(a->extent()) == 3);
    auto b = require(dir->open_existing("b", FileAccess::read_only));
    std::array<std::byte, 3> read{};
    CHECK(require(b->read_at(0, read)) == 3);
    CHECK(read == value);
    CHECK(::chmod((workspace.path + "/b").c_str(), 0644) == 0);
    CHECK(!dir->replace_file("b", "c").has_value());
  }
  auto owner = require(
      EnvironmentHead::create(native(workspace.path), root(), SyncStrength::full));
  contender(workspace.path);
  require(owner->replace(owner->selection(), id<AuditStreamId>(4), SyncStrength::full));
  contender(workspace.path);
  owner.reset();
  owner = require(
      EnvironmentHead::open(native(workspace.path), root(), SyncStrength::full));
  CHECK(owner->selection().active == id<AuditStreamId>(4));
  owner.reset();
  for (const int cut : {0, 1, 2}) {
    const TemporaryDirectory crash_workspace;
    owner = require(EnvironmentHead::create(native(crash_workspace.path), root(),
                                            SyncStrength::full));
    owner.reset();
    const auto child = ::fork();
    CHECK(child >= 0);
    if (child == 0) {
      auto crash = std::make_unique<CrashDirectory>(native(crash_workspace.path), cut);
      auto child_owner =
          require(EnvironmentHead::open(std::move(crash), root(), SyncStrength::full));
      (void)child_owner->replace(child_owner->selection(), id<AuditStreamId>(4),
                                 SyncStrength::full);
      ::_exit(7);
    }
    wait_child(child, ExpectedExit::killed);
    owner = require(EnvironmentHead::open(native(crash_workspace.path), root(),
                                          SyncStrength::full));
    CHECK(owner->selection().active == id<AuditStreamId>(cut != 0 ? 4 : 2));
    CHECK(owner->selection().generation ==
          static_cast<std::uint64_t>(cut != 0 ? 2 : 1));
    owner.reset();
  }
}
void native_prepared_creation() {
  const TemporaryDirectory workspace;
  auto prepared = require(
      EnvironmentHead::prepare(native(workspace.path), root(), SyncStrength::full));
  CHECK(!prepared->healthy());
  contender(workspace.path); // Authority excludes another process before root exists.
  auto journal =
      require(FramedJournal::create(prepared->directory(), "root", root(), {4096, 8}));
  const std::array payload{std::byte{0}, std::byte{255}, std::byte{10}};
  const std::array drafts{JournalDraft{FrameKind::source, payload}};
  require(journal->append(drafts));
  contender(workspace.path);
  require(prepared->publish_initial(SyncStrength::full));
  contender(workspace.path);
  journal.reset();
  prepared.reset();
  auto reopened = require(
      EnvironmentHead::open(native(workspace.path), root(), SyncStrength::full));
  CHECK(reopened->healthy() && reopened->selection().generation == 1);
  reopened.reset();
  for (const int cut : {0, 1, 2}) {
    const TemporaryDirectory crash_workspace;
    const auto child = ::fork();
    CHECK(child >= 0);
    if (child == 0) {
      auto storage =
          std::make_unique<CrashDirectory>(native(crash_workspace.path), cut);
      auto creating = require(
          EnvironmentHead::prepare(std::move(storage), root(), SyncStrength::full));
      auto original = require(
          FramedJournal::create(creating->directory(), "root", root(), {4096, 8}));
      require(original->append(drafts));
      (void)creating->publish_initial(SyncStrength::full);
      ::_exit(7);
    }
    wait_child(child, ExpectedExit::killed);
    auto actual =
        EnvironmentHead::open(native(crash_workspace.path), root(), SyncStrength::full);
    if (cut == 0) {
      error_is(actual, ErrorCode::io); // No selector; never infer one from orphan root.
    } else {
      CHECK(actual.has_value() && actual.value()->healthy());
      CHECK(actual.value()->selection().active == root().journal &&
            actual.value()->selection().generation == 1);
    }
    // Independently inspect known test originals; this does not activate an
    // environment.
    auto storage = native(crash_workspace.path);
    auto retained = require(FramedJournal::open(*storage, "root", root(), {4096, 8}));
    CHECK(retained->cursor().sequence == 2 && retained->cursor().end_offset == 203);
    CHECK(require(retained->read_payload(retained->staged_records()[0])) ==
          std::vector<std::byte>(payload.begin(), payload.end()));
  }
}
} // namespace
int main() {
  try {
    codec_cases();
    prepared_creation_cases();
    scripted_cases();
    native_cases();
    native_prepared_creation();
    std::puts(
        "PASS head bytes, permanent custody, failure ordering and native SIGKILL");
    return 0;
  } catch (const std::exception &error) {
    std::fprintf(stderr, "FAIL environment head: %s\n", error.what());
    return 1;
  }
}
