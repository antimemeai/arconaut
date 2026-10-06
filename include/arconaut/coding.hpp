#pragma once
#include "arconaut/openai.hpp"
#include "arconaut/session.hpp"
#include "arconaut/tools.hpp"
#include <map>
#include <memory>
namespace arconaut {
class CodingProvider {
public:
  virtual ~CodingProvider() = default;
  std::function<bool()> cancelled;
  virtual Json respond(const Json &request,
                       const std::function<void(std::string_view)> &capture) = 0;
};
class OpenAiCodingProvider final : public CodingProvider {
public:
  explicit OpenAiCodingProvider(OpenAiConfig config = {})
      : config_(std::move(config)) {}
  Json respond(const Json &, const std::function<void(std::string_view)> &) override;

private:
  OpenAiConfig config_;
};
// Settle abandoned provider requests as unknown; never redispatch old effects.
void recover_coding_session(RetainedState &root);
struct TurnInput {
  std::string_view prompt;
  std::string_view program;
};
class CodingEngine {
public:
  CodingEngine(AuditLog &log, ContextStore &context, CodingProvider &provider,
               std::string model);
  ~CodingEngine();
  CodingEngine(const CodingEngine &) = delete;
  CodingEngine &operator=(const CodingEngine &) = delete;
  void turn(TurnInput input);
  ContextStore &context() noexcept { return context_; }
  void model(std::string name) { model_ = std::move(name); }
  std::function<void(std::string_view)> display;
  std::function<void(std::string_view)> status;
  std::function<void(std::string_view)> operation_completed;
  std::function<void(std::string_view)> process_output;
  std::function<bool()> cancelled;
  void effort(std::string value);
  Json stats() const;
  void validate_restart() const;
  std::optional<std::string> take_restart_note() {
    return std::exchange(restart_note_, std::nullopt);
  }
  const std::optional<std::string> &restart_note() const noexcept {
    return restart_note_;
  }

private:
  struct Runtime;
  AuditLog &log_;
  ContextStore &context_;
  CodingProvider &provider_;
  std::string model_;
  std::string effort_ = "medium";
  Json last_request_bytes_, usage_;
  std::map<std::string, std::string> previewed_;
  void present(const Json &item);

  SessionIdentity identity_;
  std::optional<std::string> restart_note_;
  DefinitionGenerationId generation_;
  std::unique_ptr<Runtime> runtime_;
  Json request(Json options = Json::object({}));
  Json call(std::string name, Json arguments);
  Json operation(std::string_view name, const Json &input,
                 const std::function<Json(OperationAttemptId)> &body);
  friend struct Runtime;
};
} // namespace arconaut
