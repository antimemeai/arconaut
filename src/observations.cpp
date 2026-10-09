#include "blackbird/observations.hpp"
#include "native_process.hpp"
#include <algorithm>
#include <charconv>
#include <chrono>
#include <map>
#include <set>
#include <unistd.h>
namespace blackbird {
namespace {
Json integer(std::int64_t n) { return Json{JsonNumber{std::to_string(n)}}; }
std::int64_t numeric(const Json &v) {
  const auto *n = std::get_if<JsonNumber>(&v.value());
  if (!n)
    throw Error{ErrorCode::invalid_range};
  std::int64_t out = 0;
  const auto r = std::from_chars(n->text.data(), n->text.data() + n->text.size(), out);
  if (r.ec != std::errc{} || r.ptr != n->text.data() + n->text.size())
    throw Error{ErrorCode::invalid_range};
  return out;
}
bool oid(std::string_view text) {
  return (text.size() == 40 || text.size() == 64) &&
         std::all_of(text.begin(), text.end(), [](char c) {
           return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
         });
}
std::string line(std::string text) {
  while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
    text.pop_back();
  return text;
}
std::string display(std::string_view text) {
  std::string out;
  for (char c : text) {
    const auto b = static_cast<unsigned char>(c);
    out += b < 32 || b == 127 ? ' ' : c;
  }
  return out;
}
} // namespace
ObservationTime observation_time() {
  const auto before = std::chrono::steady_clock::now();
  const auto utc = std::chrono::system_clock::now();
  const auto after = std::chrono::steady_clock::now();
  return {
      std::chrono::duration_cast<std::chrono::nanoseconds>(utc.time_since_epoch())
          .count(),
      std::chrono::duration_cast<std::chrono::nanoseconds>(before.time_since_epoch())
          .count(),
      static_cast<std::uint64_t>(
          std::chrono::duration_cast<std::chrono::nanoseconds>(after - before)
              .count())};
}
Observations::Observations(AuditLog &log, std::function<ObservationTime()> clock)
    : log_(log), clock_(std::move(clock)),
      clock_id_(hex_identity(log.issue().bytes())) {
  char host[256]{};
  const bool known = gethostname(host, sizeof(host) - 1) == 0;
  log_.record(
      ApplicationChannel::log,
      Json::object(
          {{"label", Json{"clock.domain"}},
           {"clock_id", Json{clock_id_}},
           {"host", Json{known ? host : "unknown"}},
           {"pid", integer(getpid())},
           {"time", time()},
           {"synchronization", Json{"unknown"}},
           {"scope",
            Json{"local process clock mapping; cross-host alignment unverified"}}}));
}
Json Observations::time() const {
  const auto t = clock_();
  return Json::object(
      {{"clock_id", Json{clock_id_}},
       {"utc_ns", integer(t.utc_ns)},
       {"monotonic_ns", integer(t.monotonic_ns)},
       {"sampling_span_ns", Json{JsonNumber{std::to_string(t.sampling_span_ns)}}},
       {"synchronization", Json{"unknown"}}});
}
Json Observations::sample(std::string_view name, const Json &value, const Json &source,
                          std::string_view status) {
  if (name.empty() || name.size() > 64 ||
      !std::all_of(name.begin(), name.end(),
                   [](char c) {
                     return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                            c == '.' || c == '_' || c == '-';
                   }) ||
      (status != "observed" && status != "unavailable") ||
      !std::holds_alternative<Json::Object>(source.value()) ||
      unwrap(dump_json(value)).size() > 4096 || unwrap(dump_json(source)).size() > 4096)
    throw Error{ErrorCode::invalid_range};
  const auto identity = log_.issue();
  auto packet = Json::object({{"label", Json{"variable.sample"}},
                              {"sample_id", Json{hex_identity(identity.bytes())}},
                              {"variable", Json{std::string{name}}},
                              {"value", value},
                              {"source", source},
                              {"observation_status", Json{std::string{status}}},
                              {"time", time()}});
  // issue precedes cursor selection, including any issuer reservation record.
  const auto record = log_.root().fact_count();
  log_.record(identity, ApplicationChannel::log, packet);
  packet.object().emplace_back("record", Json{JsonNumber{std::to_string(record)}});
  packet.object().emplace_back(
      "journal", Json{hex_identity(log_.root().cursor().journal.bytes())});
  return packet;
}
void Observations::doctrine(std::string_view content, InvocationId invocation,
                            OperationAttemptId attempt) {
  if (doctrine_id_.empty() || content != doctrine_content_) {
    auto next_content = std::string{content};
    auto next_id = hex_identity(log_.issue().bytes());
    doctrine_content_.swap(next_content);
    doctrine_id_.swap(next_id);
  }
  sample(
      "doctrine.effective", Json::object({{"content_observation", Json{doctrine_id_}}}),
      Json::object(
          {{"kind", Json{"native_request_field"}},
           {"invocation", Json{hex_identity(invocation.bytes())}},
           {"attempt", Json{hex_identity(attempt.bytes())}},
           {"field", Json{"instructions"}},
           {"identity_scope",
            Json{
                "consecutive equal instruction bytes within this process observer"}}}));
}
Json Observations::git(const Json &query, const std::function<bool()> &cancelled) {
  if (!std::holds_alternative<Json::Object>(query.value()))
    throw Error{ErrorCode::invalid_range};
  for (const auto &[key, value] : query.object()) {
    (void)value;
    if (key != "path" && key != "history")
      throw Error{ErrorCode::invalid_range};
  }
  const auto path = string_field(query, "path");
  if (path.empty() || path.size() > 4096 || path.find('\0') != std::string::npos)
    throw Error{ErrorCode::invalid_range};
  const auto count = query.find("history") ? numeric(field(query, "history")) : 0;
  if (count < 0 || count > 64)
    throw Error{ErrorCode::invalid_range};
  auto run = [&](std::vector<std::string> args, int &code) {
    detail::Child child;
    child.cancelled = cancelled;
    // Environment Git overrides must not silently change the requested repository.
    for (char **e = environ; *e; ++e)
      if (!std::string_view{*e}.starts_with("GIT_"))
        child.child_environment.emplace_back(*e);
    child.child_environment.emplace_back("GIT_OPTIONAL_LOCKS=0");
    child.child_environment.emplace_back("GIT_TERMINAL_PROMPT=0");
    child.separate_error = true;
    child.start(std::move(args));
    child.close_input();
    return child.collect(detail::Clock::now() + std::chrono::seconds{5}, 65536, &code);
  };
  int code = 0;
  const auto repo =
      line(run({"/usr/bin/git", "-C", path, "rev-parse", "--show-toplevel"}, code));
  if (code != 0) {
    auto observed = sample("git.head", Json{},
                           Json::object({{"kind", Json{"git_cli"}},
                                         {"path", Json{path}},
                                         {"exit_code", integer(code)}}),
                           "unavailable");
    return Json::object({{"samples", Json{Json::Array{observed}}},
                         {"scope", Json{"repository lookup failed"}}});
  }
  const auto head =
      line(run({"/usr/bin/git", "-C", path, "rev-parse", "--verify", "HEAD"}, code));
  auto source = Json::object({{"kind", Json{"git_cli"}},
                              {"repository", Json{repo}},
                              {"requested_path", Json{path}},
                              {"exit_code", integer(code)}});
  Json::Array samples;
  samples.push_back(sample("git.head", code == 0 && oid(head) ? Json{head} : Json{},
                           source,
                           code == 0 && oid(head) ? "observed" : "unavailable"));
  if (code == 0 && oid(head) && count > 0) {
    // Traverse the observed immutable commit, not a potentially moved HEAD.
    const auto raw = run({"/usr/bin/git", "-C", path, "log", "--no-show-signature",
                          "--format=%H%x00%at%x00%ct%x00%P%x00", "-n",
                          std::to_string(count), head, "--"},
                         code);
    if (code != 0)
      throw Error{ErrorCode::io, code};
    std::size_t offset = 0;
    while (offset < raw.size()) {
      if (raw[offset] == '\n') {
        ++offset;
        continue;
      }
      std::array<std::string, 4> fields;
      for (auto &f : fields) {
        const auto end = raw.find('\0', offset);
        if (end == std::string::npos)
          throw Error{ErrorCode::corrupt};
        f = raw.substr(offset, end - offset);
        offset = end + 1;
      }
      if (!oid(fields[0]) || samples.size() > static_cast<std::size_t>(count))
        throw Error{ErrorCode::corrupt};
      const auto author = numeric(Json{JsonNumber{fields[1]}});
      const auto committer = numeric(Json{JsonNumber{fields[2]}});
      samples.push_back(
          sample("git.commit",
                 Json::object({{"commit", Json{fields[0]}},
                               {"author_unix_seconds", integer(author)},
                               {"committer_unix_seconds", integer(committer)},
                               {"parents", Json{fields[3]}}}),
                 source));
    }
  }
  return Json::object(
      {{"samples", Json{std::move(samples)}},
       {"scope",
        Json{"point observations; commit metadata times are not activation times; "
             "HEAD is not loaded executable identity; worktree bytes not observed"}}});
}
std::string correlation_plot(const Json &page) {
  struct Point {
    std::int64_t utc;
    std::string clock, lane, label, record;
  };
  std::vector<Point> points;
  std::size_t undated = 0;
  std::set<std::string> clocks;
  for (const auto &row : field(page, "events").array()) {
    const auto *t = row.find("time");
    const auto *clock_value = t ? t->find("clock_id") : nullptr;
    const auto *utc_value = t ? t->find("utc_ns") : nullptr;
    if (!clock_value || !utc_value ||
        !std::holds_alternative<std::string>(clock_value->value())) {
      ++undated;
      continue;
    }
    std::int64_t utc;
    try {
      utc = numeric(*utc_value);
    } catch (const Error &) {
      ++undated;
      continue;
    }
    const auto clock = clock_value->string();
    const auto *variable = row.find("variable");
    const auto *operation = row.find("operation");
    const auto *label = row.find("label");
    const auto lane = variable    ? variable->string()
                      : operation ? "work." + operation->string()
                      : label     ? label->string()
                                  : string_field(row, "event");
    const auto value = row.find("value");
    points.push_back({utc, clock, lane,
                      value ? unwrap(dump_json(*value)) : string_field(row, "event"),
                      field(row, "record").number().text});
    clocks.insert(clock);
  }
  if (points.empty())
    return "No timed observations in this page; undated=" + std::to_string(undated) +
           "\n";
  auto low = points[0].utc, high = low;
  for (const auto &p : points) {
    low = std::min(low, p.utc);
    high = std::max(high, p.utc);
  }
  std::map<std::string, std::string> lanes;
  for (const auto &p : points) {
    auto &lane = lanes[p.lane];
    if (lane.empty())
      lane = std::string(61, ' ');
    const auto fraction =
        high == low
            ? 0.0L
            : (static_cast<long double>(p.utc) - static_cast<long double>(low)) /
                  (static_cast<long double>(high) - static_cast<long double>(low));
    const auto position =
        std::min<std::size_t>(60, static_cast<std::size_t>(fraction * 60.0L));
    lane[position] = lane[position] == ' ' ? 'o' : '+';
  }
  std::string out = "UTC ns " + std::to_string(low) + " .. " + std::to_string(high) +
                    " (local wall clocks; synchronization unknown)\n";
  for (const auto &[name, lane] : lanes)
    out += display(name) + " |" + lane + "|\n";
  out += "o=observed point; +=overlap at this width; no intervals inferred; undated=" +
         std::to_string(undated) + " clocks=" + std::to_string(clocks.size()) + "\n";
  for (const auto &p : points)
    out += "#" + p.record + " " + std::to_string(p.utc) + " " + display(p.lane) + " " +
           display(p.label) + " clock=" + p.clock + "\n";
  out += "next=" + field(page, "next").number().text +
         " end=" + field(page, "end").number().text + "\n";
  return out;
}
} // namespace blackbird
