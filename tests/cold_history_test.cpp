#include "blackbird/context.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
#define CHECK(x)                                                                       \
  do {                                                                                 \
    if (!(x))                                                                          \
      throw std::runtime_error("cold history line " + std::to_string(__LINE__));       \
  } while (false)
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
int main() {
  char pattern[] = "/tmp/bb-cold-XXXXXX";
  const auto path = mkdtemp(pattern);
  if (!path)
    return 2;
  const JournalHeader h{id<EnvironmentId>(1),
                        id<AuditStreamId>(2),
                        3,
                        {1024 * 1024, 4 * 1024 * 1024},
                        std::nullopt};
  const JournalCapacity cap{32 * 1024 * 1024, 4096};
  auto directory = [&]() {
    return std::make_unique<NativeJournalDirectory>(
        unwrap(NativeJournalDirectory::open(path)));
  };
  try {
    Value view, originals, history;
    std::vector<RetainedEvent> eager;
    std::uint64_t highwater = 0;
    const auto query =
        Value::object({{"kind", Value{"history"}}, {"limit", Value{Number{"65536"}}}});
    {
      auto root = unwrap(RetainedState::create(directory(), "audit", h, cap));
      AuditLog log{*root};
      ContextStore ctx{log};
      for (unsigned i = 0; i < 12; ++i) {
        ctx.append({Value::object({{"role", Value{"assistant"}},
                                   {"content", Value{std::string(100 + i, 'x')}}})},
                   "generated");
        const auto basis = ctx.view();
        auto edit = Value::object(
            {{"base", field(basis, "base")}, {"entries", Value{Value::Array{}}}});
        CHECK(field(ctx.edit(edit), "accepted") == Value{true});
        CHECK(field(ctx.edit(edit), "accepted") == Value{false});
      }
      view = ctx.view();
      originals = ctx.originals();
      history = ctx.inspect(query);
      highwater = root->issuer_counter();
      for (const auto &fact : root->committed_facts())
        eager.push_back(fact.event);
    }
    ImmutableBytes survivor;
    {
      auto root = unwrap(RetainedState::open(directory(), "audit", h, cap));
      unwrap(root->confirm_recovery());
      auto index_directory = directory();
      unwrap(root->enable_indexed_queries(
          unwrap(index_directory->create_exclusive("queries.index")),
          32 * 1024 * 1024));
      CHECK(root->indexed_queries());
      AuditLog log{*root};
      ContextStore ctx{log};
      CHECK(ctx.view() == view && ctx.originals() == originals &&
            ctx.inspect(query) == history);
      CHECK(root->issuer_counter() == highwater &&
            root->committed_facts().size() == eager.size());
      for (std::size_t i = 0; i < eager.size(); ++i) {
        CHECK(unwrap(root->fact(i)).event == eager[i]);
        if (const auto *app = std::get_if<ApplicationRecordEvent>(
                &root->committed_facts()[i].event.body)) {
          CHECK(app->payload.is_cold() && app->payload.resident_bytes() == 0);
          survivor = app->payload;
          CHECK(unwrap(root->submit(eager[i])).existing);
          auto changed = eager[i];
          auto &payload = std::get<ApplicationRecordEvent>(changed.body).payload;
          auto data = unwrap(payload.read());
          data[0] ^= std::byte{1};
          payload = ImmutableBytes{std::move(data)};
          CHECK(!root->submit(changed).has_value());
        }
      }
    }
    const auto exact = unwrap(survivor.read());
    // Old read ownership does not retain writer permission or prevent reopening.
    {
      auto root = unwrap(RetainedState::open(directory(), "audit", h, cap));
      CHECK(unwrap(survivor.read()) == exact);
    }
    auto dir = directory();
    auto journal = unwrap(FramedJournal::open(*dir, "audit", h, cap));
    const auto record = journal->staged_records().front();
    auto bad = record;
    ++bad.payload_offset;
    CHECK(!journal->payload_reader(bad).has_value());
    bad = record;
    ++bad.payload_size;
    CHECK(!journal->payload_reader(bad).has_value());
    bad = record;
    bad.journal = id<AuditStreamId>(9);
    CHECK(!journal->payload_reader(bad).has_value());
    auto reader = unwrap(journal->payload_reader(record));
    const auto owned = unwrap(reader());
    journal.reset();
    CHECK(unwrap(reader()) == owned);
    {
      std::fstream file(std::string(path) + "/audit",
                        std::ios::binary | std::ios::in | std::ios::out);
      file.seekp(static_cast<std::streamoff>(record.payload_offset));
      char c = '!';
      file.write(&c, 1);
    }
    CHECK(!reader().has_value());
    CHECK(!survivor.read().has_value() || unwrap(survivor.read()) == exact);
    auto live = unwrap(RetainedState::open(directory(), "audit", h, cap));
    // The first frame was damaged above: recovery can expose only its valid prefix.
    CHECK(live->state() != JournalWriterState::live);
    live.reset();
    std::filesystem::resize_file(std::string(path) + "/audit", 10);
    CHECK(!reader().has_value());
    CHECK(!survivor.read().has_value());
    auto failed = ImmutableBytes::cold(
        1, [] { return Result<std::vector<std::byte>>::failure({ErrorCode::io}); });
    CHECK(!failed.read().has_value());
    bool error = false;
    try {
      (void)(failed == ImmutableBytes{std::byte{1}});
    } catch (const Error &e) {
      error = e.code == ErrorCode::io;
    }
    CHECK(error);
    auto wrong = ImmutableBytes::cold(
        2, [] { return Result<std::vector<std::byte>>::success({std::byte{1}}); });
    CHECK(!wrong.read().has_value());
    std::filesystem::remove_all(path);
    std::cout << "cold history exact/recovery/duplicates/owner/faults passed\n";
    return 0;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  std::filesystem::remove_all(path);
  return 1;
}
