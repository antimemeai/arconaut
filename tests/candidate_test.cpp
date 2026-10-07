#include "../src/native_process.hpp"
#include "blackbird/json.hpp"
#include <exception>
#include <fstream>

#include <iostream>
#include <thread>
using blackbird::Json;
namespace fs = std::filesystem;
namespace {
void require(bool b, const char *s) {
  if (!b)
    throw std::runtime_error(s);
}
std::string run(std::vector<std::string> a, int expected = 0) {
  blackbird::detail::Child c;
  c.start(std::move(a), {}, true);
  c.close_input();
  int code = 0;
  auto out = c.collect(blackbird::detail::Clock::now() + std::chrono::seconds(20),
                       1024 * 1024, &code);
  if ((expected == 0 && code != 0) || (expected != 0 && code == 0))
    throw std::runtime_error("unexpected exit: " + out);
  return out;
}
Json obj(std::initializer_list<std::pair<std::string, Json>> a) {
  return Json::object(a);
}
Json s(const std::string &a) { return Json{a}; }
Json n(std::size_t a) { return Json{blackbird::JsonNumber{std::to_string(a)}}; }
void put(const fs::path &p, const std::string &a) {
  std::ofstream f(p);
  f << a;
}
Json parse(const std::string &v) {
  auto r = blackbird::parse_json(v);
  require(r.has_value(), "parse");
  return r.value();
}
std::string dump(const Json &v) {
  auto r = blackbird::dump_json(v);
  require(r.has_value(), "dump");
  return r.value();
}
} // namespace
int main(int argc, char **argv) {
  fs::path dir;
  try {
    require(argc == 2, "CLI arg");
    auto pattern = (fs::temp_directory_path() / "arco-candidate-test-XXXXXX").string();
    std::vector<char> p(pattern.begin(), pattern.end());
    p.push_back(0);
    auto d = ::mkdtemp(p.data());
    require(d != nullptr, "temp");
    dir = d;
    auto repo = dir / "primary", pool = dir / "pool";
    fs::create_directory(repo);
    run({"git", "init", "-q", repo.string()});
    auto git = [&](std::vector<std::string> a) {
      a.insert(a.begin(), {"git", "-C", repo.string()});
      return run(a);
    };
    git({"config", "user.email", "test@local"});
    git({"config", "user.name", "Test"});
    put(repo / "source", "base\n");
    git({"add", "source"});
    git({"commit", "-qm", "base"});
    auto base = git({"rev-parse", "HEAD"});
    while (!base.empty() && base.back() == '\n')
      base.pop_back();
    put(repo / "unrelated", "keep me\n"); // Primary dirt must never be touched.
    std::size_t seq = 0;
    auto call = [&](const std::string &cmd, const Json &r, int expected = 0) {
      auto req = dir / ("request-" + std::to_string(seq++) + ".json");
      put(req, dump(r));
      auto out = run({argv[1], pool.string(), cmd, req.string()}, expected);
      return expected ? Json{} : parse(out);
    };
    call("init", obj({{"repo", s(repo.string())}, {"cap", n(1)}}));
    auto enqueue = [&](const std::string &name) {
      call("enqueue", obj({{"name", s(name)},
                           {"base", s(base)},
                           {"hypothesis", s("change behavior")},
                           {"discriminator", s("exact source bytes and branch remain")},
                           {"identities", obj({{"program", s("test")}})},
                           {"baseline", s(base)}}));
    };
    enqueue("one");
    enqueue("two");
    auto dispatched = call("dispatch", obj({}));
    auto wt = fs::path(dispatched.find("path")->string());
    auto full = call("dispatch", obj({}));
    require(std::get<bool>(full.find("queued")->value()), "full queues");
    auto evidence =
        obj({{"slot", n(0)},
             {"stopped", s("all checkout programs/builds stopped")},
             {"evidence", s("test owns synchronous processes, none detached")},
             {"artifacts", s("all relevant artifacts retained")}});
    call("release", evidence, 1); // No checkpoint, no switch.
    put(wt / "source", "candidate one\n");
    auto checkpoint = evidence;
    checkpoint.object().emplace_back("files", Json{Json::Array{s("source")}});
    checkpoint.object().emplace_back("message", s("one useful source"));
    call("checkpoint", checkpoint);
    auto activation = call("status", obj({}));
    require(std::holds_alternative<std::nullptr_t>(
                activation.find("candidates")->array()[0].find("activation")->value()),
            "commit not activation");
    put(wt / "result", "retained result\n");
    auto artifact = evidence;
    artifact.object().emplace_back("file", s("result"));
    auto retained = call("artifact", artifact);
    auto retained_path =
        retained.find("artifacts")->array()[0].find("retained")->string();
    call("release", evidence, 1); // Dirty result refuses switch.
    fs::remove(wt / "result");
    // Start managed run in separate caller; global state must remain available,
    // but slot lock blocks settle/release while program is using the checkout.
    auto req = dir / "run.json";
    put(req, dump(obj({{"slot", n(0)},
                       {"seconds", n(10)},
                       {"argv",
                        Json{Json::Array{s("/bin/sh"), s("-c"),
                                         s("touch running; sleep 2; rm running")}}}})));
    std::exception_ptr failure;
    std::jthread runner([&] {
      try {
        run({argv[1], pool.string(), "run", req.string()});
      } catch (...) {
        failure = std::current_exception();
      }
    });
    for (int i = 0; i < 100 && !fs::exists(wt / "running"); ++i)
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    if (!fs::exists(wt / "running")) {
      runner.join();
      throw std::runtime_error("run failed to start");
    }
    call("settle", evidence, 1);
    call("release", evidence, 1);
    call("dispatch", obj({}));
    runner.join();
    if (failure)
      std::rethrow_exception(failure);
    call("release", evidence, 1); // Leader exit is not all-programs-stopped proof.
    call("settle", evidence);
    call("release", evidence);
    // Real failed Git dispatch leaves the reusable old checkout present. Explicit
    // observed abandonment must free the slot without replay or destroying it.
    git({"branch", "candidate/two", base});
    call("dispatch", obj({}), 1);
    auto reconcile = evidence;
    auto old_head = run({"git", "-C", wt.string(), "rev-parse", "HEAD"});
    while (!old_head.empty() && old_head.back() == '\n')
      old_head.pop_back();
    reconcile.object().emplace_back("reconcile", s("retain clean prior checkout"));
    reconcile.object().emplace_back("observed_branch", s("candidate/one"));
    reconcile.object().emplace_back("observed_head", s(old_head));
    call("abandon", reconcile);
    enqueue("three");
    auto second = call("dispatch", obj({}));
    require(second.find("path")->string() == wt.string(), "serial reuse same checkout");
    auto timeout = call(
        "run", obj({{"slot", n(0)},
                    {"seconds", n(1)},
                    {"argv", Json{Json::Array{s("/bin/sh"), s("-c"),
                                              s("echo once >> effect; sleep 5")}}}}));
    require(timeout.find("observation")->find("outcome")->string().find("unknown") !=
                std::string::npos,
            "timeout explicit unknown");
    call("release", evidence, 1);
    call("settle", evidence);
    std::ifstream effect(wt / "effect");
    std::string once((std::istreambuf_iterator<char>(effect)), {});
    require(once == "once\n", "unknown side effect not replayed");
    fs::remove(wt / "effect");
    checkpoint.find("files"); // Same explicit named file checkpoint in new branch.
    put(wt / "source", "candidate two\n");
    call("checkpoint", checkpoint);
    call("retire", evidence);
    require(!fs::exists(wt), "checkout retired");
    require(fs::exists(retained_path), "artifact survives retirement");
    require(git({"show", "candidate/one:source"}) == "candidate one\n",
            "source branch survives");
    require(fs::exists(repo / "unrelated"), "primary unrelated dirt preserved");
    require(git({"show", "HEAD:source"}) == "base\n", "primary unchanged");
    // Failed initial dispatch plus a partial leftover directory has an explicit
    // preservation path, not a bricked slot or automatic retry/delete.
    enqueue("partial");
    git({"branch", "candidate/partial", base});
    call("dispatch", obj({}), 1);
    fs::create_directory(wt);
    put(wt / "unfinished-source", "preserve partial bytes\n");
    auto partial = evidence;
    partial.object().emplace_back("reconcile",
                                  s("quarantine partial directory without execution"));
    call("abandon", partial);
    auto status = call("status", obj({}));
    auto quarantine =
        fs::path(status.find("slots")->array()[0].find("quarantine")->string());
    require(fs::exists(quarantine / "unfinished-source") && !fs::exists(wt),
            "partial source retained outside execution slot");
    // A raised limit is configuration, not a permanent upstream max4.
    auto req5 = dir / "five.json";
    put(req5, dump(obj({{"repo", s(repo.string())}, {"cap", n(5)}, {"limit", n(5)}})));
    run({argv[1], (dir / "pool5").string(), "init", req5.string()});
    // Real two-slot concurrency: second run proceeds while first holds its lease;
    // overflow remains queued, never creates slot2.
    auto serial_pool = pool;
    pool = dir / "concurrent";
    call("init", obj({{"repo", s(repo.string())}, {"cap", n(2)}}));
    enqueue("concurrent-a");
    enqueue("concurrent-b");
    enqueue("concurrent-c");
    auto a = call("dispatch", obj({}));
    call("dispatch", obj({}));
    auto aw = fs::path(a.find("path")->string());
    auto cr = dir / "concurrent-run.json";
    put(cr, dump(obj(
                {{"slot", n(0)},
                 {"seconds", n(10)},
                 {"argv", Json{Json::Array{s("/bin/sh"), s("-c"),
                                           s("touch active; sleep 2; rm active")}}}})));
    std::jthread concurrent([&] {
      try {
        run({argv[1], pool.string(), "run", cr.string()});
      } catch (...) {
        failure = std::current_exception();
      }
    });
    for (int i = 0; i < 100 && !fs::exists(aw / "active"); ++i)
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    require(fs::exists(aw / "active"), "first concurrent run active");
    auto br = call(
        "run",
        obj({{"slot", n(1)},
             {"seconds", n(10)},
             {"argv", Json{Json::Array{s("/bin/echo"), s("other lease proceeds")}}}}));
    require(br.find("observation")->find("exit_code")->number().text == "0",
            "other lease proceeds concurrently");
    require(!fs::exists(pool / "slot-2"), "pool never auto-enlarges");
    auto overflow = call("dispatch", obj({}));
    require(std::get<bool>(overflow.find("queued")->value()),
            "concurrent overflow queued");
    concurrent.join();
    if (failure)
      std::rethrow_exception(failure);
    for (std::size_t k = 0; k < 2; ++k) {
      auto e = evidence;
      for (auto &[key, v] : e.object())
        if (key == "slot")
          v = n(k);
      call("settle", e);
      auto cp = e;
      cp.object().emplace_back("files", Json{Json::Array{}});
      cp.object().emplace_back("message", s("same source, actual run record"));
      call("checkpoint", cp);
      call("retire", e);
    }
    pool = serial_pool;
    // An attacker/misconfiguration must not alias the slot to the primary.
    fs::remove(repo / "unrelated");
    fs::remove_all(pool);
    call("init", obj({{"repo", s(repo.string())}, {"cap", n(1)}}));
    enqueue("alias");
    fs::create_directory_symlink(repo, pool / "slot-0");
    call("dispatch", obj({}), 1);
    require(git({"branch", "--show-current"}).find("candidate/alias") ==
                std::string::npos,
            "primary never switched through alias");
    fs::remove_all(dir);
    std::cout << "candidate direct checks passed\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << " retained at " << dir << '\n';
  } catch (const blackbird::Error &) {
    std::cerr << "native test failure retained at " << dir << '\n';
  }
  return 1;
}
