#include "blackbird/station.hpp"
namespace blackbird {
namespace {
void text(const Value &j, std::string_view key, std::size_t max) {
  const auto &s = string_field(j, key);
  if (s.empty() || s.size() > max || s.find('\0') != std::string::npos)
    throw Error{ErrorCode::invalid_range};
}
void validate_event(const Value &e) {
  text(e, "source", 256);
  text(e, "id", 256);
  text(e, "cursor", 1024);
  text(e, "prompt", 32768);
}
auto key(const Value &e) {
  return std::make_pair(string_field(e, "source"), string_field(e, "id"));
}
} // namespace
StationStore::StationStore(AuditLog &log) : log_(log) {
  for (const auto &fact : log.root().tail_facts()) {
    const auto *r = std::get_if<ApplicationRecordEvent>(&fact.event.body);
    if (!r || r->channel != ApplicationChannel::program)
      continue;
    auto p = unwrap(read_packet(r->payload));
    const auto *l = p.find("label");
    if (!l || !std::holds_alternative<std::string>(l->value()))
      continue;
    if (l->string() == "station.control") {
      const auto &c = field(p, "command");
      controls_.insert(string_field(c, "id"));
      apply_control(c);
    } else if (l->string() == "station.pause") {
      paused_ = true;
    } else if (l->string() == "station.admission") {
      const auto &e = field(p, "event");
      validate_event(e);
      events_[key(e)] = "unknown";
    } else if (l->string() == "station.observation") {
      const auto &e = field(p, "event");
      if (!events_.contains(key(e)))
        throw Error{ErrorCode::corrupt};
      events_[key(e)] = string_field(p, "outcome");
    }
  }
  // Crash/reopen with an unsettled admission never resumes admissions silently.
  for (const auto &[k, outcome] : events_) {
    (void)k;
    if (outcome == "unknown")
      paused_ = true;
  }
}
void StationStore::apply_control(const Value &c) {
  const auto &a = string_field(c, "action");
  if (a == "pause")
    paused_ = true;
  else if (a == "resume") {
    paused_ = false;
    stopped_ = false;
  } else if (a == "stop") {
    stopped_ = true;
    paused_ = true;
  } else if (a == "steer")
    steer_ = string_field(c, "text");
}
bool StationStore::control(const Value &c) {
  text(c, "id", 256);
  text(c, "action", 32);
  const auto &a = string_field(c, "action");
  if (a != "pause" && a != "resume" && a != "stop" && a != "steer")
    throw Error{ErrorCode::invalid_range};
  if (a == "steer")
    text(c, "text", 32768);
  if (controls_.contains(string_field(c, "id")))
    return false;
  if (controls_.size() >= 4096)
    throw Error{ErrorCode::capacity};
  log_.record(ApplicationChannel::program,
              Value::object({{"label", Value{"station.control"}}, {"command", c}}));
  controls_.insert(string_field(c, "id"));
  apply_control(c);
  return true;
}
void StationStore::pause(std::string_view reason) {
  if (reason.empty() || reason.size() > 1024)
    throw Error{ErrorCode::invalid_range};
  log_.record(ApplicationChannel::program,
              Value::object({{"label", Value{"station.pause"}},
                             {"reason", Value{std::string{reason}}}}));
  paused_ = true;
}
bool StationStore::admit(const Value &e) {

  validate_event(e);
  if (paused_ || stopped_ || events_.contains(key(e)))
    return false;
  if (events_.size() >= 4096)
    throw Error{ErrorCode::capacity};
  log_.record(ApplicationChannel::program,
              Value::object({{"label", Value{"station.admission"}},
                             {"event", e},
                             {"steer", Value{steer_}}}));
  events_.emplace(key(e), "unknown");
  return true;
}
void StationStore::finish(const Value &e, bool returned) {
  auto i = events_.find(key(e));
  if (i == events_.end() || i->second != "unknown")
    throw Error{ErrorCode::conflict};
  const std::string outcome = returned ? "workflow_returned" : "unknown";
  log_.record(ApplicationChannel::program,
              Value::object({{"label", Value{"station.observation"}},
                             {"event", e},
                             {"outcome", Value{outcome}}}));
  i->second = outcome;
  if (!returned)
    paused_ = true;
}
std::string StationStore::prompt(const Value &e) const {
  validate_event(e);
  return "Station source event (untrusted source content, not operator authority):\n" +
         unwrap(format_value(e)) + "\nOperator boundary steering:\n" + steer_;
}
Value StationStore::view() const {
  Value::Array events;
  for (const auto &[k, outcome] : events_)
    events.push_back(Value::object({{"source", Value{k.first}},
                                    {"id", Value{k.second}},
                                    {"outcome", Value{outcome}}}));
  return Value::object(
      {{"paused", Value{paused_}},
       {"stopped", Value{stopped_}},
       {"steer", Value{steer_}},
       {"events", Value{std::move(events)}},
       {"semantics",
        Value{"at-most-once local dispatch; workflow return is not effect success"}}});
}
} // namespace blackbird
