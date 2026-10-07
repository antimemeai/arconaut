#pragma once
#include "blackbird/context.hpp"
#include <set>
namespace blackbird {
// Single native session owner. File adapters are trusted local operator inputs.
class StationStore {
public:
  explicit StationStore(AuditLog &log);
  bool control(const Json &command);
  bool admit(const Json &event);
  void pause(std::string_view reason);
  void finish(const Json &event, bool returned);
  std::string prompt(const Json &event) const;
  Json view() const;
  bool paused() const noexcept { return paused_; }
  bool stopped() const noexcept { return stopped_; }

private:
  AuditLog &log_;
  bool paused_ = false, stopped_ = false;
  std::string steer_;
  std::set<std::string> controls_;
  std::map<std::pair<std::string, std::string>, std::string> events_;
  void apply_control(const Json &command);
};
} // namespace blackbird
