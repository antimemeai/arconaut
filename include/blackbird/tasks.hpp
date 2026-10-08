#pragma once
#include "blackbird/context.hpp"
#include <array>
#include <deque>
#include <set>

namespace blackbird {
// Current task projection only; history belongs to the retained journal.
class TaskState {
public:
  static constexpr std::size_t max_items = 2048;
  static constexpr std::size_t max_batch = 64;
  static constexpr std::size_t max_page = 64;
  static constexpr std::size_t retry_window = 16;
  struct Prepared {
    std::map<std::string, Json> changed;
    std::vector<std::string> removed;
    std::optional<std::vector<std::string>> order;
    std::deque<Json> receipts;
    std::array<std::size_t, 5> counts{};
    std::map<std::string, std::array<std::size_t, 2>> rollups;
    std::uint64_t revision = 0, next = 1;
    std::string title, bead;
    Json packet, result;
  };
  Prepared prepare(const Json &request) const;
  void commit(Prepared prepared);
  void replay(const Json &packet);
  void restore(const Json &snapshot);
  Json snapshot() const;
  Json read(const Json &query = Json::object({})) const;
  const Json *item(std::string_view id) const;
  std::uint64_t revision() const noexcept { return revision_; }
  bool empty() const noexcept { return items_.empty(); }
  std::size_t size() const noexcept { return items_.size(); }
  std::optional<std::size_t> position(std::string_view id) const;

private:
  std::map<std::string, Json> items_;
  std::vector<std::string> order_;
  std::deque<Json> receipts_;
  std::array<std::size_t, 5> counts_{};
  std::map<std::string, std::array<std::size_t, 2>> rollups_;
  std::uint64_t revision_ = 0, next_ = 1;
  std::string title_ = "Work", bead_;
};

class TaskStore {
public:
  explicit TaskStore(AuditLog &log);
  Json read(const Json &query = Json::object({})) const { return state_.read(query); }
  Json edit(const Json &request);
  Json operator_command(std::string_view command);
  Json snapshot() const { return state_.snapshot(); }
  const TaskState &state() const noexcept { return state_; }
  std::function<void(const Json &)> changed;

private:
  AuditLog &log_;
  TaskState state_;
};
} // namespace blackbird
