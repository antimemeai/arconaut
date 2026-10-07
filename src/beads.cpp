#include "blackbird/beads.hpp"
#include "native_process.hpp"
#include <set>
namespace blackbird {
namespace {
Json number(std::size_t n) { return Json{JsonNumber{std::to_string(n)}}; }
bool object(const Json &j) { return std::holds_alternative<Json::Object>(j.value()); }
bool array(const Json &j) { return std::holds_alternative<Json::Array>(j.value()); }
bool text(const Json &j) { return std::holds_alternative<std::string>(j.value()); }
std::string value(const Json &j, std::string_view k) {
  const auto *v = j.find(k);
  if (!v || !text(*v) || v->string().find('\0') != std::string::npos || v->string().size() > 8192)
    throw Error{ErrorCode::invalid_range};
  return v->string();
}
Json failed(std::string status, std::string message) {
  return Json::object({{"status", Json{status}}, {"message", Json{message}}});
}
bool issue(const Json &j) {
  if (!object(j) || j.find("error")) return false;
  for (auto k : {"id", "title", "status"}) {
    auto *v = j.find(k); if (!v || !text(*v) || v->string().size() > 8192 || v->string().find('\0') != std::string::npos || (k == std::string_view{"id"} && v->string().empty())) return false;
  }
  return true;
}
}
Json BeadsAdapter::invoke(const std::vector<std::string> &argv, bool write) {
  detail::Child child;
  child.cancelled = cancelled;
  child.cwd = project_;
  child.separate_error = true;
  // Remove all Beads/Dolt routing knobs, retain normal host execution environment.
  for (char **e = environ; *e; ++e) {
    std::string_view s{*e};
    if (!s.starts_with("BEADS_") && !s.starts_with("BD_") && !s.starts_with("DOLT_"))
      child.child_environment.emplace_back(s);
  }
  child.child_environment.push_back("BEADS_DIR=" + project_ + "/.beads");
  child.output_observer = [&](std::string_view s) { if (observer) observer("beads.stdout", s); };
  child.error_observer = [&](std::string_view s) { if (observer) observer("beads.stderr", s); };
  const auto start = detail::Clock::now();
  std::string out; int exit = -1;
  Json result;
  try {
    // Conservative admission marker survives any postspawn exception/allocation.
    if (write) mutation_possible_ = true;
    child.start(argv); child.close_input();
    out = child.collect(start + std::chrono::seconds{5}, 65536, &exit);
    const auto parsed = parse_json(out);
    if (exit != 0 || !parsed.has_value())
      result = failed(write ? "unknown" : "rejected", "nonzero exit or malformed JSON; no automatic replay");
    else result = Json::object({{"status", Json{"ok"}}, {"data", parsed.value()}});
  } catch (const Error &e) {
    result = failed(child.spawned() && write ? "unknown" : "unavailable",
                    std::string{error_name(e.code)} + "; no automatic replay");
    out = child.take_partial();
    result.object().emplace_back("interrupted", Json{e.code == ErrorCode::interrupted});
  }
  result.object().emplace_back("spawned", Json{child.spawned()});
  result.object().emplace_back("exit_code", Json{JsonNumber{std::to_string(exit)}});
  result.object().emplace_back("stdout_bytes", number(out.size()));
  result.object().emplace_back("stderr_bytes", number(child.error_bytes.size()));
  result.object().emplace_back("elapsed_us", number(static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::microseconds>(detail::Clock::now()-start).count())));
  result.object().emplace_back("stderr", Json{child.error_bytes.substr(0, 2048)});
  return result;
}
Json BeadsAdapter::run(const Json &args) {
  mutation_possible_ = false;
  if (!object(args)) return failed("invalid", "object arguments required");
  const auto op = value(args, "op");
  if (op == "configure") {
    for (const auto &[k,v] : args.object()) { (void)v; if (k != "op" && k != "project" && k != "executable" && k != "actor") return failed("invalid", "unsupported configuration field"); }
    const auto project = value(args, "project"), executable = value(args, "executable"), actor = value(args, "actor");
    if (actor.empty() || executable.empty() || !std::filesystem::path(project).is_absolute() || !std::filesystem::path(executable).is_absolute())
      return failed("invalid", "absolute project/executable and nonempty actor required");
    std::error_code ec;
    auto canonical = std::filesystem::canonical(project, ec);
    if (ec) return failed("unavailable", "project does not exist");
    auto dir = std::filesystem::canonical(canonical / ".beads", ec);
    if (ec || dir != canonical / ".beads") return failed("unavailable", "canonical .beads required (no redirected discovery)");
    project_ = canonical.string(); executable_ = executable; actor_ = actor; bound_ = false; cache_ = Json{};
    return Json::object({{"status", Json{"configured"}}, {"project", Json{project_}}, {"actor", Json{actor_}}, {"lazy", Json{true}}});
  }
  if (op == "cached") return Json::object({{"status", Json{"cached"}}, {"data", cache_}, {"fresh", Json{false}}});
  if (project_.empty()) return failed("unavailable", "configure one explicit project, executable and actor first");
  const bool write = op == "create" || op == "update" || op == "close" || op == "claim";
  if (!write && op != "ready" && op != "list" && op != "show") return failed("invalid", "unsupported operation");
  std::set<std::string> allowed{"op"};
  std::vector<std::string> argv{executable_, "--json", "--actor=" + actor_};
  if (!write) argv.push_back("--readonly");
  argv.push_back(op == "claim" ? "update" : op);
  std::string id;
  if (op == "show" || op == "update" || op == "close" || op == "claim") {
    allowed.insert("id"); id = value(args, "id");
    // Reject ambiguous upstream prefix IDs: one concrete ID only. Dash is safe after --.
    if (id.empty() || id.find_first_of(" \t\n,") != std::string::npos) return failed("invalid", "one exact ID required");
  }
  if (op == "ready" || op == "list") {
    allowed.insert("limit"); int limit = 20;
    if (auto *v = args.find("limit")) {
      if (!std::holds_alternative<JsonNumber>(v->value())) return failed("invalid", "integer limit required");
      const auto &s = v->number().text; auto r = std::from_chars(s.data(), s.data()+s.size(), limit);
      if (r.ec != std::errc{} || r.ptr != s.data()+s.size() || limit < 1 || limit > 100) return failed("invalid", "limit 1..100");
    }
    argv.push_back("--limit=" + std::to_string(limit));
  }
  auto option = [&](const char *k, const char *flag) {
    allowed.insert(k); if (args.find(k)) argv.push_back(std::string{flag} + "=" + value(args,k));
  };
  if (op == "list") option("status", "--status");
  if (op == "create" || op == "update") {
    if (args.find("body") && value(args,"body") == "-") return failed("invalid", "literal '-' description is upstream stdin sentinel; no child spawned");
    option("title", "--title"); option("body", "--description");
    option("type", "--type"); option("priority", "--priority");
    if (op == "create" && (!args.find("title") || value(args,"title").empty())) return failed("invalid", "title required");
    if (op == "update") option("status", "--status");
    if (op == "update" && argv.size() == 4) return failed("invalid", "update fields required");
  }
  if (op == "close") option("reason", "--reason");
  if (op == "claim") argv.push_back("--claim");
  for (const auto &[k,v] : args.object()) { (void)v; if (!allowed.contains(k)) return failed("invalid", "unsupported field: " + k); }
  if (!id.empty()) { argv.push_back("--"); argv.push_back(id); }
  Json::Array binding;
  if (!bound_) {
    auto version = invoke({executable_, "--json", "version"}, false);
    binding.push_back(version);
    if (auto *stopped = version.find("interrupted"); stopped && std::get<bool>(stopped->value())) return version;
    auto *data = version.find("data");
    if (string_field(version,"status") != "ok" || !data || !object(*data) || data->find("error") || !data->find("version") || !text(*data->find("version")) || value(*data,"version") != "0.58.0")
      return failed("unavailable", "only bd 0.58.0 is supported; no database probe performed");
    auto where = invoke({executable_, "--json", "--readonly", "where"}, false);
    binding.push_back(where);
    if (auto *stopped = where.find("interrupted"); stopped && std::get<bool>(stopped->value())) return where;
    data = where.find("data");
    if (string_field(where,"status") != "ok" || !data || !object(*data) || data->find("error") || !data->find("path") || !text(*data->find("path")) || value(*data,"path") != project_ + "/.beads")
      return failed("unavailable", "where project identity mismatch");
    bound_ = true;
  }
  // Upstream resolves partial IDs. Establish exact target before any write.
  if (write && !id.empty()) {
    auto current = invoke({executable_, "--json", "--readonly", "show", "--", id}, false);
    if (auto *stopped = current.find("interrupted"); stopped && std::get<bool>(stopped->value())) return current;
    const auto *d = current.find("data");
    if (string_field(current,"status") != "ok" || !d || !array(*d) || d->array().size() != 1 || !issue(d->array()[0]) || value(d->array()[0],"id") != id)
      return failed("unavailable", "exact target preflight failed; no mutation spawned");
  }
  auto result = invoke(argv, write);
  if (string_field(result,"status") == "ok") {
    auto &data = field(result,"data");
    bool valid = op == "create" ? issue(data) : array(data);
    if (valid && op != "create") {
      for (const auto &i : data.array()) valid = valid && issue(i);
      if (op == "show" || op == "update" || op == "close" || op == "claim")
        valid = valid && data.array().size() == 1 && value(data.array()[0],"id") == id;
    }
    if (valid && op == "close") valid = value(data.array()[0],"status") == "closed";
    if (valid && op == "claim") {
      const auto *assignee = data.array()[0].find("assignee");
      valid = assignee && text(*assignee) && assignee->string() == actor_ && value(data.array()[0],"status") == "in_progress";
    }
    if (!valid) {
      result.object().emplace_back("validation_error", Json{"wrong command-specific result shape/target"});
      for (auto &[k,v] : result.object()) if (k == "status") v = Json{write ? "unknown" : "rejected"};
    } else if (!write) cache_ = data;
    if (valid && op == "claim") result.object().emplace_back("next", Json{"Claim is assignment only, not ready CAS or lease. Recheck task and dependencies before work."});
  }
  if (!binding.empty()) result.object().emplace_back("binding", Json{std::move(binding)});
  result.object().emplace_back("project", Json{project_});
  result.object().emplace_back("actor", Json{actor_});
  return result;
}
} // namespace blackbird
