// Bounded, local candidate checkout ownership. No provider or activation authority.
#include "arconaut/json.hpp"
#include "native_process.hpp"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/file.h>

using arconaut::Json;
namespace fs = std::filesystem;
namespace {
Json text(const std::string &s) { return Json{s}; }
Json num(std::size_t n) { return Json{arconaut::JsonNumber{std::to_string(n)}}; }
[[noreturn]] void refuse(const std::string &s) { throw std::runtime_error(s); }
std::string dump(const Json &j) {
  auto r = arconaut::dump_json(j);
  if (!r.has_value())
    refuse("JSON encoding failed");
  return r.value();
}
Json read(const fs::path &p) {
  std::ifstream f(p, std::ios::binary);
  if (!f)
    refuse("cannot read " + p.string());
  std::string s((std::istreambuf_iterator<char>(f)), {});
  auto r = arconaut::parse_json(s);
  if (!r.has_value())
    refuse("invalid JSON: " + p.string());
  return r.value();
}
const Json &get(const Json &j, std::string_view k) {
  auto v = j.find(k);
  if (!v)
    refuse("missing " + std::string(k));
  return *v;
}
std::string str(const Json &j, std::string_view k) { return get(j, k).string(); }
std::size_t integer(const Json &j, std::string_view k) {
  const auto &s = get(j, k).number().text;
  std::size_t n = 0;
  auto r = std::from_chars(s.data(), s.data() + s.size(), n);
  if (r.ec != std::errc{} || r.ptr != s.data() + s.size())
    refuse("invalid integer");
  return n;
}
void set(Json &j, std::string k, Json v) {
  for (auto &[key, value] : j.object())
    if (key == k) {
      value = std::move(v);
      return;
    }
  j.object().emplace_back(std::move(k), std::move(v));
}
Json &member(Json &j, const std::string &k) {
  for (auto &[key, value] : j.object())
    if (key == k)
      return value;
  refuse("missing " + k);
}
class Lock {
  int fd = -1;

public:
  explicit Lock(const fs::path &p, bool attempt = false) {
    fd = ::open(p.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600);
    if (fd < 0)
      refuse("lock open failed");
    if (::flock(fd, LOCK_EX | (attempt ? LOCK_NB : 0)) != 0) {
      ::close(fd);
      fd = -1;
      refuse("busy lease");
    }
  }
  Lock(const Lock &) = delete;
  Lock &operator=(const Lock &) = delete;
  ~Lock() {
    if (fd >= 0)
      ::close(fd);
  }
  void unlock() {
    if (::flock(fd, LOCK_UN) != 0)
      refuse("unlock failed");
  }
  void lock() {
    if (::flock(fd, LOCK_EX) != 0)
      refuse("lock failed");
  }
};
// Atomic replacement with durable file and directory. Journal originals never replaced.
void write(const fs::path &p, std::string_view s) {
  auto pattern = p.string() + ".tmp.XXXXXX";
  std::vector<char> path(pattern.begin(), pattern.end());
  path.push_back(0);
  int fd = ::mkstemp(path.data());
  if (fd < 0)
    refuse("write open failed");
  std::size_t off = 0;
  while (off < s.size()) {
    auto n = ::write(fd, s.data() + off, s.size() - off);
    if (n < 0 && errno == EINTR)
      continue;
    if (n <= 0) {
      ::close(fd);
      refuse("write failed");
    }
    off += static_cast<std::size_t>(n);
  }
  const bool synced = ::fsync(fd) == 0;
  ::close(fd);
  if (!synced || ::rename(path.data(), p.c_str()) != 0)
    refuse("publish failed");
  fd = ::open(p.parent_path().c_str(), O_RDONLY | O_CLOEXEC);
  if (fd < 0)
    refuse("directory open failed");
  const int rc = ::fsync(fd);
  ::close(fd);
  if (rc != 0)
    refuse("directory sync failed");
}
std::string git(const fs::path &repo, std::vector<std::string> args) {
  args.insert(args.begin(), {"git", "-C", repo.string()});
  arconaut::detail::Child child;
  child.start(std::move(args), {}, true);
  child.close_input();
  auto out = child.collect(arconaut::detail::Clock::now() + std::chrono::seconds(60),
                           1024 * 1024);
  while (!out.empty() && (out.back() == '\n' || out.back() == '\r'))
    out.pop_back();
  return out;
}
void name(const std::string &s) {
  if (s.empty() || s.size() > 80 || s.front() == '-' ||
      s.find("..") != std::string::npos)
    refuse("invalid candidate name");
  for (char c : s)
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_')
      refuse("invalid candidate name");
}
fs::path inside(const fs::path &base, const std::string &s) {
  fs::path p(s);
  if (p.empty() || p.is_absolute())
    refuse("relative file required");
  for (const auto &part : p)
    if (part == ".." || part == ".git")
      refuse("unsafe file");
  auto b = fs::canonical(base);
  auto v = fs::canonical(base / p);
  auto rel = v.lexically_relative(b);
  if (rel.empty() || *rel.begin() == ".." || !fs::is_regular_file(v))
    refuse("file outside checkout");
  return v;
}
void stopped(const Json &r) {
  if (str(r, "stopped") != "all checkout programs/builds stopped")
    refuse("explicit stopped evidence required");
  if (str(r, "evidence").empty())
    refuse("nonempty observed evidence required");
}
class Manager {
  fs::path root;
  Lock global;
  Json state;

public:
  explicit Manager(fs::path p) : root(std::move(p)), global(root / "lock") {
    if (fs::exists(root / "state.json"))
      state = read(root / "state.json");
  }
  fs::path repo() { return str(state, "repo"); }
  fs::path checkout(std::size_t i) {
    auto path = root / ("slot-" + std::to_string(i));
    if (fs::is_symlink(fs::symlink_status(path)))
      refuse("slot symlink refused; primary must not be aliased");
    if (fs::exists(path)) {
      auto canonical = fs::canonical(path);
      if (canonical == fs::canonical(repo()))
        refuse("slot cannot be primary checkout");
      auto list = git(repo(), {"worktree", "list", "--porcelain"});
      if (("\n" + list + "\n").find("\nworktree " + canonical.string() + "\n") ==
          std::string::npos)
        refuse("slot is not an owned linked worktree");
    }
    return path;
  }
  Json &slot(std::size_t i) {
    auto &s = member(state, "slots").array();
    if (i >= s.size())
      refuse("slot outside configured pool");
    return s[i];
  }
  Json &candidate(const std::string &n) {
    for (auto &c : member(state, "candidates").array())
      if (str(c, "name") == n)
        return c;
    refuse("unknown candidate");
  }
  // Return locator. A new immutable snapshot is durable BEFORE any dispatched effect.
  std::string save(const std::string &action, const Json &evidence) {
    fs::create_directories(root / "records");
    auto pattern = (root / "records" / "event-XXXXXX").string();
    std::vector<char> p(pattern.begin(), pattern.end());
    p.push_back(0);
    int fd = ::mkstemp(p.data());
    if (fd < 0)
      refuse("event allocation failed");
    ::close(fd);
    const std::string id = fs::path(p.data()).filename().string();
    auto old = state.find("record");
    Json parent = old ? *old : Json{};
    set(state, "record", text(id));
    write(p.data(), dump(Json::object({{"parent", parent},
                                       {"action", text(action)},
                                       {"evidence", evidence},
                                       {"state", state}})));
    write(root / "state.json", dump(state));
    return id;
  }
  void clean(std::size_t i) {
    if (!git(checkout(i), {"status", "--porcelain"}).empty())
      refuse("dirty checkout: checkpoint named files first");
  }
  void leased(std::size_t i) {
    if (str(slot(i), "status") != "leased")
      refuse("lease unknown/not settled; no replay");
  }
  Json perform(const std::string &cmd, const Json &r) {
    if (cmd == "init") {
      if (!std::holds_alternative<std::nullptr_t>(state.value()))
        refuse("already initialized");
      auto cap = integer(r, "cap");
      auto limit = r.find("limit") ? integer(r, "limit") : std::size_t{4};
      if (cap < 1 || cap > limit)
        refuse("cap exceeds configured limit (pilot default 4; explicit limit "
               "configurable)");
      auto primary = fs::canonical(str(r, "repo"));
      primary = git(primary, {"rev-parse", "--show-toplevel"});
      if (fs::canonical(root) == primary)
        refuse("pool cannot be primary checkout");
      Json::Array slots;
      for (std::size_t i = 0; i < cap; ++i)
        slots.push_back(
            Json::object({{"status", text("empty")}, {"candidate", text("")}}));
      state = Json::object({{"repo", text(primary.string())},
                            {"slots", Json{slots}},
                            {"candidates", Json{Json::Array{}}}});
      save(cmd, r);
      return state;
    }
    if (!state.find("repo"))
      refuse("init required");
    if (cmd == "status")
      return state;
    if (cmd == "record") {
      for (const auto &k : {"question", "action", "outcome", "references"})
        if (!r.find(k))
          refuse("question/action/outcome/references required");
      auto id = save("experiment.record", r);
      return Json::object(
          {{"record", text(id)}, {"path", text((root / "records" / id).string())}});
    }
    if (cmd == "enqueue") {
      auto n = str(r, "name");
      name(n);
      for (auto &c : member(state, "candidates").array())
        if (str(c, "name") == n)
          refuse("duplicate candidate");
      // Freeze exact starting source; reject branch-name ambiguity on later dispatch.
      auto base = git(repo(), {"rev-parse", "--verify", str(r, "base") + "^{commit}"});
      for (const auto &k : {"hypothesis", "discriminator", "identities", "baseline"})
        if (!r.find(k) || dump(get(r, k)) == "null" || dump(get(r, k)) == "\"\"")
          refuse("missing experiment declaration");
      auto branch = "candidate/" + n;
      if (!git(repo(), {"branch", "--list", branch}).empty())
        refuse("branch already exists; choose a new candidate identity");
      member(state, "candidates")
          .array()
          .push_back(Json::object({{"name", text(n)},
                                   {"branch", text(branch)},
                                   {"base", text(base)},
                                   {"status", text("queued")},
                                   {"declaration", r},
                                   {"checkpoint", Json{}},
                                   {"artifacts", Json{Json::Array{}}},
                                   {"activation", Json{}}}));
      save(cmd, r);
      return candidate(n);
    }
    if (cmd == "dispatch") {
      std::size_t i = 0;
      for (; i < get(state, "slots").array().size(); ++i)
        if (str(slot(i), "status") == "empty")
          break;
      if (i == get(state, "slots").array().size())
        return Json::object(
            {{"queued", Json{true}}, {"reason", text("pool full; cap unchanged")}});
      Json *c = nullptr;
      for (auto &v : member(state, "candidates").array())
        if (str(v, "status") == "queued") {
          c = &v;
          break;
        }
      if (!c)
        return Json::object({{"queued", Json{false}}, {"reason", text("queue empty")}});
      Lock held(root / ("slot-" + std::to_string(i) + ".lock"), true);
      if (fs::exists(checkout(i)))
        clean(i);
      auto n = str(*c, "name"), branch = str(*c, "branch"), base = str(*c, "base");
      set(slot(i), "candidate", text(n));
      set(slot(i), "status", text("unknown"));
      set(*c, "status", text("transition"));
      save("dispatch.intent", r);
      if (!fs::exists(checkout(i)))
        git(repo(), {"worktree", "add", "-b", branch, checkout(i).string(), base});
      else
        git(checkout(i), {"checkout", "-b", branch, base});
      set(slot(i), "status", text("leased"));
      set(candidate(n), "status", text("leased"));
      save("dispatch.observed", r);
      return Json::object({{"slot", num(i)},
                           {"path", text(checkout(i).string())},
                           {"candidate", candidate(n)}});
    }
    auto i = integer(r, "slot");
    Lock held(root / ("slot-" + std::to_string(i) + ".lock"), true);
    auto &s = slot(i);
    auto n = str(s, "candidate");
    if (n.empty())
      refuse("empty slot");
    if (cmd == "abandon") {
      stopped(r);
      if (str(s, "status") != "unknown")
        refuse("abandon only uncertain transitions");
      auto raw = root / ("slot-" + std::to_string(i));
      if (fs::is_symlink(fs::symlink_status(raw)))
        refuse("never reconcile a slot alias");
      if (fs::exists(raw)) {
        if (str(r, "reconcile") == "retain clean prior checkout") {
          clean(i); // Ownership and clean original branch independently observed.
          if (git(raw, {"branch", "--show-current"}) != str(r, "observed_branch") ||
              git(raw, {"rev-parse", "HEAD"}) != str(r, "observed_head"))
            refuse("reconciliation identity mismatch");
        } else if (str(r, "reconcile") ==
                   "quarantine partial directory without execution") {
          // Explicit caller choice, never a retry or destructive cleanup. Raw
          // contents survive; stale Git registration may need separately observed
          // repair. Do not claim this is a newly certified execution checkout.
          auto id = save("abandon.quarantine.intent", r);
          fs::create_directories(root / "quarantine");
          auto destination = root / "quarantine" / id;
          fs::rename(raw, destination);
          int directory =
              ::open(destination.parent_path().c_str(), O_RDONLY | O_CLOEXEC);
          if (directory < 0)
            refuse("quarantine directory open failed");
          const int synced = ::fsync(directory);
          ::close(directory);
          if (synced != 0)
            refuse("quarantine directory sync failed; outcome unknown");
          set(s, "quarantine", text(destination.string()));
        } else
          refuse("present checkout needs observed reconciliation or preservation");
      }
      auto &c = candidate(n);
      // A missing checkout is observed, not a license to retry the effect.
      auto branches = git(repo(), {"branch", "--list", str(c, "branch")});
      if (!branches.empty())
        set(c, "checkpoint", text(git(repo(), {"rev-parse", str(c, "branch")})));
      set(c, "status", text("abandoned transition; retained source/records"));
      set(s, "status", text("empty"));
      set(s, "candidate", text(""));
      save("abandon.observed", r);
      return c;
    }
    if (cmd == "settle") {
      stopped(r);
      if (git(checkout(i), {"branch", "--show-current"}) != str(candidate(n), "branch"))
        refuse("branch mismatch; inspect transition, do not replay");
      set(s, "status", text("leased"));
      set(candidate(n), "status", text("leased"));
      save("settle.observed", r);
      return s;
    }
    leased(i);
    if (cmd == "run") {
      auto seconds = integer(r, "seconds");
      if (seconds < 1 || seconds > 3600)
        refuse("seconds must be 1..3600");
      std::vector<std::string> args{"/bin/sh", "-c",
                                    "cd -- \"$1\" && shift && exec \"$@\"",
                                    "candidate-run", checkout(i).string()};
      for (auto &v : get(r, "argv").array())
        args.push_back(v.string());
      if (args.size() == 5)
        refuse("nonempty argv required");
      set(s, "status", text("running"));
      set(candidate(n), "status", text("running"));
      auto id = save("run.intent", r);
      global.unlock();
      Json outcome;
      arconaut::detail::Child child;
      try {
        child.start(std::move(args), {}, true);
        child.close_input();
        int code = 0;
        auto out = child.collect(arconaut::detail::Clock::now() +
                                     std::chrono::seconds(seconds),
                                 16 * 1024 * 1024, &code);
        write(root / "records" / (id + ".output"), out);
        outcome = Json::object(
            {{"exit_code", num(static_cast<std::size_t>(code))},
             {"outcome",
              text("observed leader exit; descendants not certified stopped")}});
      } catch (...) {
        write(root / "records" / (id + ".output"), child.take_partial());
        outcome = Json::object({{"outcome", text("unknown; no automatic replay")}});
      }
      // Child destruction below releases any unfinished owned child. Slot remains
      // unknown regardless: explicit stopped evidence required before further use.
      global.lock();
      state = read(root / "state.json");
      set(slot(i), "status", text("unknown"));
      set(slot(i), "last_run", text(id));
      set(candidate(n), "status", text("unknown"));
      save("run.observation", outcome);
      return Json::object(
          {{"run", text(id)},
           {"output", text((root / "records" / (id + ".output")).string())},
           {"observation", outcome}});
    }
    stopped(r);
    auto &c = candidate(n);
    if (cmd == "checkpoint") {
      auto files = get(r, "files").array();
      if (!git(checkout(i), {"diff", "--cached", "--name-only"}).empty())
        refuse("preexisting staged changes: inspect and unstage explicitly first");
      for (auto &v : files)
        (void)inside(checkout(i), v.string());
      set(s, "status", text("unknown"));
      save("checkpoint.intent", r);
      if (!files.empty()) {
        std::vector<std::string> args{"add", "--"};
        for (auto &v : files)
          args.push_back(v.string());
        git(checkout(i), args);
        if (!git(checkout(i), {"diff", "--cached", "--name-only"}).empty())
          git(checkout(i), {"commit", "-m", str(r, "message")});
      }
      clean(i);
      set(c, "checkpoint", text(git(checkout(i), {"rev-parse", "HEAD"})));
      set(s, "status", text("leased"));
      save("checkpoint.observed", r);
      return c;
    }
    if (cmd == "artifact") {
      auto file = inside(checkout(i), str(r, "file"));
      auto hash = git(checkout(i), {"hash-object", file.string()});
      fs::create_directories(root / "artifacts");
      auto destination = root / "artifacts" / hash;
      if (!fs::exists(destination)) {
        std::ifstream f(file, std::ios::binary);
        std::string bytes((std::istreambuf_iterator<char>(f)), {});
        write(destination, bytes);
      }
      member(c, "artifacts")
          .array()
          .push_back(Json::object({{"file", text(file.string())},
                                   {"content", text(hash)},
                                   {"retained", text(destination.string())}}));
      save(cmd, r);
      return c;
    }
    if (cmd == "activation") {
      if (std::holds_alternative<std::nullptr_t>(get(c, "checkpoint").value()))
        refuse("checkpoint before activation record");
      if (str(r, "source_commit") != str(c, "checkpoint"))
        refuse("activation source mismatch");
      for (const auto &k : {"qualification", "effective_program", "request", "mode"})
        if (str(r, k).empty())
          refuse("activation evidence missing");
      // Evidence is observed by caller, not inferred from commit or self-rated.
      set(c, "activation", r);
      save("activation.observed", r);
      return c;
    }
    if (cmd == "release" || cmd == "retire") {
      clean(i);
      if (!c.find("checkpoint") ||
          !std::holds_alternative<std::string>(get(c, "checkpoint").value()) ||
          str(c, "checkpoint") != git(checkout(i), {"rev-parse", "HEAD"}))
        refuse("record checkpoint before release");
      if (str(r, "artifacts") != "all relevant artifacts retained")
        refuse("retain declared artifacts first");
      set(s, "status", text("unknown"));
      save(cmd + ".intent", r);
      if (cmd == "retire")
        git(repo(), {"worktree", "remove", checkout(i).string()});
      set(c, "status", text("archived"));
      set(s, "status", text("empty"));
      set(s, "candidate", text(""));
      save(cmd + ".observed", r);
      return c;
    }
    refuse("unknown command");
  }
};
} // namespace
int main(int argc, char **argv) {
  try {
    if (argc != 4)
      refuse("usage: arco-candidate POOL COMMAND REQUEST.json");
    fs::path root = fs::absolute(argv[1]);
    fs::create_directories(root);
    Manager m(root);
    std::cout << dump(m.perform(argv[2], read(argv[3]))) << '\n';
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  } catch (const arconaut::Error &e) {
    std::cerr << "native operation failed; inspect recorded transition: "
              << static_cast<int>(e.code) << '\n';
  }
  return 1;
}
