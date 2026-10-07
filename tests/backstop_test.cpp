#include "arconaut/backstop.hpp"
#include "arconaut/process_lifetime.hpp"
#include "../src/native_process.hpp"
#include <iostream>
#include <unistd.h>
using namespace arconaut;
void require(bool value) { if (!value) throw Error{ErrorCode::conflict}; }
template <class T> T id(unsigned char n) {
  IdentityBytes b{}; b[0] = std::byte{n}; return unwrap(T::from_bytes(b));
}
class Failed final : public CodingProvider {
public:
  Json respond(const Json &, const std::function<void(std::string_view)> &capture) override {
    capture("incomplete remote response: outcome unknown");
    throw Error{ErrorCode::external_unknown};
  }
};
class Assessor final : public CodingProvider {
public:
  std::string artifact, dirty;
  bool no_progress = false, cancel = false;
  std::string action_override, oracle_override;
  std::function<void()> on_request;
  int calls = 0;
  Json respond(const Json &request, const std::function<void(std::string_view)> &capture) override {
    ++calls;
    require(field(request, "tools").array().empty());
    const auto encoded = unwrap(dump_json(field(request, "input")));
    require(encoded.find("outcome_unknown") != std::string::npos);
    require(encoded.find("replay_authority") != std::string::npos);
    require(encoded.find("incomplete remote response") == std::string::npos);
    if (on_request) on_request();
    if (cancel) throw Error{ErrorCode::interrupted};
    std::string action = no_progress ? "error('same failure, no progress')" :
        "local r=arco.call('read_file',{path='" + dirty + "'}); "
        "assert(r.content=='dirty work preserved'); "
        "local w=arco.call('write_file',{path='" + artifact + "',content='reconciled useful work'}); assert(w.written)";
    if (!action_override.empty()) action = action_override;
    auto choice = Json::object({{"retained", Json{"original audit and dirty source"}},
        {"uncertain", Json{"provider remote completion unknown"}},
        {"change", Json{"inspect file, do not repeat provider"}},
        {"next", Json{"deliver reconciled artifact"}}, {"action", Json{action}},
        {"oracle", Json::object({{"path", Json{artifact}}, {"content", Json{"reconciled useful work"}}})}});
    Json response = Json::object({{"output", Json{Json::Array{Json::object({
        {"type", Json{"message"}}, {"role", Json{"assistant"}}, {"id", Json{"assessment"}},
        {"content", Json{Json::Array{Json::object({{"type", Json{"output_text"}},
              {"text", Json{unwrap(dump_json(choice))}}})}}}})}}}});
    capture(unwrap(dump_json(response))); return response;
  }
};
struct Fixture {
  std::filesystem::path path, work;
  std::unique_ptr<RetainedState> root;
  std::unique_ptr<AuditLog> log;
  std::unique_ptr<ContextStore> context;
  Failed provider;
  std::unique_ptr<CodingEngine> engine;
  Fixture() {
    char name[] = "/tmp/arco-backstop-XXXXXX";
    const auto *made = mkdtemp(name); if (!made) throw Error{ErrorCode::io};
    path = made;
    char work_name[] = "/tmp/arco-backstop-work-XXXXXX";
    const auto *workspace = mkdtemp(work_name); if (!workspace) throw Error{ErrorCode::io};
    work = workspace;
    root = unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
        unwrap(NativeJournalDirectory::open(path.string()))), "audit",
        {id<EnvironmentId>(1), id<AuditStreamId>(2), 7, {1024*1024, 4*1024*1024}, std::nullopt},
        {64*1024*1024, 20000}));
    log = std::make_unique<AuditLog>(*root);
    context = std::make_unique<ContextStore>(*log);
    engine = std::make_unique<CodingEngine>(*log, *context, provider, "test");
  }
  void fail() {
    const auto program = "local w=arco.call('write_file',{path='" + (path/"dirty").string() +
        "',content='dirty work preserved'}); assert(w.written); arco.request({retry_policy={max_attempts=1}})";
    try { engine->turn({"Reconcile useful source work", program}); throw std::runtime_error("expected failure"); }
    catch (const Error &e) { require(e.code == ErrorCode::external_unknown); }
  }
  ~Fixture() { engine.reset(); context.reset(); log.reset(); root.reset(); std::filesystem::remove_all(path); std::filesystem::remove_all(work); }
};
Json mission() { return Json::object({{"mission", Json{"Reconcile file and produce useful artifact"}},
    {"steer", Json{"do not replay unknown remote request"}}}); }
