#include "blackbird/session.hpp"
#include "blackbird/packet.hpp"
#include "blackbird/tools.hpp"
#include <algorithm>
#include <sys/stat.h>
namespace blackbird {
std::filesystem::path default_session_directory(const std::filesystem::path &home) {
  const auto current = home / ".local/state/blackbird/default";
  const auto legacy = home / ".local/state/arconaut/default";
  return !std::filesystem::exists(current) && std::filesystem::is_directory(legacy)
             ? legacy
             : current;
}
namespace {
Value packet(const ApplicationRecordEvent &record) {
  return unwrap(read_packet(record.payload));
}
bool labelled(const Value &value, std::string_view label) {
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
  if (value.name.size() > 128 ||
      value.name.find_first_of("\r\n") != std::string::npos ||
      value.name.find('\0') != std::string::npos || value.model.size() > 1024 ||
      value.workflow.size() > 65536 || value.model.empty() ||
      value.model.find('\0') != std::string::npos || value.workflow.empty() ||
      value.workflow.find('\0') != std::string::npos ||
      (value.effort != "low" && value.effort != "medium" && value.effort != "high" &&
       value.effort != "xhigh"))
    throw Error{ErrorCode::invalid_range};
}
} // namespace
void save_session_info(const std::filesystem::path &directory,
                       const SessionSettings &settings, const SessionIdentity &id) {
  validate(settings);
  const auto serialized = unwrap(encode_packet_string(
      Value::object({{"version", Value{Number{"1"}}},
                     {"name", Value{settings.name}},
                     {"model", Value{settings.model}},
                     {"effort", Value{settings.effort}},
                     {"workflow", Value{settings.workflow}},
                     {"actor", Value{hex_identity(id.actor.bytes())}},
                     {"conversation", Value{hex_identity(id.conversation.bytes())}},
                     {"workflow_id", Value{hex_identity(id.workflow.bytes())}}})));
  const auto path = directory / "session-info.bbm";
  try {
    if (read_file(path, 512 * 1024) == serialized)
      return;
  } catch (const Error &) {
    // Missing/unreadable derived metadata is replaced from current audit state.
  }
  write_file(path, serialized);
}
Value list_sessions(const std::filesystem::path &requested) {
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
        std::filesystem::exists(path / "session-info.bbm", e))
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
  Value::Array entries;
  for (const auto &path : paths) {
    Value config;
    std::string status = "missing";
    try {
      if (std::filesystem::exists(path / "session-info.bbm")) {
        struct stat info{};
        if (::lstat((path / "session-info.bbm").c_str(), &info) != 0 ||
            !S_ISREG(info.st_mode))
          throw Error{ErrorCode::corrupt};
        auto packet = unwrap(
            decode_packet_string(read_file(path / "session-info.bbm", 1024 * 1024)));
        if (field(packet, "version").number().text() != "1")
          throw Error{ErrorCode::corrupt};
        SessionSettings settings{string_field(packet, "model"),
                                 string_field(packet, "effort"),
                                 string_field(packet, "workflow")};
        if (const auto *name = packet.find("name"))
          settings.name = name->string();
        validate(settings);
        (void)identity<ParticipantId>(string_field(packet, "actor"));
        (void)identity<ConversationId>(string_field(packet, "conversation"));
        (void)identity<WorkflowId>(string_field(packet, "workflow_id"));
        config = Value::object({{"name", Value{settings.name}},
                                {"model", Value{settings.model}},
                                {"effort", Value{settings.effort}},
                                {"workflow", Value{settings.workflow}}});
        status = "snapshot";
      }
    } catch (...) {
      status = "damaged";
    }
    struct stat metadata{};
    Value activity;
    const bool activity_available = ::stat((path / "audit").c_str(), &metadata) == 0;
    if (activity_available)
      activity = Value{Number{metadata.st_mtime}};
    std::error_code canonical_error;
    const auto canonical = std::filesystem::weakly_canonical(path, canonical_error);
    const auto stable = canonical_error ? path.string() : canonical.string();
    std::string quoted = "'";
    for (const char c : stable)
      quoted += c == '\'' ? "'\"'\"'" : std::string(1, c);
    quoted += "'";
    entries.push_back(Value::object(
        {{"path", Value{stable}},
         {"configuration", std::move(config)},
         {"metadata_status", Value{status}},
         {"last_activity_unix_seconds", std::move(activity)},
         {"last_activity_status",
          Value{activity_available ? "available" : "unavailable"}},
         {"resume_command", Value{"./scripts/blackbird --session " + quoted}}}));
  }
  return Value::object(
      {{"root", Value{root.string()}},
       {"sessions", Value{std::move(entries)}},
       {"configuration_source", Value{"derived snapshot; audit authoritative; missing "
                                      "legacy snapshots require ordinary reopen"}},
       {"scan_status", Value{error ? "incomplete/unreadable root" : "complete"}}});
}
SessionIdentity session_identity(AuditLog &log) {
  std::optional<SessionIdentity> result;
  for (const auto &value : log.root().current_programs())
    if (labelled(value, "session.identity"))
      return {identity<ParticipantId>(string_field(value, "actor")),
              identity<ConversationId>(string_field(value, "conversation")),
              identity<WorkflowId>(string_field(value, "workflow"))};
  // Legacy sessions may not have an explicit identity packet.
  for (std::size_t ordinal = 0; ordinal < log.root().fact_count(); ++ordinal) {
    const auto fact = unwrap(log.root().fact(ordinal));
    if (const auto *decision = std::get_if<DecisionEvent>(&fact.event.body))
      result =
          SessionIdentity{decision->actor, decision->conversation, decision->workflow};
  }
  if (!result)
    result = SessionIdentity{unwrap(log.root().issue<ParticipantId>()),
                             unwrap(log.root().issue<ConversationId>()),
                             unwrap(log.root().issue<WorkflowId>())};
  log.record(ApplicationChannel::program,
             Value::object(
                 {{"label", Value{"session.identity"}},
                  {"actor", Value{hex_identity(result->actor.bytes())}},
                  {"conversation", Value{hex_identity(result->conversation.bytes())}},
                  {"workflow", Value{hex_identity(result->workflow.bytes())}}}));
  return *result;
}
SessionStore::SessionStore(AuditLog &log) : log_(log) {
  for (const auto &value : log.root().current_programs()) {
    if (labelled(value, "session.settings")) {
      settings_ = {string_field(value, "model"), string_field(value, "effort"),
                   string_field(value, "workflow")};
      if (const auto *name = value.find("name"))
        settings_.name = name->string();
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
              Value::object({{"label", Value{"session.settings"}},
                             {"name", Value{value.name}},
                             {"model", Value{value.model}},
                             {"effort", Value{value.effort}},
                             {"workflow", Value{value.workflow}}}));
  settings_ = std::move(value);
}
void SessionStore::restart(std::string_view note) {
  if (note.empty() || note.size() > 65536)
    throw Error{ErrorCode::invalid_range};
  if (!std::holds_alternative<std::nullptr_t>(restart_.value()) &&
      string_field(restart_, "state") == "pending")
    throw Error{ErrorCode::busy};
  if (!unwrap(log_.root().unresolved_attempts()).empty())
    throw Error{ErrorCode::busy};
  const auto token = hex_identity(log_.issue().bytes());
  auto value = Value::object({{"label", Value{"session.restart"}},
                              {"token", Value{token}},
                              {"state", Value{"pending"}},
                              {"note", Value{std::string{note}}}});
  log_.record(ApplicationChannel::program, value);
  restart_ = std::move(value);
}
bool SessionStore::resume(ContextStore &context) {
  if (std::holds_alternative<std::nullptr_t>(restart_.value()) ||
      string_field(restart_, "state") != "pending")
    return false;
  const auto origin = "rrc:" + string_field(restart_, "token");
  bool injected = false;
  for (const auto &fact : log_.root().tail_facts()) {
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
        {Value::object({{"role", Value{"user"}},
                        {"content", Value{"continue\n\nRestart/resume note:\n" +
                                          string_field(restart_, "note")}}})},
        origin);
  auto consumed = Value::object({{"label", Value{"session.restart"}},
                                 {"state", Value{"consumed"}},
                                 {"token", field(restart_, "token")},
                                 {"note", field(restart_, "note")}});
  log_.record(ApplicationChannel::program, consumed);
  restart_ = std::move(consumed);
  return true;
}
} // namespace blackbird
