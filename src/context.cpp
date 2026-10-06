#include "arconaut/context.hpp"
#include <algorithm>
#include <charconv>
#include <filesystem>
#include <limits>
#include <set>

namespace arconaut {
std::string hex_identity(const IdentityBytes &bytes) {
  constexpr char digits[] = "0123456789abcdef";
  std::string out;
  out.reserve(32);
  for (const auto b : bytes) {
    const auto n = std::to_integer<unsigned>(b);
    out += digits[n >> 4U];
    out += digits[n & 15U];
  }
  return out;
}
const Json &field(const Json &value, std::string_view name) {
  const auto *f = value.find(name);
  if (f == nullptr)
    throw Error{ErrorCode::corrupt};
  return *f;
}
const std::string &string_field(const Json &value, std::string_view name) {
  const auto &f = field(value, name);
  const auto *s = std::get_if<std::string>(&f.value());
  if (s == nullptr)
    throw Error{ErrorCode::corrupt};
  return *s;
}
namespace {
std::vector<std::byte> bytes(std::string_view s) {
  auto view = std::as_bytes(std::span{s.data(), s.size()});
  return {view.begin(), view.end()};
}
std::string_view text(ByteView b) {
  return {reinterpret_cast<const char *>(b.data()), b.size()};
}
bool accepted(const Json &packet) {
  const auto &f = field(packet, "accepted");
  const auto *b = std::get_if<bool>(&f.value());
  if (b == nullptr)
    throw Error{ErrorCode::corrupt};
  return *b;
}
} // namespace
std::string AuditLog::record(ApplicationChannel channel, const Json &packet) {
  auto identity = issue();
  record(identity, channel, packet);
  return hex_identity(identity.bytes());
}
void AuditLog::record(ApplicationRecordId identity, ApplicationChannel channel,
                      const Json &packet) {
  const auto serialized = unwrap(dump_json(packet));
  (void)unwrap(
      root_.submit({{}, ApplicationRecordEvent{identity, channel, bytes(serialized)}}));
}
std::string AuditLog::original(OriginalCapture capture) {
  const auto label = capture.label;
  const auto raw = capture.bytes;
  auto metadata = std::move(capture.metadata);
  auto identity = issue();
  auto packet = Json::object(
      {{"label", Json{std::string{label}}}, {"metadata", std::move(metadata)}});
  const auto cursor = root_.cursor();
  if (cursor.sequence == UINT64_MAX)
    throw Error{ErrorCode::overflow};
  const SourceReference reference{cursor.journal, cursor.sequence + 1};
  const auto source = std::as_bytes(std::span{raw.data(), raw.size()});
  RetainedEvent event{{reference},
                      ApplicationRecordEvent{identity, ApplicationChannel::log,
                                             bytes(unwrap(dump_json(packet)))}};
  (void)unwrap(root_.append(cursor, std::span{&source, 1}, std::span{&event, 1}));
  return hex_identity(identity.bytes());
}
bool ContextStore::valid_entries(const Json &entries) const {
  const auto *array = std::get_if<Json::Array>(&entries.value());
  if (array == nullptr)
    return false;
  std::set<std::string> ids;
  for (const auto &entry : *array) {
    const auto *id = entry.find("id");
    const auto *item = entry.find("item");
    if (id == nullptr || item == nullptr ||
        !std::holds_alternative<std::string>(id->value()) ||
        !std::holds_alternative<Json::Object>(item->value()))
      return false;
    if (!originals_.contains(id->string()) || !ids.insert(id->string()).second)
      return false;
  }
  return true;
}
ContextStore::ContextStore(AuditLog &log) : log_(log) {
  for (const auto &fact : log.root().committed_facts()) {
    const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
    if (record == nullptr || record->channel != ApplicationChannel::context)
      continue;
    const auto packet = unwrap(parse_json(text(record->payload)));
    const auto revision = hex_identity(record->identity.bytes());
    if (string_field(packet, "revision") != revision ||
        string_field(packet, "observed") != head_)
      throw Error{ErrorCode::corrupt};
    history_.push_back(packet);
    const auto op = string_field(packet, "op");
    if (op != "append" && op != "edit" && op != "managed")
      throw Error{ErrorCode::corrupt};
    const bool publishes = accepted(packet);
    if (publishes) {
      if (string_field(packet, "base") != head_)
        throw Error{ErrorCode::corrupt};
      if (op == "append" || op == "managed") {
        const auto &fresh = field(packet, "originals");
        if (!std::holds_alternative<Json::Array>(fresh.value()))
          throw Error{ErrorCode::corrupt};
        for (const auto &entry : fresh.array()) {
          const auto &id = string_field(entry, "id");
          captured_.push_back(entry);
          if (!originals_.emplace(id, field(entry, "item")).second)
            throw Error{ErrorCode::corrupt};
        }
      }
      const auto &entries = field(packet, "entries");
      if (!valid_entries(entries))
        throw Error{ErrorCode::corrupt};
      entries_ = entries.array();
      head_ = revision;
    }
  }
}
Json ContextStore::view() const {
  return Json::object({{"base", Json{head_}}, {"entries", Json{entries_}}});
}
Json ContextStore::stats() const {
  auto number = [](std::size_t n) { return Json{JsonNumber{std::to_string(n)}}; };
  Json::Array sizes;
  for (const auto &entry : entries_)
    sizes.push_back(Json::object(
        {{"id", field(entry, "id")},
         {"item_bytes", number(unwrap(dump_json(field(entry, "item"))).size())}}));
  return Json::object({{"revision", Json{head_}},
                       {"entry_count", number(entries_.size())},
                       {"input_bytes", number(unwrap(dump_json(Json{items()})).size())},
                       {"view_bytes", number(unwrap(dump_json(view())).size())},
                       {"entries", Json{std::move(sizes)}},
                       {"units", Json{"serialized UTF-8 JSON bytes; not tokens"}}});
}
Json::Array ContextStore::items() const {
  Json::Array items;
  items.reserve(entries_.size());
  for (const auto &entry : entries_)
    items.push_back(field(entry, "item"));
  return items;
}
namespace {
void seed_hex(std::string_view value) {
  if (value.size() != 32 || value == std::string(32, '0') ||
      value.find_first_not_of("0123456789abcdef") != std::string_view::npos)
    throw Error{ErrorCode::invalid_range};
}
std::uint64_t seed_number(const Json &value) {
  const auto *n = std::get_if<JsonNumber>(&value.value());
  if (!n || n->text.empty())
    throw Error{ErrorCode::invalid_range};
  std::uint64_t number = 0;
  const auto r =
      std::from_chars(n->text.data(), n->text.data() + n->text.size(), number);
  if (r.ec != std::errc{} || r.ptr != n->text.data() + n->text.size())
    throw Error{ErrorCode::invalid_range};
  return number;
}
} // namespace
void ContextStore::validate_successor_seed(const Json &seed) {
  if (unwrap(dump_json(seed)).size() > 1024 * 1024 ||
      seed_number(field(seed, "version")) != 1)
    throw Error{ErrorCode::invalid_range};
  const auto &source = field(seed, "source");
  const auto &path = string_field(source, "session");
  if (path.size() > 65536 || path.find('\0') != std::string::npos ||
      !std::filesystem::path{path}.is_absolute())
    throw Error{ErrorCode::invalid_range};
  for (const auto key : {"environment", "journal", "context_revision"})
    seed_hex(string_field(source, key));
  (void)seed_number(field(source, "prefix_sequence"));
  if (seed_number(field(source, "prefix_end")) < journal_header_size)
    throw Error{ErrorCode::invalid_range};
  const auto &state = string_field(source, "status");
  if (state != "settled" && state != "unsettled" && state != "unknown")
    throw Error{ErrorCode::invalid_range};
  const auto &reason = string_field(source, "reason");
  if (reason.empty() || reason.size() > 65536)
    throw Error{ErrorCode::invalid_range};
  const auto &selected = field(seed, "entries");
  if (!std::holds_alternative<Json::Array>(selected.value()))
    throw Error{ErrorCode::invalid_range};
  const auto &entries = selected.array();
  if (entries.empty() || entries.size() > 4096)
    throw Error{ErrorCode::invalid_range};
  std::set<std::string> locators, seen, pending;
  for (const auto &entry : entries) {
    const auto &id = string_field(entry, "id");
    if (id.size() < 34 || id.size() > 53 || id[32] != '.' ||
        !locators.insert(id).second)
      throw Error{ErrorCode::invalid_range};
    seed_hex(std::string_view{id}.substr(0, 32));
    (void)seed_number(Json{JsonNumber{id.substr(33)}});
    const auto &item = field(entry, "item");
    if (!std::holds_alternative<Json::Object>(item.value()))
      throw Error{ErrorCode::invalid_range};
    const auto *role = item.find("role");
    const auto *type = item.find("type");
    if (role) {
      if (!std::holds_alternative<std::string>(role->value()))
        throw Error{ErrorCode::invalid_range};
      const auto &r = role->string();
      if (r != "user" && r != "system" && r != "developer" && r != "assistant")
        throw Error{ErrorCode::invalid_range};
    }
    if (!type && !role)
      throw Error{ErrorCode::invalid_range};
    if (!type || (std::holds_alternative<std::string>(type->value()) &&
                  type->string() == "message")) {
      if (!role)
        throw Error{ErrorCode::invalid_range};
      const auto &content = field(item, "content");
      if (const auto *parts = std::get_if<Json::Array>(&content.value())) {
        for (const auto &part : *parts) {
          const auto &kind = string_field(part, "type");
          if (kind == "input_text" || kind == "output_text")
            (void)string_field(part, "text");
          else if (kind == "refusal")
            (void)string_field(part, "refusal");
          else
            throw Error{ErrorCode::invalid_range};
        }
      } else if (!std::holds_alternative<std::string>(content.value()))
        throw Error{ErrorCode::invalid_range};
      if (!type)
        continue;
    }
    if (!std::holds_alternative<std::string>(type->value()))
      throw Error{ErrorCode::invalid_range};
    const auto &t = type->string();
    if (t != "message" && t != "reasoning" && t != "function_call" &&
        t != "function_call_output")
      throw Error{ErrorCode::invalid_range};
    if (t != "message" && role)
      throw Error{ErrorCode::invalid_range};
    if (t == "reasoning") {
      // store:false requires self-contained encrypted reasoning, not an old ID.
      if (string_field(item, "encrypted_content").empty())
        throw Error{ErrorCode::invalid_range};
      const auto &summary = field(item, "summary");
      if (!std::holds_alternative<Json::Array>(summary.value()))
        throw Error{ErrorCode::invalid_range};
      for (const auto &part : summary.array()) {
        if (string_field(part, "type") != "summary_text")
          throw Error{ErrorCode::invalid_range};
        (void)string_field(part, "text");
      }
    }
    if (t == "function_call") {
      if (string_field(item, "name").empty())
        throw Error{ErrorCode::invalid_range};
      (void)string_field(item, "arguments");
      const auto &call = string_field(item, "call_id");
      if (call.empty() || !seen.insert(call).second)
        throw Error{ErrorCode::conflict};
      pending.insert(call);
    } else if (t == "function_call_output") {
      (void)string_field(item, "output");
      if (pending.erase(string_field(item, "call_id")) != 1)
        throw Error{ErrorCode::conflict};
    }
  }
  if (!pending.empty())
    throw Error{ErrorCode::conflict};
}
void ContextStore::seed_successor(const Json &seed) {
  validate_successor_seed(seed);
  if (workflow_ || pending_ || !entries_.empty() ||
      !log_.root().committed_facts().empty() ||
      log_.root().state() != JournalWriterState::live ||
      string_field(field(seed, "source"), "journal") ==
          hex_identity(log_.root().cursor().journal.bytes()))
    throw Error{ErrorCode::conflict};
  Json::Array items, locators;
  for (const auto &entry : field(seed, "entries").array()) {
    items.push_back(field(entry, "item"));
    locators.push_back(Json{string_field(entry, "id")});
  }
  const auto lineage = Json::object(
      {{"version", Json{JsonNumber{"1"}}},
       {"scope",
        Json{"declared source; no inherited admissions or settlement verification"}},
       {"source", field(seed, "source")},
       {"source_entries", Json{std::move(locators)}}});
  append_impl(std::move(items), "session.successor", &lineage);
}
void ContextStore::append(Json::Array items, std::string_view origin) {
  append_impl(std::move(items), origin, nullptr);
}
void ContextStore::append_impl(Json::Array items, std::string_view origin,
                               const Json *lineage) {
  for (const auto &item : items)
    if (!std::holds_alternative<Json::Object>(item.value()))
      throw Error{ErrorCode::corrupt};
  auto identity = log_.issue();
  auto revision = hex_identity(identity.bytes());
  auto next = entries_;
  auto originals = originals_;
  Json::Array fresh;
  for (std::size_t i = 0; i < items.size(); ++i) {
    const auto id = revision + "." + std::to_string(i);
    auto entry = Json::object({{"id", Json{id}}, {"item", items[i]}});
    originals.emplace(id, std::move(items[i]));
    fresh.push_back(entry);
    next.push_back(std::move(entry));
  }
  auto packet = Json::object({{"op", Json{"append"}},
                              {"base", Json{head_}},
                              {"observed", Json{head_}},
                              {"revision", Json{revision}},
                              {"accepted", Json{true}},
                              {"origin", Json{std::string{origin}}},
                              {"originals", Json{std::move(fresh)}},
                              {"entries", Json{next}}});
  if (lineage)
    packet.object().emplace_back("lineage", *lineage);
  auto history = history_;
  history.push_back(packet);
  auto captured = captured_;
  for (const auto &entry : field(packet, "originals").array())
    captured.push_back(entry);
  auto expected = revision;
  log_.record(identity, ApplicationChannel::context, packet);
  history_.swap(history);
  captured_.swap(captured);
  if (pending_ && pending_->expected == head_)
    pending_->expected.swap(expected);
  entries_.swap(next);
  originals_.swap(originals);
  head_.swap(revision);
}
Json ContextStore::edit(const Json &candidate) {
  const auto *base = candidate.find("base");
  const auto *entries = candidate.find("entries");
  const bool representation = base != nullptr &&
                              std::holds_alternative<std::string>(base->value()) &&
                              entries != nullptr && valid_entries(*entries);
  const bool publishes = representation && base->string() == head_;
  auto identity = log_.issue();
  auto revision = hex_identity(identity.bytes());
  Json::Array next;
  if (publishes)
    next = entries->array();
  auto outcome =
      Json::object({{"accepted", Json{publishes}},
                    {"revision", Json{revision}},
                    {"current", Json{publishes ? revision : head_}},
                    {"reason", Json{publishes        ? "published"
                                    : representation ? "stale-base"
                                                     : "invalid-representation"}}});
  auto packet = Json::object({{"op", Json{"edit"}},
                              {"base", base == nullptr ? Json{} : *base},
                              {"observed", Json{head_}},
                              {"revision", Json{revision}},
                              {"accepted", Json{publishes}},
                              {"candidate", candidate},
                              {"entries", entries == nullptr ? Json{} : *entries},
                              {"outcome", outcome}});
  auto history = history_;
  history.push_back(packet);
  log_.record(identity, ApplicationChannel::context, packet);
  history_.swap(history);
  if (publishes) {
    entries_.swap(next);
    head_.swap(revision);
  }
  return outcome;
}
void ContextStore::restore(std::string_view entry) {
  const auto original = originals_.find(std::string{entry});
  if (original == originals_.end())
    throw Error{ErrorCode::invalid_identity};
  auto next = entries_;
  bool present = false;
  for (auto &value : next)
    if (string_field(value, "id") == entry) {
      value =
          Json::object({{"id", Json{std::string{entry}}}, {"item", original->second}});
      present = true;
      break;
    }
  if (!present)
    next.push_back(
        Json::object({{"id", Json{std::string{entry}}}, {"item", original->second}}));
  (void)edit(Json::object({{"base", Json{head_}}, {"entries", Json{std::move(next)}}}));
}
Json ContextStore::originals() const {
  Json::Array out;
  for (const auto &[id, item] : originals_)
    out.push_back(Json::object({{"id", Json{id}}, {"item", item}}));
  return Json{std::move(out)};
}
// Managed transformations share the native context journal and publication owner.
namespace {
bool protocol_complete(const Json::Array &entries) {
  std::set<std::string> seen, pending;
  for (const auto &entry : entries) {
    const auto &item = field(entry, "item");
    const auto *type = item.find("type");
    if (!type)
      continue;
    if (!std::holds_alternative<std::string>(type->value()))
      return false;
    if (type->string() == "function_call") {
      const auto &id = string_field(item, "call_id");
      if (id.empty() || !seen.insert(id).second)
        return false;
      pending.insert(id);
    } else if (type->string() == "function_call_output") {
      if (pending.erase(string_field(item, "call_id")) != 1)
        return false;
    }
  }
  return pending.empty();
}
bool whole_groups(const Json::Array &entries, const std::set<std::string> &ids) {
  std::set<std::string> pending;
  std::size_t start = 0;
  for (std::size_t i = 0; i < entries.size(); ++i) {
    const auto &item = field(entries[i], "item");
    const auto *type = item.find("type");
    if (!type || !std::holds_alternative<std::string>(type->value()))
      continue;
    if (type->string() == "function_call") {
      if (pending.empty())
        start = i;
      pending.insert(string_field(item, "call_id"));
    } else if (type->string() == "function_call_output") {
      if (pending.erase(string_field(item, "call_id")) != 1)
        return false;
      if (pending.empty()) {
        const bool selected = ids.contains(string_field(entries[start], "id"));
        for (std::size_t j = start; j <= i; ++j)
          if (ids.contains(string_field(entries[j], "id")) != selected)
            return false;
      }
    }
  }
  // Open current tool batches are allowed only if untouched.
  if (!pending.empty())
    for (std::size_t j = start; j < entries.size(); ++j)
      if (ids.contains(string_field(entries[j], "id")))
        return false;
  return true;
}
Json number(std::size_t n) { return Json{JsonNumber{std::to_string(n)}}; }
std::size_t index_field(const Json &q, std::string_view key, std::size_t fallback) {
  const auto *v = q.find(key);
  if (!v)
    return fallback;
  if (!std::holds_alternative<JsonNumber>(v->value()))
    throw Error{ErrorCode::invalid_range};
  std::size_t n = 0;
  const auto &s = v->number().text;
  const auto r = std::from_chars(s.data(), s.data() + s.size(), n);
  if (r.ec != std::errc{} || r.ptr != s.data() + s.size())
    throw Error{ErrorCode::invalid_range};
  return n;
}
} // namespace
Json ContextStore::reject_managed(const Json &proposal, std::string_view why) {
  auto identity = log_.issue();
  auto revision = hex_identity(identity.bytes());
  auto outcome = Json::object({{"accepted", Json{false}},
                               {"reason", Json{std::string{why}}},
                               {"current", Json{head_}},
                               {"revision", Json{revision}}});
  auto packet = Json::object({{"op", Json{"managed"}},
                              {"revision", Json{revision}},
                              {"observed", Json{head_}},
                              {"accepted", Json{false}},
                              {"candidate", proposal},
                              {"outcome", outcome}});
  auto history = history_;
  history.push_back(packet);
  log_.record(identity, ApplicationChannel::context, packet);
  history_.swap(history);
  return outcome;
}
void ContextStore::begin_workflow() {
  if (workflow_)
    throw Error{ErrorCode::conflict};
  workflow_ = true;
}
Json ContextStore::manage(const Json &proposal) {
  try {
    if (string_field(proposal, "base") != head_)
      return reject_managed(proposal, "stale-base");
    if (pending_)
      return reject_managed(proposal, "pending-overlap");
    const auto &mode = string_field(proposal, "mode");
    if (mode != "select" && mode != "archive" && mode != "summarize" &&
        mode != "restore")
      return reject_managed(proposal, "invalid-mode");
    if (string_field(proposal, "reason").empty() ||
        string_field(proposal, "source").empty())
      return reject_managed(proposal, "missing-source-or-reason");
    const auto &ids = field(proposal, "ids");
    if (!std::holds_alternative<Json::Array>(ids.value()))
      return reject_managed(proposal, "invalid-ids");
    std::set<std::string> selected;
    for (const auto &id : ids.array()) {
      if (!std::holds_alternative<std::string>(id.value()) ||
          !originals_.contains(id.string()) || !selected.insert(id.string()).second)
        return reject_managed(proposal, "invalid-ids");
      if (mode != "restore" &&
          std::none_of(entries_.begin(), entries_.end(), [&](const Json &e) {
            return string_field(e, "id") == id.string();
          }))
        return reject_managed(proposal, "absent-id");
    }
    if (mode == "summarize") {
      const auto &summary = field(proposal, "summary");
      if (selected.empty() || !std::holds_alternative<Json::Object>(summary.value()) ||
          summary.find("type") ||
          (string_field(summary, "role") != "developer" &&
           string_field(summary, "role") != "assistant") ||
          string_field(summary, "content").empty())
        return reject_managed(proposal, "invalid-summary");
    }
    if (!whole_groups(mode == "restore" ? captured_ : entries_, selected))
      return reject_managed(proposal, "partial-tool-group");
    for (const auto &entry : entries_) {
      const auto &id = string_field(entry, "id");
      const auto *role = field(entry, "item").find("role");
      const bool pinned = role && (*role == Json{"user"} || *role == Json{"system"} ||
                                   *role == Json{"developer"});
      if (pinned &&
          ((mode == "select" && !selected.contains(id)) ||
           ((mode == "archive" || mode == "summarize") && selected.contains(id))))
        return reject_managed(proposal, "mandatory-entry");
    }
    if (!workflow_)
      return publish_managed(proposal, entries_, "");
    auto result = reject_managed(proposal, "staged");
    pending_ = Pending{proposal, entries_, head_, string_field(result, "revision")};
    result.object().emplace_back("staged", Json{true});
    return result;
  } catch (const Error &e) {
    if (e.code != ErrorCode::corrupt && e.code != ErrorCode::invalid_range)
      throw;
    return reject_managed(proposal, "malformed-proposal");
  }
}
Json ContextStore::finish_workflow(bool success) {
  workflow_ = false;
  if (!pending_)
    return Json{};
  auto pending = std::move(*pending_);
  pending_.reset();
  if (!success) {
    if (log_.root().state() != JournalWriterState::live)
      return Json::object({{"accepted", Json{false}},
                           {"reason", Json{"workflow-cancelled-audit-unavailable"}}});
    return reject_managed(pending.proposal, "workflow-cancelled");
  }
  if (pending.expected != head_)
    return reject_managed(pending.proposal, "settlement-conflict");
  return publish_managed(pending.proposal, pending.snapshot, pending.stage);
}
Json ContextStore::publish_managed(const Json &proposal, const Json::Array &basis,
                                   std::string_view stage) {
  const auto &mode = string_field(proposal, "mode");
  std::set<std::string> selected;
  for (const auto &id : field(proposal, "ids").array())
    selected.insert(id.string());
  auto identity = log_.issue();
  auto revision = hex_identity(identity.bytes());
  Json::Array next, fresh;
  std::set<std::string> pending_calls, active_entries;
  std::size_t open_start = 0;
  for (std::size_t i = 0; i < basis.size(); ++i) {
    const auto &item = field(basis[i], "item");
    const auto *type = item.find("type");
    if (!type || !std::holds_alternative<std::string>(type->value()))
      continue;
    if (type->string() == "function_call") {
      if (pending_calls.empty())
        open_start = i;
      pending_calls.insert(string_field(item, "call_id"));
    } else if (type->string() == "function_call_output") {
      pending_calls.erase(string_field(item, "call_id"));
    }
  }
  if (!pending_calls.empty())
    for (std::size_t i = open_start; i < basis.size(); ++i)
      active_entries.insert(string_field(basis[i], "id"));
  bool inserted = false;
  for (const auto &entry : basis) {
    const bool chosen = selected.contains(string_field(entry, "id"));
    if (mode == "summarize" && chosen && !inserted) {
      auto summary = Json::object({{"id", Json{revision + ".0"}},
                                   {"item", field(proposal, "summary")},
                                   {"ancestry", field(proposal, "ids")}});
      next.push_back(summary);
      fresh.push_back(summary);
      inserted = true;
    }
    if ((mode == "select" &&
         (chosen || active_entries.contains(string_field(entry, "id")))) ||
        ((mode == "archive" || mode == "summarize") && !chosen) || mode == "restore")
      next.push_back(entry);
  }
  // Explicit append-only continuation settlement, retained separately from input base.
  if (!stage.empty()) {
    if (entries_.size() < basis.size() ||
        !std::equal(basis.begin(), basis.end(), entries_.begin()))
      return reject_managed(proposal, "settlement-conflict");
    next.insert(next.end(),
                entries_.begin() + static_cast<std::ptrdiff_t>(basis.size()),
                entries_.end());
  }
  if (mode == "restore") {
    // Remove selected presentations first, then merge exact originals in capture order.
    // This also repairs a reordered/edited group, rather than retaining its bad order.
    std::erase_if(next, [&](const Json &entry) {
      return selected.contains(string_field(entry, "id"));
    });
    for (const auto &original : captured_) {

      const auto &id = string_field(original, "id");
      if (!selected.contains(id))
        continue;
      auto pos = std::find_if(next.begin(), next.end(), [&](const Json &entry) {
        return string_field(entry, "id") == id;
      });
      if (pos != next.end()) {
        *pos = original;
        continue;
      }
      // Insert before the first present original captured later, never reorder live
      // entries.
      bool later = false;
      std::set<std::string> successors;
      for (const auto &entry : captured_) {
        if (later)
          successors.insert(string_field(entry, "id"));
        if (string_field(entry, "id") == id)
          later = true;
      }
      pos = std::find_if(next.begin(), next.end(), [&](const Json &entry) {
        return successors.contains(string_field(entry, "id"));
      });
      next.insert(pos, original);
    }
  }
  if (!protocol_complete(next))
    return reject_managed(proposal, "invalid-tool-protocol");
  auto originals = originals_;
  auto captured = captured_;
  for (const auto &entry : fresh) {
    originals.emplace(string_field(entry, "id"), field(entry, "item"));
    captured.push_back(entry);
  }
  auto outcome = Json::object({{"accepted", Json{true}},
                               {"reason", Json{"published"}},
                               {"revision", Json{revision}},
                               {"current", Json{revision}}});
  auto packet = Json::object({{"op", Json{"managed"}},
                              {"base", Json{head_}},
                              {"observed", Json{head_}},
                              {"revision", Json{revision}},
                              {"accepted", Json{true}},
                              {"candidate", proposal},
                              {"stage", Json{std::string{stage}}},
                              {"entries", Json{next}},
                              {"originals", Json{fresh}},
                              {"outcome", outcome}});
  auto history = history_;
  history.push_back(packet);
  log_.record(identity, ApplicationChannel::context, packet);
  history_.swap(history);
  originals_.swap(originals);
  captured_.swap(captured);
  entries_.swap(next);
  head_.swap(revision);
  return outcome;
}
Json ContextStore::inspect(const Json &query) const {
  const auto &kind = string_field(query, "kind");
  const auto &snapshot = kind == "history" && !history_.empty()
                             ? string_field(history_.back(), "revision")
                             : head_;
  if (query.find("revision") && string_field(query, "revision") != snapshot)
    throw Error{ErrorCode::conflict};
  const auto requested = index_field(query, "offset", 0);
  const auto limit = index_field(query, "limit", 4096);
  if (limit == 0 || limit > 65536)
    throw Error{ErrorCode::invalid_range};
  // Virtual JSON concatenation: never materialize the aggregate retained history.
  // Each audited record already passed the ordinary JSON document limits.
  std::size_t total = 0;
  std::string page;
  auto emit = [&](std::string_view raw) {
    if (raw.size() > std::numeric_limits<std::size_t>::max() - total)
      throw Error{ErrorCode::capacity};
    const auto begin = total;
    total += raw.size();
    if (total > requested && page.size() < limit) {
      const auto from = requested > begin ? requested - begin : 0;
      page.append(raw.substr(from, std::min(limit - page.size(), raw.size() - from)));
    }
  };
  auto emit_array = [&](const Json::Array &entries) {
    emit("[");
    bool first = true;
    for (const auto &entry : entries) {
      if (!first)
        emit(",");
      first = false;
      emit(unwrap(dump_json(entry)));
    }
    emit("]");
  };
  if (kind == "history")
    emit_array(history_);
  else if (kind == "index") {
    emit("[");
    std::size_t capture_index = 0;
    for (const auto &entry : captured_) {
      if (capture_index != 0)
        emit(",");
      auto item = Json::object(
          {{"id", field(entry, "id")},
           {"item_bytes", number(unwrap(dump_json(field(entry, "item"))).size())}});
      item.object().emplace_back("capture_index", number(capture_index++));
      item.object().emplace_back("capture_revision",
                                 Json{string_field(entry, "id").substr(0, 32)});
      if (const auto *a = entry.find("ancestry"))
        item.object().emplace_back("ancestry", *a);
      emit(unwrap(dump_json(item)));
    }
    emit("]");
  } else if (kind == "originals") {
    if (const auto *id = query.find("entry")) {
      const auto found =
          std::find_if(captured_.begin(), captured_.end(),
                       [&](const Json &entry) { return field(entry, "id") == *id; });
      if (found == captured_.end())
        throw Error{ErrorCode::invalid_identity};
      emit(unwrap(dump_json(*found)));
    } else
      emit_array(captured_);
  } else
    throw Error{ErrorCode::invalid_range};
  constexpr char digits[] = "0123456789abcdef";
  std::string hex;
  for (const auto byte : page) {
    const auto c = static_cast<unsigned char>(byte);
    hex += digits[c >> 4U];
    hex += digits[c & 15U];
  }
  const auto offset = std::min(requested, total);
  return Json::object({{"encoding", Json{"hex-json-utf8"}},
                       {"bytes", Json{std::move(hex)}},
                       {"total_bytes", number(total)},
                       {"offset", number(offset)},
                       {"next", number(offset + page.size())},
                       {"revision", Json{snapshot}},
                       {"context_revision", Json{head_}}});
}

} // namespace arconaut
