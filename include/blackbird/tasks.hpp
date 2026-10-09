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
    std::map<std::string, Value> changed;
    std::vector<std::string> removed;
    std::optional<std::vector<std::string>> order;
    std::deque<Value> receipts;
    std::array<std::size_t, 5> counts{};
    std::map<std::string, std::array<std::size_t, 2>> rollups;
    std::uint64_t revision = 0, next = 1;
    std::string title, bead;
    Value packet, result;
  };
  Prepared prepare(const Value &request) const;
  void commit(Prepared prepared);
  void replay(const Value &packet);
  void restore(const Value &snapshot);
  Value snapshot() const;
  Value read(const Value &query = Value::object({})) const;
  const Value *item(std::string_view id) const;
  std::uint64_t revision() const noexcept { return revision_; }
  bool empty() const noexcept { return items_.empty(); }
  std::size_t size() const noexcept { return items_.size(); }
  std::optional<std::size_t> position(std::string_view id) const;

private:
  std::map<std::string, Value> items_;
  std::vector<std::string> order_;
  std::deque<Value> receipts_;
  std::array<std::size_t, 5> counts_{};
  std::map<std::string, std::array<std::size_t, 2>> rollups_;
  std::uint64_t revision_ = 0, next_ = 1;
  std::string title_ = "Work", bead_;
};

class TaskStore {
public:
  explicit TaskStore(AuditLog &log);
  Value read(const Value &query = Value::object({})) const {
    return state_.read(query);
  }
  Value edit(const Value &request);
  Value operator_command(std::string_view command);
  Value snapshot() const { return state_.snapshot(); }
  const TaskState &state() const noexcept { return state_; }
  std::function<void(const Value &)> changed;

private:
  AuditLog &log_;
  TaskState state_;
};
} // namespace blackbird
