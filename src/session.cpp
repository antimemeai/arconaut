#include "arconaut/session.hpp"
#include "arconaut/tools.hpp"
#include <algorithm>
#include <sys/stat.h>
namespace arconaut {
namespace {
Json packet(const ApplicationRecordEvent &record) {
  return unwrap(parse_json(std::string_view{
      reinterpret_cast<const char *>(record.payload.data()), record.payload.size()}));
}
bool labelled(const Json &value, std::string_view label) {
  const auto *name = value.find("label");
  return name && std::holds_alternative<std::string>(name->value()) &&
         name->string() == label;
}
template <class T> T identity(std::string_view text) {
  if (text.size() != 32)
    throw Error{ErrorCode::corrupt};
  IdentityBytes bytes{};
  auto digit = [](char c) -> unsigned {
    if (c >= '0' && c <= '9')
      return static_cast<unsigned>(c - '0');
    if (c >= 'a' && c <= 'f')
      return static_cast<unsigned>(c - 'a' + 10);
    throw Error{ErrorCode::corrupt};
  };
  for (std::size_t i = 0; i < bytes.size(); ++i)
    bytes[i] = static_cast<std::byte>(16 * digit(text[2 * i]) + digit(text[2 * i + 1]));
  return unwrap(T::from_bytes(bytes));
}
void validate(const SessionSettings &value) {
  if (value.model.size() > 1024 || value.workflow.size() > 65536 ||
      value.model.empty() || value.model.find('\0') != std::string::npos ||
      value.workflow.empty() || value.workflow.find('\0') != std::string::npos ||
      (value.effort != "low" && value.effort != "medium" && value.effort != "high" &&
       value.effort != "xhigh"))
    throw Error{ErrorCode::invalid_range};
}
} // namespace
void save_session_info(const std::filesystem::path &directory,
                       const SessionSettings &settings, const SessionIdentity &id) {
  validate(settings);
  write_file(directory / "session-info.json",
             unwrap(dump_json(Json::object(
                 {{"version", Json{JsonNumber{"1"}}},
                  {"model", Json{settings.model}},
                  {"effort", Json{settings.effort}},
                  {"workflow", Json{settings.workflow}},
                  {"actor", Json{hex_identity(id.actor.bytes())}},
                  {"conversation", Json{hex_identity(id.conversation.bytes())}},
                  {"workflow_id", Json{hex_identity(id.workflow.bytes())}}}))));
}
Json list_sessions(const std::filesystem::path &requested) {
  const auto absolute = std::filesystem::absolute(requested).lexically_normal();
  std::error_code root_error;
  const auto canonical_root = std::filesystem::weakly_canonical(absolute, root_error);
  const auto root = root_error ? absolute : canonical_root;
  std::vector<std::filesystem::path> paths;
  std::error_code error;
  auto candidate = [&](const std::filesystem::path &path) {
    std::error_code e;
    if (std::filesystem::is_symlink(path, e) || e)
      return;
    if (std::filesystem::exists(path / "audit", e) ||
        std::filesystem::exists(path / "session-info.json", e))
      paths.push_back(path);
  };
  if (std::filesystem::is_directory(root, error)) {
    candidate(root);
    for (std::filesystem::directory_iterator i{root, error}, end; !error && i != end;
         i.increment(error)) {
      std::error_code entry_error;
      if (i->is_directory(entry_error))
        candidate(i->path());
    }
  }
  std::sort(paths.begin(), paths.end());
  Json::Array entries;
  for (const auto &path : paths) {
    Json config;
    std::string status = "missing";
    try {
      if (std::filesystem::exists(path / "session-info.json")) {
        struct stat info{};
        if (::lstat((path / "session-info.json").c_str(), &info) != 0 ||
            !S_ISREG(info.st_mode))
          throw Error{ErrorCode::corrupt};
        auto packet =
            unwrap(parse_json(read_file(path / "session-info.json", 1024 * 1024)));
        if (field(packet, "version").number().text != "1")
          throw Error{ErrorCode::corrupt};
        SessionSettings settings{string_field(packet, "model"),
                                 string_field(packet, "effort"),
                                 string_field(packet, "workflow")};
        validate(settings);
        (void)identity<ParticipantId>(string_field(packet, "actor"));
        (void)identity<ConversationId>(string_field(packet, "conversation"));
        (void)identity<WorkflowId>(string_field(packet, "workflow_id"));
        config = Json::object({{"model", Json{settings.model}},
                               {"effort", Json{settings.effort}},
                               {"workflow", Json{settings.workflow}}});
        status = "snapshot";
      }
    } catch (...) {
      status = "damaged";
    }
    struct stat metadata{};
    Json activity;
    const bool activity_available = ::stat((path / "audit").c_str(), &metadata) == 0;
    if (activity_available)
      activity = Json{JsonNumber{std::to_string(metadata.st_mtime)}};
    std::error_code canonical_error;
    const auto canonical = std::filesystem::weakly_canonical(path, canonical_error);
    const auto stable = canonical_error ? path.string() : canonical.string();
    std::string quoted = "'";
    for (const char c : stable)
      quoted += c == '\'' ? "'\"'\"'" : std::string(1, c);
    quoted += "'";
    entries.push_back(
        Json::object({{"path", Json{stable}},
                      {"configuration", std::move(config)},
                      {"metadata_status", Json{status}},
                      {"last_activity_unix_seconds", std::move(activity)},
                      {"last_activity_status",
                       Json{activity_available ? "available" : "unavailable"}},
                      {"resume_command", Json{"./scripts/arco --session " + quoted}}}));
  }
  return Json::object(
      {{"root", Json{root.string()}},
       {"sessions", Json{std::move(entries)}},
       {"configuration_source", Json{"derived snapshot; audit authoritative; missing "
                                     "legacy snapshots require ordinary reopen"}},
       {"scan_status", Json{error ? "incomplete/unreadable root" : "complete"}}});
}
SessionIdentity session_identity(AuditLog &log) {
  std::optional<SessionIdentity> result;
  for (const auto &fact : log.root().committed_facts()) {
    if (const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
        record && record->channel == ApplicationChannel::program) {
      const auto value = packet(*record);
      if (labelled(value, "session.identity"))
        return {identity<ParticipantId>(string_field(value, "actor")),
                identity<ConversationId>(string_field(value, "conversation")),
                identity<WorkflowId>(string_field(value, "workflow"))};
    }
    if (const auto *decision = std::get_if<DecisionEvent>(&fact.event.body))
      result =
          SessionIdentity{decision->actor, decision->conversation, decision->workflow};
  }
  if (!result)
    result = SessionIdentity{unwrap(log.root().issue<ParticipantId>()),
                             unwrap(log.root().issue<ConversationId>()),
                             unwrap(log.root().issue<WorkflowId>())};
  log.record(
      ApplicationChannel::program,
      Json::object({{"label", Json{"session.identity"}},
                    {"actor", Json{hex_identity(result->actor.bytes())}},
                    {"conversation", Json{hex_identity(result->conversation.bytes())}},
                    {"workflow", Json{hex_identity(result->workflow.bytes())}}}));
  return *result;
}
SessionStore::SessionStore(AuditLog &log) : log_(log) {
  for (const auto &fact : log.root().committed_facts()) {
    const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
    if (!record || record->channel != ApplicationChannel::program)
      continue;
    const auto value = packet(*record);
    if (labelled(value, "session.settings")) {
      settings_ = {string_field(value, "model"), string_field(value, "effort"),
                   string_field(value, "workflow")};
      validate(settings_);
    } else if (labelled(value, "session.restart"))
      restart_ = value;
  }
}
void SessionStore::save(SessionSettings value) {
  validate(value);
  if (value == settings_)
    return;
  log_.record(ApplicationChannel::program,
              Json::object({{"label", Json{"session.settings"}},
                            {"model", Json{value.model}},
                            {"effort", Json{value.effort}},
                            {"workflow", Json{value.workflow}}}));
  settings_ = std::move(value);
}
void SessionStore::restart(std::string_view note) {
  if (note.empty() || note.size() > 65536)
    throw Error{ErrorCode::invalid_range};
  if (!std::holds_alternative<std::nullptr_t>(restart_.value()) &&
      string_field(restart_, "state") == "pending")
    throw Error{ErrorCode::busy};
  for (const auto &fact : log_.root().committed_facts())
    if (const auto *admission = std::get_if<AttemptAdmissionEvent>(&fact.event.body)) {
      const auto attempt = unwrap(log_.root().attempt(admission->attempt));
      if (!attempt.observation || attempt.observation->phase != AttemptPhase::terminal)
        throw Error{ErrorCode::busy};
    }
  const auto token = hex_identity(log_.issue().bytes());
  auto value = Json::object({{"label", Json{"session.restart"}},
                             {"token", Json{token}},
                             {"state", Json{"pending"}},
                             {"note", Json{std::string{note}}}});
  log_.record(ApplicationChannel::program, value);
  restart_ = std::move(value);
}
bool SessionStore::resume(ContextStore &context) {
  if (std::holds_alternative<std::nullptr_t>(restart_.value()) ||
      string_field(restart_, "state") != "pending")
    return false;
  const auto origin = "rrc:" + string_field(restart_, "token");
  bool injected = false;
  for (const auto &fact : log_.root().committed_facts()) {
    const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
    if (!record || record->channel != ApplicationChannel::context)
      continue;
    const auto value = packet(*record);
    const auto *previous = value.find("origin");
    if (previous && std::holds_alternative<std::string>(previous->value()) &&
        previous->string() == origin)
      injected = true;
  }
  if (!injected)
    context.append(
        {Json::object({{"role", Json{"user"}},
                       {"content", Json{"continue\n\nRestart/resume note:\n" +
                                        string_field(restart_, "note")}}})},
        origin);
  auto consumed = Json::object({{"label", Json{"session.restart"}},
                                {"state", Json{"consumed"}},
                                {"token", field(restart_, "token")},
                                {"note", field(restart_, "note")}});
  log_.record(ApplicationChannel::program, consumed);
  restart_ = std::move(consumed);
  return true;
}
} // namespace arconaut
