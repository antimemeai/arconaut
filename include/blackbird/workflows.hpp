#pragma once
#include "blackbird/chat_view.hpp"
#include "blackbird/json.hpp"
#include <map>
#include <memory>
#include <optional>
namespace blackbird {
struct WorkflowSelection {
  Json definition;
  std::string arguments, trigger;
};
class WorkflowRegistry {
public:
  WorkflowRegistry(const Json &config, std::string revision);
  const Json &definitions() const { return definitions_; }
  const std::string &revision() const { return revision_; }
  const std::string &prefix() const { return prefix_; }
  Json discover() const;
  std::optional<WorkflowSelection> select(std::string_view text) const;
  const Json &named(std::string_view name) const;
  ChatRow color(std::string_view text, Ink base = Ink::normal) const;

private:
  struct Powerword {
    std::size_t definition;
    Ink ink;
  };
  std::map<std::string, std::size_t, std::less<>> aliases_;
  std::map<std::string, Powerword, std::less<>> powerwords_;
  Json definitions_ = Json{Json::Array{}};
  std::string revision_, prefix_;
};
Json default_workflows();
std::vector<std::string> terminal_builtin_names();
void publish_workflows(std::shared_ptr<const WorkflowRegistry> registry);
std::shared_ptr<const WorkflowRegistry> displayed_workflows();
} // namespace blackbird
