#include "blackbird/tasks.hpp"
#include "blackbird/json.hpp"
#include "blackbird/packet.hpp"
#include <algorithm>
#include <charconv>
#include <chrono>
#include <limits>

namespace blackbird {
namespace {
constexpr std::array<std::string_view, 5> statuses{"queued", "active", "blocked",
                                                   "done", "dropped"};
Value number(std::uint64_t n) { return Value{Number{n}}; }
std::uint64_t integer(const Value &value) {
  const auto *n = std::get_if<Number>(&value.value());
  if (!n)
    throw Error{ErrorCode::invalid_range};
  std::uint64_t out = 0;
  const auto digits = n->text();
  const auto r = std::from_chars(digits.data(), digits.data() + digits.size(), out);
  if (r.ec != std::errc{} || r.ptr != digits.data() + digits.size())
    throw Error{ErrorCode::invalid_range};
  return out;
}
std::size_t status_index(const Value &row) {
  const auto &s = string_field(row, "status");
  const auto i = std::find(statuses.begin(), statuses.end(), s);
  if (i == statuses.end())
    throw Error{ErrorCode::invalid_range};
  return static_cast<std::size_t>(i - statuses.begin());
}
void put(Value &value, std::string_view key, Value fresh) {
  for (auto &[name, entry] : value.object())
    if (name == key) {
      entry = std::move(fresh);
      return;
    }
  value.object().emplace_back(key, std::move(fresh));
}
void bounded_text(const Value &value, std::size_t bound, bool allow_empty = true) {
  const auto *text = std::get_if<std::string>(&value.value());
  if (!text || text->size() > bound || (!allow_empty && text->empty()) ||
      text->find('\0') != std::string::npos)
    throw Error{ErrorCode::invalid_range};
}
void keys(const Value &object, std::initializer_list<std::string_view> allowed) {
  for (const auto &[key, value] : object.object()) {
    (void)value;
    if (std::find(allowed.begin(), allowed.end(), key) == allowed.end())
      throw Error{ErrorCode::invalid_range};
  }
}
bool same_arguments(const Value &left, const Value &right) {
  if (const auto *object = std::get_if<Value::Object>(&left.value())) {
    const auto *other = std::get_if<Value::Object>(&right.value());
    if (!other || object->size() != other->size())
      return false;
    for (const auto &[key, value] : *object) {
      const auto *found = right.find(key);
      if (!found || !same_arguments(value, *found))
        return false;
    }
    return true;
  }
  if (const auto *array = std::get_if<Value::Array>(&left.value())) {
    const auto *other = std::get_if<Value::Array>(&right.value());
    if (!other || array->size() != other->size())
      return false;
    for (std::size_t i = 0; i < array->size(); ++i)
      if (!same_arguments((*array)[i], (*other)[i]))
        return false;
    return true;
  }
  return left == right;
}
void validate_row(const Value &row) {
  bounded_text(field(row, "id"), 32, false);
  bounded_text(field(row, "title"), 512, false);
  bounded_text(field(row, "parent"), 32);
  bounded_text(field(row, "owner"), 128);
  bounded_text(field(row, "note"), 2048);
  bounded_text(field(row, "blocker"), 512);
  (void)status_index(row);
  if (!integer(field(row, "version")))
    throw Error{ErrorCode::invalid_range};
  (void)integer(field(row, "updated"));
}
Value counts_json(const std::array<std::size_t, 5> &counts) {
  Value::Object out;
  for (std::size_t i = 0; i < counts.size(); ++i)
    out.emplace_back(statuses[i], number(counts[i]));
  return Value{std::move(out)};
}
std::uint64_t now_seconds() {
  return static_cast<std::uint64_t>(
      std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}
} // namespace

const Value *TaskState::item(std::string_view id) const {
  const auto found = items_.find(std::string{id});
  return found == items_.end() ? nullptr : &found->second;
}
std::optional<std::size_t> TaskState::position(std::string_view id) const {
  const auto found = std::find(order_.begin(), order_.end(), id);
  if (found == order_.end())
    return std::nullopt;
  return static_cast<std::size_t>(found - order_.begin());
}
TaskState::Prepared TaskState::prepare(const Value &request) const {
  keys(request, {"op_id", "base", "ops"});
  bounded_text(field(request, "op_id"), 64, false);
  if (unwrap(encode_packet_string(request)).size() > 65536)
    throw Error{ErrorCode::invalid_range};
  for (const auto &receipt : receipts_)
    if (field(receipt, "op_id") == field(request, "op_id")) {
      if (!same_arguments(field(receipt, "request"), request))
        throw Error{ErrorCode::conflict};
      Prepared retry;
      retry.result = field(receipt, "result");
      return retry;
    }
  const auto &ops = field(request, "ops").array();
  if (ops.empty() || ops.size() > max_batch || revision_ == UINT64_MAX)
    throw Error{ErrorCode::invalid_range};
  Prepared p;
  p.revision = revision_ + 1;
  p.next = next_;
  p.counts = counts_;
  p.title = title_;
  p.bead = bead_;
  p.receipts = receipts_;
  const auto structural = [&]() -> std::vector<std::string> & {
    if (!request.find("base") || integer(field(request, "base")) != revision_)
      throw Error{ErrorCode::conflict};
    if (!p.order)
      p.order = order_;
    return *p.order;
  };
  const auto lookup = [&](std::string_view id) -> const Value * {
    if (std::find(p.removed.begin(), p.removed.end(), id) != p.removed.end())
      return nullptr;
    const auto found = p.changed.find(std::string{id});
    return found == p.changed.end() ? item(id) : &found->second;
  };
  const auto resolve = [&](const Value &op, std::string_view key) {
    const auto *value = op.find(key);
    return value ? value->string() : std::string{};
  };
  const auto place = [&](const std::string &id, const std::string &parent,
                         const std::string &after) {
    if (!p.order)
      throw Error{ErrorCode::corrupt};
    auto &order = *p.order;
    std::erase(order, id);
    auto position = order.end();
    if (!after.empty()) {
      const auto *anchor = lookup(after);
      if (!anchor || after == id || string_field(*anchor, "parent") != parent)
        throw Error{ErrorCode::invalid_range};
      position = std::find(order.begin(), order.end(), after) + 1;
      if (parent.empty())
        while (position != order.end() &&
               string_field(*lookup(*position), "parent") == after)
          ++position;
    } else if (!parent.empty()) {
      position = std::find(order.begin(), order.end(), parent);
      if (position == order.end())
        throw Error{ErrorCode::invalid_range};
      ++position;
      while (position != order.end() &&
             string_field(*lookup(*position), "parent") == parent)
        ++position;
    }
    order.insert(position, id);
  };
  const auto timestamp = now_seconds();
  Value::Array added;
  for (const auto &op : ops) {
    const auto &kind = string_field(op, "op");
    if (kind == "list") {
      keys(op, {"op", "title", "bead"});
      structural();
      if (const auto *title = op.find("title")) {
        bounded_text(*title, 512, false);
        p.title = title->string();
      }
      if (const auto *bead = op.find("bead")) {
        bounded_text(*bead, 128);
        p.bead = bead->string();
      }
      continue;
    }
    if (kind == "add") {
      keys(op,
           {"op", "title", "parent", "after", "status", "owner", "note", "blocker"});
      structural();
      const auto old_removed = static_cast<std::size_t>(
          std::count_if(p.removed.begin(), p.removed.end(),
                        [&](const auto &id) { return items_.contains(id); }));
      const auto new_items = static_cast<std::size_t>(
          std::count_if(p.changed.begin(), p.changed.end(), [&](const auto &entry) {
            return !items_.contains(entry.first);
          }));
      if (items_.size() - old_removed + new_items >= max_items || p.next == UINT64_MAX)
        throw Error{ErrorCode::capacity};
      const auto id = "t" + std::to_string(p.next++);
      const auto parent = resolve(op, "parent");
      if (!parent.empty()) {
        const auto *owner = lookup(parent);
        if (!owner || !string_field(*owner, "parent").empty())
          throw Error{ErrorCode::invalid_range};
      }
      auto row = Value::object({{"id", Value{id}},
                                {"parent", Value{parent}},
                                {"title", field(op, "title")},
                                {"status", Value{"queued"}},
                                {"owner", Value{""}},
                                {"note", Value{""}},
                                {"blocker", Value{""}},
                                {"version", number(1)},
                                {"updated", number(timestamp)}});
      for (const auto key : {"status", "owner", "note", "blocker"})
        if (const auto *value = op.find(key))
          put(row, key, *value);
      validate_row(row);
      p.changed.emplace(id, row);
      place(id, parent, resolve(op, "after"));
      added.push_back(Value{id});
      continue;
    }
    const auto &id = string_field(op, "id");
    const auto *old = lookup(id);
    if (!old)
      throw Error{ErrorCode::invalid_range};
    // A batch may touch an existing row only once; each expected version binds
    // to what the actor actually read, rather than to intermediate batch state.
    if (p.changed.contains(id) ||
        integer(field(op, "version")) != integer(field(*old, "version")))
      throw Error{ErrorCode::conflict};
    if (kind == "archive") {
      keys(op, {"op", "id", "version"});
      auto &order = structural();
      std::vector<std::string> remove{id};
      for (const auto &candidate : order)
        if (string_field(*lookup(candidate), "parent") == id)
          remove.push_back(candidate);
      for (const auto &removed : remove) {
        p.changed.erase(removed);
        p.removed.push_back(removed);
        std::erase(order, removed);
      }
      continue;
    }
    auto row = *old;
    if (kind == "set") {
      keys(op, {"op", "id", "version", "title", "status", "owner", "note", "blocker"});
      for (const auto key : {"title", "status", "owner", "note", "blocker"})
        if (const auto *value = op.find(key))
          put(row, key, *value);
    } else if (kind == "move") {
      keys(op, {"op", "id", "version", "parent", "after"});
      auto &order = structural();
      const auto parent = resolve(op, "parent");
      if (!parent.empty()) {
        const auto *owner = lookup(parent);
        if (!owner || parent == id || !string_field(*owner, "parent").empty())
          throw Error{ErrorCode::invalid_range};
        for (const auto &candidate : order)
          if (string_field(*lookup(candidate), "parent") == id)
            throw Error{ErrorCode::invalid_range};
      }
      // Keep a moved top-level task and its children together.
      std::vector<std::string> children;
      for (const auto &candidate : order)
        if (string_field(*lookup(candidate), "parent") == id)
          children.push_back(candidate);
      for (const auto &child : children)
        std::erase(order, child);
      put(row, "parent", Value{parent});
      p.changed.emplace(id, row);
      place(id, parent, resolve(op, "after"));
      auto pos = std::find(order.begin(), order.end(), id) + 1;
      order.insert(pos, children.begin(), children.end());
    } else
      throw Error{ErrorCode::invalid_range};
    const auto version = integer(field(*old, "version"));
    if (version == UINT64_MAX)
      throw Error{ErrorCode::capacity};
    put(row, "version", number(version + 1));
    put(row, "updated", number(timestamp));
    validate_row(row);
    p.changed.insert_or_assign(id, std::move(row));
  }
  for (const auto &id : p.removed)
    if (const auto *old = item(id); old && string_field(*old, "parent").empty())
      --p.counts[status_index(*old)];
  for (const auto &[id, row] : p.changed) {
    if (const auto *old = item(id); old && string_field(*old, "parent").empty())
      --p.counts[status_index(*old)];
    if (string_field(row, "parent").empty())
      ++p.counts[status_index(row)];
  }
  const auto adjust_child = [&](const Value &row, bool adding) {
    const auto &parent = string_field(row, "parent");
    if (parent.empty())
      return;
    auto found = p.rollups.find(parent);
    if (found == p.rollups.end()) {
      const auto existing = rollups_.find(parent);
      found =
          p.rollups
              .emplace(parent, existing == rollups_.end() ? std::array<std::size_t, 2>{}
                                                          : existing->second)
              .first;
    }
    if (adding) {
      ++found->second[0];
      if (string_field(row, "status") == "done")
        ++found->second[1];
    } else {
      --found->second[0];
      if (string_field(row, "status") == "done")
        --found->second[1];
    }
  };
  for (const auto &id : p.removed)
    if (const auto *old = item(id))
      adjust_child(*old, false);
  for (const auto &[id, row] : p.changed) {
    if (const auto *old = item(id))
      adjust_child(*old, false);
    adjust_child(row, true);
  }
  Value::Array changed, removed;
  for (const auto &[id, row] : p.changed) {
    (void)id;
    changed.push_back(row);
  }
  for (const auto &id : p.removed)
    removed.push_back(Value{id});
  p.result = Value::object({{"accepted", Value{true}},
                            {"revision", number(p.revision)},
                            {"changed", Value{changed}},
                            {"removed", Value{removed}},
                            {"added", Value{std::move(added)}},
                            {"counts", counts_json(p.counts)}});
  auto receipt = Value::object(
      {{"op_id", field(request, "op_id")}, {"request", request}, {"result", p.result}});
  p.receipts.push_back(receipt);
  if (p.receipts.size() > retry_window)
    p.receipts.pop_front();
  p.packet = Value::object({{"label", Value{"task-delta-v1"}},
                            {"revision", number(p.revision)},
                            {"next", number(p.next)},
                            {"title", Value{p.title}},
                            {"bead", Value{p.bead}},
                            {"changed", Value{std::move(changed)}},
                            {"removed", Value{std::move(removed)}},
                            {"receipt", std::move(receipt)}});
  if (p.order) {
    Value::Array order;
    for (const auto &id : *p.order)
      order.push_back(Value{id});
    put(p.packet, "order", Value{std::move(order)});
  }
  return p;
}
void TaskState::commit(Prepared p) {
  for (const auto &id : p.removed) {
    items_.erase(id);
    rollups_.erase(id);
  }
  for (auto &[id, row] : p.changed)
    if (auto found = items_.find(id); found != items_.end())
      found->second = std::move(row);
  items_.merge(p.changed); // Transfer preallocated nodes; no whole-list candidate copy.
  std::erase_if(p.rollups,
                [&](const auto &entry) { return !items_.contains(entry.first); });
  for (const auto &[id, value] : p.rollups)
    if (auto found = rollups_.find(id); found != rollups_.end())
      found->second = value;
  rollups_.merge(p.rollups);
  if (p.order)
    order_.swap(*p.order);
  receipts_.swap(p.receipts);
  counts_ = p.counts;
  revision_ = p.revision;
  next_ = p.next;
  title_.swap(p.title);
  bead_.swap(p.bead);
}
void TaskState::replay(const Value &packet) {
  if (integer(field(packet, "revision")) != revision_ + 1)
    throw Error{ErrorCode::corrupt};
  auto prepared = prepare(field(field(packet, "receipt"), "request"));
  if (field(packet, "changed").array().size() != prepared.changed.size())
    throw Error{ErrorCode::corrupt};
  // Wall-clock timestamps are retained data, not recomputed on replay.
  for (const auto &row : field(packet, "changed").array()) {
    validate_row(row);
    const auto &id = string_field(row, "id");
    if (!prepared.changed.contains(id))
      throw Error{ErrorCode::corrupt};
    auto expected = prepared.changed.at(id);
    put(expected, "updated", field(row, "updated"));
    if (row != expected)
      throw Error{ErrorCode::corrupt};
    prepared.changed.at(id) = row;
  }
  auto expected = prepared.packet;
  auto result = prepared.result;
  put(result, "changed", field(packet, "changed"));
  auto receipt = field(expected, "receipt");
  put(receipt, "result", std::move(result));
  if (receipt != field(packet, "receipt"))
    throw Error{ErrorCode::corrupt};
  put(expected, "changed", field(packet, "changed"));
  put(expected, "receipt", field(packet, "receipt"));
  if (expected != packet)
    throw Error{ErrorCode::corrupt};
  prepared.receipts.back() = field(packet, "receipt");
  commit(std::move(prepared));
}
Value TaskState::snapshot() const {
  Value::Array rows, order, receipts;
  for (const auto &[id, row] : items_) {
    (void)id;
    rows.push_back(row);
  }
  for (const auto &id : order_)
    order.push_back(Value{id});
  for (const auto &receipt : receipts_)
    receipts.push_back(receipt);
  return Value::object({{"label", Value{"task-state-v1"}},
                        {"revision", number(revision_)},
                        {"next", number(next_)},
                        {"title", Value{title_}},
                        {"bead", Value{bead_}},
                        {"items", Value{std::move(rows)}},
                        {"order", Value{std::move(order)}},
                        {"receipts", Value{std::move(receipts)}}});
}
void TaskState::restore(const Value &snapshot) {
  TaskState candidate;
  candidate.revision_ = integer(field(snapshot, "revision"));
  candidate.next_ = integer(field(snapshot, "next"));
  bounded_text(field(snapshot, "title"), 512, false);
  bounded_text(field(snapshot, "bead"), 128);
  candidate.title_ = string_field(snapshot, "title");
  candidate.bead_ = string_field(snapshot, "bead");
  const auto &rows = field(snapshot, "items").array();
  if (rows.size() > max_items || !candidate.next_)
    throw Error{ErrorCode::corrupt};
  for (const auto &row : rows) {
    validate_row(row);
    const auto &id = string_field(row, "id");
    if (id.size() < 2 || id[0] != 't')
      throw Error{ErrorCode::corrupt};
    std::uint64_t ordinal = 0;
    const auto decoded = std::from_chars(id.data() + 1, id.data() + id.size(), ordinal);
    if (decoded.ec != std::errc{} || decoded.ptr != id.data() + id.size() || !ordinal ||
        ordinal >= candidate.next_ || id != "t" + std::to_string(ordinal))
      throw Error{ErrorCode::corrupt};
    if (!candidate.items_.emplace(id, row).second)
      throw Error{ErrorCode::corrupt};
    if (string_field(row, "parent").empty())
      ++candidate.counts_[status_index(row)];
  }
  std::set<std::string> seen;
  std::string parent;
  for (const auto &id : field(snapshot, "order").array()) {
    const auto *row = candidate.item(id.string());
    if (!row || !seen.insert(id.string()).second)
      throw Error{ErrorCode::corrupt};
    const auto &owner = string_field(*row, "parent");
    if (owner.empty())
      parent = id.string();
    else if (owner != parent)
      throw Error{ErrorCode::corrupt};
    candidate.order_.push_back(id.string());
  }
  if (seen.size() != rows.size())
    throw Error{ErrorCode::corrupt};
  for (const auto &[id, row] : candidate.items_) {
    (void)id;
    const auto &owner = string_field(row, "parent");
    if (!owner.empty()) {
      auto &rollup = candidate.rollups_[owner];
      ++rollup[0];
      if (string_field(row, "status") == "done")
        ++rollup[1];
    }
  }
  const auto &receipts = field(snapshot, "receipts").array();
  if (receipts.size() > retry_window)
    throw Error{ErrorCode::corrupt};
  for (const auto &receipt : receipts)
    candidate.receipts_.push_back(receipt);
  *this = std::move(candidate);
}
Value TaskState::read(const Value &query) const {
  keys(query, {"revision", "offset", "limit", "collapsed", "status", "id", "owner",
               "compact"});
  if (const auto *revision = query.find("revision");
      revision && integer(*revision) != revision_)
    return Value::object({{"conflict", Value{true}}, {"revision", number(revision_)}});
  const auto offset = query.find("offset") ? integer(field(query, "offset")) : 0;
  const auto limit = query.find("limit") ? integer(field(query, "limit")) : 32;
  if (!limit || limit > max_page || offset > order_.size())
    throw Error{ErrorCode::invalid_range};
  std::set<std::string> collapsed;
  if (const auto *values = query.find("collapsed")) {
    if (values->array().size() > max_items)
      throw Error{ErrorCode::invalid_range};
    for (const auto &id : values->array()) {
      bounded_text(id, 32, false);
      collapsed.insert(id.string());
    }
  }
  const auto *status = query.find("status");
  if (status && status->string() != "open" &&
      std::find(statuses.begin(), statuses.end(), status->string()) == statuses.end())
    throw Error{ErrorCode::invalid_range};
  const auto *selected = query.find("id");
  const auto *owner_filter = query.find("owner");
  if (selected)
    bounded_text(*selected, 32, false);
  if (owner_filter)
    bounded_text(*owner_filter, 128);
  Value::Array rows;
  std::size_t cursor = static_cast<std::size_t>(offset);
  for (; cursor < order_.size() && rows.size() < limit; ++cursor) {
    const auto &row = items_.at(order_[cursor]);
    if (selected && field(row, "id") != *selected)
      continue;
    if (owner_filter && field(row, "owner") != *owner_filter)
      continue;
    if (status && status->string() == "open" &&
        (string_field(row, "status") == "done" ||
         string_field(row, "status") == "dropped"))
      continue;
    if (status && status->string() != "open" && field(row, "status") != *status)
      continue;
    const auto &parent = string_field(row, "parent");
    if (!parent.empty() && collapsed.contains(parent))
      continue;
    auto visible = row;
    if (const auto *compact = query.find("compact");
        compact && std::get<bool>(compact->value()))
      std::erase_if(visible.object(), [](const auto &entry) {
        return entry.first == "note" || entry.first == "updated";
      });
    if (parent.empty()) {
      const auto found = rollups_.find(order_[cursor]);
      const auto rollup =
          found == rollups_.end() ? std::array<std::size_t, 2>{} : found->second;
      put(visible, "subtasks", number(rollup[0]));
      put(visible, "subtasks_done", number(rollup[1]));
    } else
      put(visible, "parent_title", field(items_.at(parent), "title"));
    rows.push_back(std::move(visible));
  }
  return Value::object({{"scope", Value{"session"}},
                        {"revision", number(revision_)},
                        {"title", Value{title_}},
                        {"bead", Value{bead_}},
                        {"counts", counts_json(counts_)},
                        {"total", number(items_.size())},
                        {"offset", number(offset)},
                        {"next", cursor < order_.size() ? number(cursor) : Value{}},
                        {"items", Value{std::move(rows)}},
                        {"retry_window", number(retry_window)}});
}
TaskStore::TaskStore(AuditLog &log) : log_(log) {
  for (const auto &packet : log.root().current_programs())
    if (string_field(packet, "label") == "task-state-v1")
      state_.restore(packet);
}
Value TaskStore::edit(const Value &request) {
  try {
    auto prepared = state_.prepare(request);
    if (prepared.packet == Value{})
      return prepared.result;
    auto result = prepared.result;
    auto packet = prepared.packet;
    log_.record(ApplicationChannel::program, packet);
    state_.commit(std::move(prepared));
    if (changed) {
      try {
        changed(packet);
      } catch (...) {
      } // Display failure cannot undo accepted work.
    }
    return result;
  } catch (const std::bad_variant_access &) {
    throw Error{ErrorCode::invalid_range};
  } catch (const Error &e) {
    if (e.code != ErrorCode::conflict)
      throw;
    Value::Array current;
    if (const auto *ops = request.find("ops"))
      if (const auto *array = std::get_if<Value::Array>(&ops->value()))
        for (std::size_t i = 0; i < std::min(TaskState::max_batch, array->size()); ++i)
          if (const auto *id = (*array)[i].find("id");
              id && std::holds_alternative<std::string>(id->value()))
            if (const auto *row = state_.item(id->string()))
              current.push_back(*row);
    return Value::object(
        {{"accepted", Value{false}},
         {"conflict", Value{true}},
         {"revision", number(state_.revision())},
         {"current", Value{std::move(current)}},
         {"message",
          Value{"Stale version/base or reused op_id. Read current rows; use a fresh "
                "op_id for a revised edit. Identical retries are remembered for the "
                "last 16 edits; older guarded retries conflict."}}});
  }
}
Value TaskStore::operator_command(std::string_view command) {
  if (command == "help")
    return Value::object(
        {{"help",
          Value{"/tasks [list] · add TITLE · sub ID TITLE · queued/active/done/dropped "
                "ID · "
                "block ID REASON · rename ID TITLE · note ID TEXT · owner ID NAME · "
                "title TEXT · bead ID · archive ID (includes children) · read JSON · "
                "apply JSON\n"
                "Pane: up · down · fold ID · expand · show · hide. Tasks and subtasks "
                "only."}}});
  if (command.empty() || command == "list")
    return read();
  if (command.starts_with("read "))
    return read(unwrap(parse_json(command.substr(5))));
  if (command.starts_with("apply "))
    return edit(unwrap(parse_json(command.substr(6))));
  const auto space = command.find(' ');
  const auto verb = command.substr(0, space);
  auto args =
      space == std::string_view::npos ? std::string_view{} : command.substr(space + 1);
  Value op;
  if (verb == "add")
    op = Value::object({{"op", Value{"add"}}, {"title", Value{std::string{args}}}});
  else if (verb == "title" || verb == "bead")
    op = Value::object(
        {{"op", Value{"list"}}, {std::string{verb}, Value{std::string{args}}}});
  else {
    const auto end = args.find(' ');
    const auto id = std::string{args.substr(0, end)};
    args = end == std::string_view::npos ? std::string_view{} : args.substr(end + 1);
    const auto *row = state_.item(id);
    if (!row)
      throw Error{ErrorCode::invalid_range};
    if (verb == "sub")
      op = Value::object({{"op", Value{"add"}},
                          {"parent", Value{id}},
                          {"title", Value{std::string{args}}}});
    else {
      op = Value::object({{"op", Value{verb == "archive" ? "archive" : "set"}},
                          {"id", Value{id}},
                          {"version", field(*row, "version")}});
      if (verb == "rename" || verb == "note" || verb == "owner")
        put(op, verb == "rename" ? "title" : verb, Value{std::string{args}});
      else if (verb == "block") {
        put(op, "status", Value{"blocked"});
        put(op, "blocker", Value{std::string{args}});
      } else if (verb != "archive") {
        if (std::find(statuses.begin(), statuses.end(), verb) == statuses.end())
          throw Error{ErrorCode::invalid_range};
        put(op, "status", Value{std::string{verb}});
        if (verb != "blocked")
          put(op, "blocker", Value{""});
      }
    }
  }
  return edit(Value::object({{"op_id", Value{hex_identity(log_.issue().bytes())}},
                             {"base", number(state_.revision())},
                             {"ops", Value{Value::Array{op}}}}));
}
} // namespace blackbird
