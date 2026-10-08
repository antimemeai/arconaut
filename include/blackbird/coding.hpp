#pragma once
#include "blackbird/beads.hpp"
#include "blackbird/decision_models.hpp"
#include "blackbird/openai.hpp"
#include "blackbird/session.hpp"
#include "blackbird/tools.hpp"

#include "blackbird/tasks.hpp"
#include "blackbird/workflows.hpp"
#include <map>
#include <memory>
namespace blackbird {
class CodingProvider {
public:
  virtual ~CodingProvider() = default;
  std::function<bool()> cancelled;
  virtual std::string_view provider_identity() const noexcept { return "unavailable"; }
  virtual Json respond(const Json &request,
                       const std::function<void(std::string_view)> &capture) = 0;
};
class OpenAiCodingProvider final : public CodingProvider {
public:
  explicit OpenAiCodingProvider(OpenAiConfig config = {})
      : config_(std::move(config)) {}
  std::string_view provider_identity() const noexcept override {
    return "openai-codex";
  }
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
               std::string model, DecisionModelConfig decisions = {});
  ~CodingEngine();
  CodingEngine(const CodingEngine &) = delete;
  CodingEngine &operator=(const CodingEngine &) = delete;
  void turn(TurnInput input);
  bool operator_turn(std::string_view prompt, std::string_view fallback);
  std::shared_ptr<const WorkflowRegistry> workflows() const { return workflows_; }
  // One cooperative failure ticket, consumed before creating a recovery audit.
  // A reopened/crashed engine has no such authority.
  Error claim_backstop();
  const Json &workflow_result() const noexcept { return workflow_result_; }
  ContextStore &context() noexcept { return context_; }
  TaskStore &tasks() noexcept { return tasks_; }
  void model(std::string name) { model_ = std::move(name); }
  std::function<void(std::string_view)> display;
  std::function<void(std::string_view)> diagnostic;
  std::function<void(std::string_view)> notice;
  std::function<void(const Json &)> observed_usage;
  std::function<void(std::string_view)> operation_started;
  std::function<void(const Json &)> task_activity;
  std::function<void(std::string_view)> status;
  std::function<void(std::string_view)> operation_completed;
  std::function<void(std::string_view, AttemptDisposition)> operation_outcome;
  std::function<void(std::string_view)> process_output;
  std::function<bool()> cancelled;
  // Optional native effect restriction for a scoped workflow (e.g. recovery
  // evidence protection). Throwing refuses admission; it is not Lua authority.
  std::function<void(std::string_view, const Json &)> effect_policy;
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
  bool turn_running_ = false, backstop_claimed_ = false;
  std::optional<Error> failed_turn_;
  Json workflow_result_;
  bool capacity_stopped_ = false;
  std::size_t operation_depth_ = 0;
  std::size_t unretained_bytes_ = 0;
  void protect_workflow(const Json::Array &entries, const Json &proposal);
  AuditLog &log_;
  ContextStore &context_;
  TaskStore tasks_;
  CodingProvider &provider_;
  std::string model_;
  std::string effort_ = "medium";
  Json last_request_bytes_, usage_;
  // Calls already unanswered at workflow admission. Repair never closes calls
  // introduced by the currently running workflow.
  Json::Array repairable_outputs_;
  Json lua_tools_{Json::Array{}}, pending_lua_tools_;
  std::string tools_revision_ = "initial-empty", pending_tools_revision_;
  Json program_config_{Json::object(
      {{"modules", Json{Json::Array{}}}, {"model", Json{""}}, {"effort", Json{""}}})};
  std::shared_ptr<const WorkflowRegistry> workflows_, pending_workflows_;
  unsigned workflow_depth_ = 0, workflow_invocations_ = 0;
  struct WorkflowContinuation {
    Json definition, arguments;
    std::string revision, selection_attempt;
  };
  std::vector<WorkflowContinuation> workflow_continuations_;
  bool draining_workflows_ = false;
  std::optional<Error> workflow_failure_;
  Json execute_workflow(const WorkflowContinuation &invocation);
  void drain_workflows();
  Json pending_program_config_;
  std::string program_revision_ = "initial-empty", pending_program_revision_;
  Json program_config(const Json &arguments);
  Json module_source(const Json &arguments);
  Json tool_registry() const;
  Json define_tool(const Json &arguments);
  Json request_tools() const;
  Json budget_, pending_budget_;

  std::string budget_revision_ = "initial-disabled", pending_budget_revision_;
  Json context_budget(const Json &arguments);
  Json budget_view(std::string_view model, std::size_t input_bytes) const;
  std::map<std::string, std::string> previewed_;
  void present(const Json &item);

  BeadsAdapter beads_;
  DecisionModels decision_models_;
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
} // namespace blackbird
