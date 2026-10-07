#include "blackbird/backstop.hpp"
#include "blackbird/process_lifetime.hpp"
#include <fstream>
#include <set>
#include <sys/stat.h>
#include <sys/random.h>
#include <unistd.h>
namespace blackbird {
namespace {
Json number(std::uint64_t n) { return Json{JsonNumber{std::to_string(n)}}; }
template <class T> T random_identity() {
  IdentityBytes bytes{};
  if (::getentropy(bytes.data(), bytes.size()) != 0)
    throw Error{ErrorCode::io, errno};
  return unwrap(T::from_bytes(bytes));
}
constexpr std::string_view assessment = R"lua(
local response = blackbird.request({tools=blackbird.json.decode('[]'), retry_policy={max_attempts=1}})
local text = ''
for _, item in ipairs(response.output) do
  if item.type == 'function_call' then error('Assessment must not dispatch tools') end
  if item.type == 'message' then
    for _, part in ipairs(item.content) do
      if part.type == 'output_text' then text = text .. part.text end
    end
  end
end
return blackbird.json.decode(text)
)lua";
constexpr std::string_view instruction =
    "Independent backstop assessment. Source workflow failed; its audit is preserved "
    "and locally quiescent under cooperative native custody. Local quiescence says "
    "nothing about remote success. Imported attempts have NO replay authority. "
    "Mission packet is author-declared, not verified inherited context. Preserve "
    "dirty work and unknowns. Choose a changed approach or independent useful work "
    "where a resource is blocked. Return only JSON, no markdown, with string fields "
    "retained, uncertain, change, next, action (an ordinary Blackbird Lua turn program), "
    "and oracle:{path:absolute file path,content:exact expected new bytes}. Action "
    "must do useful work for the mission and produce the declared artifact, not a "
    "recovery-status facade. Use blackbird.call/read_file to reconcile uncertain file "
    "effects rather than replay them. Arbitrary exec containment is unavailable: "
    "exec/restart and provider requests during action are refused in recovery; use "
    "native file/context APIs. Two-minute total deadline. File mutations "
    "and oracles inside the predecessor session tree (including aliases) are refused. "
    "Calls return errors as JSON; check "
    "them. You have at most two assessment/pivot attempts, one request per assessment. "
    "Keep action and expected artifact small. The oracle must be absent or different "
    "before action; stale output does not establish progress. Explicit pause stays paused.";
void check_pause(const std::function<bool()> &paused) {
  if (paused && paused())
    throw Error{ErrorCode::interrupted};
}
}
Json run_backstop(CodingEngine &source_engine, AuditLog &source_log,
                  const std::filesystem::path &source_session, const Json &mission,
                  CodingProvider &assessor, const SessionSettings &settings,
                  const std::function<bool()> &paused,
                  const std::function<void(std::string_view)> &display) {
  check_pause(paused);
  if (!std::holds_alternative<Json::Object>(mission.value()) ||
      unwrap(dump_json(mission)).size() > 32768 ||
      string_field(mission, "mission").empty())
    throw Error{ErrorCode::invalid_range};
  const auto cause = source_engine.claim_backstop();
  const auto source_path = std::filesystem::absolute(source_session).lexically_normal();
  const auto destination = source_path / "backstop";
  // Atomic intent creation while still holding native predecessor custody. Never
  // delete/replace this intent, even after crash; reopen needs explicit inspection.
  if (::mkdir(destination.c_str(), 0700) != 0)
    throw Error{errno == EEXIST ? ErrorCode::conflict : ErrorCode::io, errno};
  const auto cursor = source_log.root().cursor();
  std::array<std::byte, journal_header_size> bytes{};
  std::ifstream file{source_path / "audit", std::ios::binary};
  file.read(reinterpret_cast<char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
  if (!file)
    throw Error{ErrorCode::incomplete};
  const auto source_header = unwrap(decode_journal_header(bytes));
  Json lineage = Json::object({
      {"session", Json{source_path.string()}},
      {"environment", Json{hex_identity(source_header.environment.bytes())}},
      {"journal", Json{hex_identity(cursor.journal.bytes())}},
      {"prefix_sequence", number(cursor.sequence)}, {"prefix_end", number(cursor.end_offset)},
      {"context_revision", Json{source_engine.context().head()}},
      {"status", Json{"unsettled"}},
      {"reason", Json{"Cooperative backstop: remote effects remain unknown; no inherited admissions"}}});
  Json::Array selected;
  const auto view = source_engine.context().view();
  // Preserve all governing messages and the latest operator prompt, not the huge
  // predecessor working presentation. Refuse oversized authority, never truncate.
  const auto &entries = field(view, "entries").array();
  const Json *last_user = nullptr;
  for (const auto &entry : entries) {
    const auto &item = field(entry, "item");
    const auto *role = item.find("role");
    if (!role) continue;
    if (role->string() == "system" || role->string() == "developer")
      selected.push_back(entry);
    if (role->string() == "user") last_user = &entry;
  }
  if (last_user) selected.push_back(*last_user);
  if (selected.empty() || unwrap(dump_json(Json{selected})).size() > 65536)
    throw Error{ErrorCode::capacity};
  Json::Array attempts;
  std::size_t total = 0;
  for (const auto &fact : source_log.root().committed_facts()) {
    const auto *admission = std::get_if<AttemptAdmissionEvent>(&fact.event.body);
    if (!admission) continue;
    ++total;
    if (attempts.size() == 32) attempts.erase(attempts.begin());
    const auto state = unwrap(source_log.root().attempt(admission->attempt));
    const bool unknown = !state.observation ||
        state.observation->disposition == AttemptDisposition::unknown;
    attempts.push_back(Json::object({
        {"attempt", Json{hex_identity(admission->attempt.bytes())}},
        {"input", Json{std::string{reinterpret_cast<const char *>(admission->input.data()),
                                  std::min<std::size_t>(admission->input.size(), 1024)}}},
        {"input_truncated", Json{admission->input.size() > 1024}},
        {"outcome_unknown", Json{unknown}}, {"replay_authority", Json{false}}}));
  }
  Json packet = Json::object({{"mission", mission}, {"source", lineage},
      {"cause", Json{std::string{error_name(cause.code)}}}, {"detail", Json{JsonNumber{std::to_string(cause.detail)}}},
      {"local_quiescence", Json{"cooperative-native; no live callbacks or children; no uncontained exec"}},
      {"attempts", Json{attempts}}, {"attempt_count", number(total)},
      {"older_attempts", Json{"inspect exact original audit; not retroactively settled"}},
      {"dirty_paths", Json{"unavailable unless supplied in root mission packet; do not reset checkout"}}});
  std::uint64_t issuer = 0;
  if (::getentropy(&issuer, sizeof(issuer)) != 0 || issuer == 0)
    throw Error{ErrorCode::io};
  auto root = unwrap(RetainedState::create(
      std::make_unique<NativeJournalDirectory>(unwrap(NativeJournalDirectory::open(destination.string()))),
      "audit", JournalHeader{random_identity<EnvironmentId>(), random_identity<AuditStreamId>(),
                              issuer, {32*1024*1024, 96*1024*1024}, std::nullopt},
      {64ULL*1024*1024, 20000}));
  AuditLog log{*root};
  ContextStore context{log};
  context.seed_successor(Json::object({{"version", number(1)}, {"source", lineage}, {"entries", Json{selected}}}));
  context.append({Json::object({{"role", Json{"developer"}}, {"content", Json{std::string{instruction}}}})}, "backstop.instructions");
  log.original({"backstop.packet", unwrap(dump_json(packet)), Json{}});
  // request() installs the engine cancellation callback on the shared adapter.
  // Restore it before this recovery lifetime ends; do not leave borrowed captures.
  struct RestoreCancellation {
    CodingProvider &provider;
    std::function<bool()> previous;
    ~RestoreCancellation() { provider.cancelled = std::move(previous); }
  } restore_cancellation{assessor, assessor.cancelled};
  CodingEngine engine{log, context, assessor, settings.model};
  engine.effort(settings.effort);
  // Bound even a selected Lua action that loops without making progress. The
  // ordinary VM interrupt hook and transport cancellation consume this deadline.
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::minutes(2);
  engine.cancelled = [&] {
    return (paused && paused()) || std::chrono::steady_clock::now() >= deadline;
  };
  engine.display = display;
  const auto protected_tree = std::filesystem::canonical(source_path);
  auto protect_path = [&](const std::filesystem::path &requested) {
    const auto resolved = std::filesystem::weakly_canonical(requested);
    const auto relative = resolved.lexically_relative(protected_tree);
    if (relative.empty() || *relative.begin() != "..")
      throw Error{ErrorCode::conflict};
    // Resolve hardlink aliases as well as symlinks. This is an owned local
    // workflow, not an OS sandbox against concurrent adversarial filesystem edits.
    if (std::filesystem::exists(resolved)) {
      std::size_t inspected = 0;
      for (const auto &entry : std::filesystem::recursive_directory_iterator(protected_tree)) {
        if (++inspected > 4096)
          throw Error{ErrorCode::capacity};
        if (entry.is_regular_file() && std::filesystem::equivalent(resolved, entry.path()))
          throw Error{ErrorCode::conflict};
      }
    }
  };
  bool assessing = false;
  unsigned assessment_calls = 0;
  engine.effect_policy = [&](std::string_view name, const Json &input) {
    if (name == "provider" && (!assessing || ++assessment_calls > 1))
      throw Error{ErrorCode::conflict};
    if (name == "exec" || name == "restart")
      throw Error{ErrorCode::conflict};
    if (name == "write_file" || name == "edit_file")
      protect_path(string_field(input, "path"));
  };
  std::set<std::string> attempted_actions;
  Json outcome;
  for (unsigned pivot = 1; pivot <= 2; ++pivot) {
    check_pause(paused);
    if (!detail::locally_quiescent())
      throw Error{ErrorCode::external_unknown};
    try {
      assessing = true;
      assessment_calls = 0;
      engine.turn({unwrap(dump_json(packet)), assessment});
      assessing = false;
      const auto choice = engine.workflow_result();
      for (const auto key : {"retained", "uncertain", "change", "next", "action"})
        if (string_field(choice, key).empty() || string_field(choice, key).size() > 32768)
          throw Error{ErrorCode::invalid_range};
      const auto &oracle = field(choice, "oracle");
      const auto path = std::filesystem::path{string_field(oracle, "path")};
      const auto &expected = string_field(oracle, "content");
      if (!path.is_absolute() || expected.empty() || expected.size() > 32768)
        throw Error{ErrorCode::invalid_range};
      protect_path(path);
      const auto &action = string_field(choice, "action");
      if (attempted_actions.contains(action))
        throw Error{ErrorCode::conflict};
      std::string prior;
      const bool existed = std::filesystem::exists(path);
      if (existed) prior = read_file(path, 1024*1024);
      if (existed && prior == expected)
        throw Error{ErrorCode::conflict};
      check_pause(paused);
      log.original({"backstop.pivot", unwrap(dump_json(choice)), Json::object({{"ordinal", number(pivot)}})});
      attempted_actions.insert(action);
      packet.object().emplace_back("selected_pivot_" + std::to_string(pivot), choice);
      engine.turn({"Execute the explicitly selected pivot; preserve source unknowns.", action});
      check_pause(paused);
      const auto actual = read_file(path, 1024*1024);
      if (actual != expected)
        throw Error{ErrorCode::conflict};
      outcome = Json::object({{"phase", Json{"useful-work-observed"}}, {"pivot", number(pivot)},
          {"artifact", Json{path.string()}}, {"content", Json{actual}},
          {"session", Json{destination.string()}}, {"remote_effects_settled", Json{false}}});
      log.original({"backstop.outcome", unwrap(dump_json(outcome)), Json{}});
      return outcome;
    } catch (const Error &error) {
      const bool expired = std::chrono::steady_clock::now() >= deadline;
      outcome = Json::object({{"phase", Json{expired ? "deadline-blocked" :
          error.code == ErrorCode::interrupted ? "paused" : "no-progress"}}, 
          {"pivot", number(pivot)}, {"error", Json{std::string{error_name(error.code)}}},
          {"session", Json{destination.string()}}, {"effect_outcome", Json{"unknown; inspect recovery audit; no replay"}}});
      log.original({"backstop.outcome", unwrap(dump_json(outcome)), Json{}});
      if (error.code == ErrorCode::interrupted || !detail::locally_quiescent() ||
          root->state() != JournalWriterState::live)
        return outcome;
      packet.object().emplace_back("previous_pivot_outcome_" + std::to_string(pivot), outcome);
    }
  }
  outcome.object().emplace_back("bound", Json{"two no-progress pivots exhausted; explicit inspection required"});
  log.original({"backstop.blocked", unwrap(dump_json(outcome)), Json{}});
  return outcome;
}
} // namespace blackbird