Assessor assessor(const Fixture &f) { Assessor a; a.artifact=(f.work/"result").string(); a.dirty=(f.path/"dirty").string(); return a; }
int main() {
  try {
    {
      Fixture f; f.fail(); auto a=assessor(f);
      const auto old=read_file(f.path/"audit");
      const auto result=run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""});
      require(string_field(result,"phase")=="useful-work-observed" && a.calls==1);
      require(!a.cancelled); // adapter must not retain the recovery's stack captures
      require(read_file(f.path/"audit")==old);
      require(read_file(f.path/"dirty")=="dirty work preserved");
      require(read_file(f.work/"result")=="reconciled useful work");
      require(std::filesystem::file_size(f.path/"backstop"/"audit")>journal_header_size);
      try { (void)run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""}); require(false); }
      catch (const Error &e) { require(e.code==ErrorCode::conflict); }
      require(a.calls==1);
    }
    {
      Fixture f; f.fail(); auto a=assessor(f); a.no_progress=true;
      const auto result=run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""});
      require(a.calls==2 && result.find("bound") && !std::filesystem::exists(f.work/"result"));
    }
    {
      Fixture f; f.fail(); auto a=assessor(f);
      try { (void)run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""},[]{return true;}); require(false); }
      catch (const Error &e) { require(e.code==ErrorCode::interrupted); }
      require(a.calls==0 && !std::filesystem::exists(f.path/"backstop"));
      bool paused=false; a.on_request=[&]{paused=true;}; a.cancel=true;
      const auto result=run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""},[&]{return paused;});
      require(string_field(result,"phase")=="paused" && a.calls==1);
    }
    {
      Fixture f; f.fail(); auto a=assessor(f);
      detail::Child child; child.start({"/bin/sh","-c","sleep 5"});
      require(detail::owned_children.load()==1);
      try { (void)run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""}); require(false); }
      catch (const Error &e) { require(e.code==ErrorCode::conflict); }
      require(a.calls==0 && !std::filesystem::exists(f.path/"backstop"));
    }
    // A pivot cannot change predecessor evidence via direct, symlink or hardlink
    // aliases, even when its declared oracle lies outside the protected tree.
    for (int alias = 0; alias < 3; ++alias) {
      Fixture f; f.fail(); auto a=assessor(f);
      const auto old=read_file(f.path/"audit");
      auto target=f.path/"audit";
      if (alias) {
        target=f.work/"alias";
        if (alias==1) std::filesystem::create_symlink(f.path/"audit",target);
        else std::filesystem::create_hard_link(f.path/"audit",target);
      }
      a.action_override="local w=arco.call('write_file',{path='"+target.string()+
          "',content='destroy evidence'}); assert(w.written)";
      const auto result=run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""});
      require(result.find("bound") && a.calls==2 && read_file(f.path/"audit")==old);
    }
    {
      Fixture f; f.fail(); auto a=assessor(f);
      a.artifact=(f.path/"audit").string();
      const auto old=read_file(f.path/"audit");
      const auto result=run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""});
      require(result.find("bound") && read_file(f.path/"audit")==old);
    }
    {
      Fixture f; f.fail(); auto a=assessor(f);
      a.action_override="arco.request({retry_policy={max_attempts=1}})";
      const auto result=run_backstop(*f.engine,*f.log,f.path,mission(),a,{"test","medium",""});
      require(result.find("bound") && a.calls==2); // no provider during action
    }
    // Leader exit leaves native custody held until group cleanup, not just reap.
    {
      Fixture f; f.fail(); auto a=assessor(f);
      {
        detail::Child child;
        child.start({"/bin/sh","-c","sleep 10 </dev/null >/dev/null 2>&1 & echo $!"});
        child.close_input();
        const auto text=child.collect(detail::Clock::now()+std::chrono::seconds(2),1024);
        const auto descendant=static_cast<pid_t>(std::stoi(text));
        require(::kill(descendant,0)==0 && detail::owned_children.load()==1);
        try { (void)f.engine->claim_backstop(); require(false); }
        catch (const Error &e) { require(e.code==ErrorCode::conflict); }
      }
      require(detail::owned_children.load()==0);
      // Descendant disappearance may be unavailable (e.g. adopted zombie); that
      // conservative observation must also block handoff, never mean success.
      if (!detail::locally_quiescent()) {
        try { (void)f.engine->claim_backstop(); require(false); }
        catch (const Error &e) { require(e.code==ErrorCode::conflict); }
      }
    }
    // Last: arbitrary child containment remains unavailable for this lifetime.
    detail::uncontained_exec.store(true);
    { Fixture f; f.fail(); try { (void)f.engine->claim_backstop(); require(false); }
      catch (const Error &e) { require(e.code==ErrorCode::conflict); } }
    std::cout << "backstop: real failure, unknowns, useful work, duplicate, pause, live child and loop bound passed\n";
  } catch (const Error &e) { std::cerr << error_name(e.code) << '\n'; return 1; }
}
