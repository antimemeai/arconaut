#include "blackbird/json.hpp"
// Development-only synthetic sessions for actual launcher startup measurements.
#include "blackbird/coding.hpp"
#include "blackbird/terminal.hpp"
#include <filesystem>
#include <iostream>
using namespace blackbird;
template <class T> T fixed(unsigned char n) {
  IdentityBytes bytes{};
  bytes[0] = std::byte{n};
  return unwrap(T::from_bytes(bytes));
}
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  try {
    const std::string mode{argv[1]};
    const std::filesystem::path path{argv[2]};
    std::filesystem::create_directories(path);
    std::filesystem::permissions(path, std::filesystem::perms::owner_all);
    JournalHeader header{fixed<EnvironmentId>(1),
                         fixed<AuditStreamId>(2),
                         3,
                         {32 * 1024 * 1024, 96 * 1024 * 1024},
                         std::nullopt};
    auto root =
        unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                         NativeJournalDirectory::open(path.string()))),
                                     "audit", header, {512ULL * 1024 * 1024, 200000}));
    AuditLog log{*root};
    ContextStore context{log};
    Value::Array items;
    const std::string code = "// source evidence\nstruct Item { unsigned id; };\n"
                             "bool ready(const Item& item) { return item.id != 0; }\n";
    if (mode != "heavy" && mode != "live" && mode != "weird")
      return 2;
    std::size_t live_bytes = 0;
    for (unsigned i = 0; i < (mode != "weird" ? 64U : 32U); ++i) {
      std::string content;
      if (mode == "weird")
        content = "Unicode: 日本語 · 🐦 · é · tabs\tand code\n";
      while (content.size() < 4096)
        content += code;
      content.resize(4096);
      live_bytes += content.size();
      items.push_back(Value::object({{"role", Value{i % 2 ? "assistant" : "user"}},
                                     {"content", Value{content}}}));
    }
    context.append(items, "benchmark.synthetic");
    if (mode == "heavy") {
      const std::string discarded(512 * 1024, 'x');
      for (unsigned i = 0; i < 640; ++i) {
        const auto revision = log.issue();
        log.record(revision, ApplicationChannel::context,
                   Value::object({{"op", Value{"edit"}},
                                  {"revision", Value{hex_identity(revision.bytes())}},
                                  {"observed", Value{context.head()}},
                                  {"accepted", Value{false}},
                                  {"candidate", Value{discarded}},
                                  {"entries", Value{Value::Array{}}}}));
      }
    } else if (mode == "weird") {
      for (unsigned i = 0; i < 2048; ++i)
        log.record(ApplicationChannel::log,
                   Value::object({{"label", Value{"benchmark.historical-event"}},
                                  {"ordinal", Value{Number{std::to_string(i)}}}}));
      SessionStore session{log};
      session.restart("benchmark pending RRC; no effect in flight");
      TerminalState state;
      state.draft = "saved Unicode draft 日本語 🐦";
      state.cursor = state.draft.size();
      for (unsigned i = 0; i < 128; ++i)
        state.history.push_back("retained prompt " + std::to_string(i));
      state.queued = {"saved queued draft; must not auto-dispatch"};
      save_terminal_state(path / "ui-state.json", state);
    }
    std::cout << unwrap(dump_json(Value::object(
                     {{"scenario", Value{mode}},
                      {"archive_bytes",
                       Value{Number{std::to_string(
                           std::filesystem::file_size(path / "audit"))}}},
                      {"live_content_bytes", Value{Number{std::to_string(live_bytes)}}},
                      {"facts", Value{Number{std::to_string(root->fact_count())}}}})))
              << '\n';
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
    return 1;
  }
}
