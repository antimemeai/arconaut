#pragma once
#include "arconaut/context.hpp"
#include <filesystem>
namespace arconaut {
struct SessionSettings {
  std::string model = "gpt-6.1-sol", effort = "medium", workflow;
  bool operator==(const SessionSettings &) const = default;
};
struct SessionIdentity {
  ParticipantId actor;
  ConversationId conversation;
  WorkflowId workflow;
};
SessionIdentity session_identity(AuditLog &log);
void save_session_info(const std::filesystem::path &directory,
                       const SessionSettings &settings,
                       const SessionIdentity &identity);
Json list_sessions(const std::filesystem::path &root);
class SessionStore {
public:
  explicit SessionStore(AuditLog &log);
  const SessionSettings &settings() const noexcept { return settings_; }
  void save(SessionSettings value);
  void restart(std::string_view note);
  bool resume(ContextStore &context);

private:
  AuditLog &log_;
  SessionSettings settings_;
  Json restart_;
};
} // namespace arconaut
