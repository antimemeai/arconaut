#include "blackbird/beads.hpp"
#include "blackbird/coding.hpp"
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>
using namespace blackbird;
namespace {
void check(bool yes, const char *why) { if (!yes) throw std::runtime_error(why); }
template<class T> T id(unsigned char n) { IdentityBytes b{}; b[0]=std::byte{n}; return unwrap(T::from_bytes(b)); }
Json issue(std::string target = "-task;$()") { return Json::object({{"id",Json{target}}, {"title",Json{"fake"}}, {"status",Json{"open"}}}); }
int fake(int argc, char **argv) {
  std::string dir = std::getenv("BEADS_DIR"); auto project = std::filesystem::path(dir).parent_path();
  std::string mode = read_file(project / "mode");
  Json::Array a; for (int i=1;i<argc;++i) a.emplace_back(std::string{argv[i]});
  auto record = Json::object({{"argv",Json{a}}, {"cwd",Json{std::filesystem::current_path().string()}}, {"dir",Json{dir}}, {"routing_clean",Json{std::getenv("BEADS_DB")==nullptr && std::getenv("DOLT_SERVER_PORT")==nullptr}}});
  auto logfile = project / "calls"; std::string old; if (std::filesystem::exists(logfile)) old=read_file(logfile); write_file(logfile,old+unwrap(dump_json(record))+"\n");
  std::string op; for (const auto &s:a) if (!s.string().starts_with("-")) { op=s.string(); break; }
  if (mode=="cancel_"+op) { write_file(project/"phase","started"); std::cout<<"partial"<<std::flush; sleep(8); return 0; }
  Json result;
  if (op=="version") result=Json::object({{"version",Json{mode=="badversion"?"0.59.0":"0.58.0"}}});
  else if (op=="where") result=Json::object({{"path",Json{mode=="wrongproject"?"/wrong/.beads":dir}}, {"database_path",Json{dir+"/dolt"}}, {"prefix",Json{"fake"}}});
  else if (op=="show") result=Json{Json::Array{issue(argc>1?argv[argc-1]:"bad")}};
  else {
    const bool mutation=op=="create"||op=="update"||op=="close";
    if (mutation) write_file(project/"written","applied");
    if (mode=="nonzero") { std::cout<<"{\"error\":\"version commit failed\"}"; return 9; }
    if (mode=="malformed") { std::cout<<"broken"; return 0; }
    if (mode=="timeout" || mode=="cancel") { std::cout<<"partial"<<std::flush; sleep(8); return 0; }
    if (mode=="overflow") { std::cout<<std::string(100000,'x'); return 0; }
    if (mode=="object") result=issue();
    else if (mode=="error") result=Json::object({{"error",Json{"error despite zero exit"}}});
    else {
      auto item=op=="create"?issue("created"):issue();
      for (const auto &arg:a) if (arg.string()=="--claim") {
        for (auto &[k,v]:item.object()) if (k=="status") v=Json{"in_progress"};
        item.object().emplace_back("assignee",Json{"exact actor;$()"});
      }
      if (op=="close") for (auto &[k,v]:item.object()) if (k=="status") v=Json{"closed"};
      if (mode=="nulid") for (auto &[k,v]:item.object()) if (k=="id") v=Json{std::string{"bad\0id",6}};
      if (mode=="longid") for (auto &[k,v]:item.object()) if (k=="id") v=Json{std::string(9000,'x')};
      result=op=="create"?item:Json{Json::Array{item}};
    }
  }
  std::cerr<<"warning: not JSON\n"; std::cout<<unwrap(dump_json(result)); return 0;
}
class NoProvider : public CodingProvider { Json respond(const Json&, const std::function<void(std::string_view)> &) override { throw std::runtime_error("unexpected provider"); } };
}
int main(int argc,char **argv) {
  if (argc>1 && std::string_view{argv[1]}=="--json") return fake(argc,argv);
  try {
    auto base=std::filesystem::temp_directory_path()/("blackbird-beads-"+std::to_string(getpid()));
    std::filesystem::create_directories(base/".beads"); base=std::filesystem::canonical(base); write_file(base/"mode","ok");
    const auto exe=std::filesystem::canonical(argv[0]).string();
    auto config=Json::object({{"op",Json{"configure"}}, {"project",Json{base.string()}}, {"executable",Json{exe}}, {"actor",Json{"exact actor;$()"}}});
    BeadsAdapter adapter;
    Json::Array measurements;
    adapter.observer=[&](std::string_view label,std::string_view bytes) { (void)label;(void)bytes; };
    auto run=[&](Json args) { auto r=adapter.run(args); measurements.push_back(Json::object({{"op",args},{"result",r}})); return r; };
    check(string_field(run(config),"status")=="configured","configure");
    check(!std::filesystem::exists(base/"calls"),"startup/config spawned child");
    auto ready=Json::object({{"op",Json{"ready"}},{"limit",Json{JsonNumber{"2"}}}});
    auto r=run(ready); check(string_field(r,"status")=="ok","ready failed");
    auto calls=read_file(base/"calls"); check(calls.find("--readonly")!=std::string::npos && calls.find("--limit=2")!=std::string::npos,"ready argv");
    check(calls.find("\"routing_clean\":true")!=std::string::npos && calls.find("\"cwd\":\""+base.string())!=std::string::npos,"env/cwd");
    auto saved=field(r,"data");
    write_file(base/"mode","error"); check(string_field(run(ready),"status")=="rejected","error shape");
    check(field(run(Json::object({{"op",Json{"cached"}}})),"data")==saved,"cache lost");
    write_file(base/"mode","object"); check(string_field(run(ready),"status")=="rejected","object accepted as array");
    write_file(base/"mode","badversion"); write_file(base/"calls",""); run(config);
    check(string_field(run(ready),"status")=="unavailable","unsupported version"); check(read_file(base/"calls").find("where")==std::string::npos,"DB probe after unsupported version");
    write_file(base/"mode","wrongproject"); run(config); check(string_field(run(ready),"status")=="unavailable","wrong project");
    write_file(base/"mode","ok"); run(config);
    auto claim=Json::object({{"op",Json{"claim"}},{"id",Json{"-task;$()"}}});
    check(string_field(run(claim),"status")=="ok","claim");
    calls=read_file(base/"calls"); check(calls.find("\"--claim\",\"--\",\"-task;$()\"")!=std::string::npos,"standalone claim/dash argv");
    check(string_field(run(Json::object({{"op",Json{"claim"}},{"id",Json{"-task;$()"}},{"title",Json{"bad"}}})),"status")=="invalid","composite claim");
    check(string_field(run(Json::object({{"op",Json{"create"}},{"title",Json{"-title;$(touch NEVER)"}},{"body",Json{"body"}}})),"status")=="ok","create");
    auto before=read_file(base/"calls");
    check(string_field(run(Json::object({{"op",Json{"update"}},{"id",Json{"-task;$()"}},{"body",Json{"-"}}})),"status")=="invalid","stdin sentinel accepted");
    check(read_file(base/"calls")==before,"sentinel spawned child");
    auto auditdir=base/"auditdir"; std::filesystem::create_directory(auditdir); std::filesystem::permissions(auditdir,std::filesystem::perms::owner_all);
    JournalHeader h{id<EnvironmentId>(31),id<AuditStreamId>(32),3,{32768,131072},std::nullopt};
    auto root=unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(NativeJournalDirectory::open(auditdir.string()))),"audit",h,{64*1024*1024,10000}));
    AuditLog log{*root}; ContextStore context{log}; NoProvider provider; CodingEngine engine{log,context,provider,"test"};
    auto invoke=[&](const Json &args) { auto encoded=unwrap(dump_json(args)); engine.turn({"", "return blackbird.call('beads',blackbird.json.decode([=["+encoded+"]=]))"}); return engine.workflow_result(); };
    invoke(config); write_file(base/"mode","ok"); invoke(ready);
    for (const auto mode:{"nonzero","malformed","overflow","nulid","longid","timeout","cancel"}) {
      write_file(base/"mode",mode); std::filesystem::remove(base/"written");
      if (std::string_view{mode}=="cancel") engine.cancelled=[&] { return std::filesystem::exists(base/"written"); };
      bool interrupted=false; Json result;
      auto began=std::chrono::steady_clock::now();
      Json args=Json::object({{"op",Json{"create"}},{"title",Json{"write test"}}});
      if (std::string_view{mode}=="nulid" || std::string_view{mode}=="longid") args=Json::object({{"op",Json{"update"}},{"id",Json{"-task;$()"}},{"title",Json{"write test"}}});
      try { result=invoke(args); }
      catch (const Error &e) { if (e.code!=ErrorCode::interrupted) { std::cerr<<"mode="<<mode<<" error="<<error_name(e.code)<<" detail="<<e.detail<<"\n"; throw; } interrupted=true; }
      engine.cancelled={};
      check(std::filesystem::exists(base/"written"),"write not simulated");
      check(interrupted || string_field(result,"status")=="unknown","write envelope not unknown");
      bool found=false;
      for (auto i=root->committed_facts().rbegin();i!=root->committed_facts().rend();++i) {
        if (auto *event=std::get_if<AttemptObservationEvent>(&i->event.body); event && event->phase==AttemptPhase::terminal) { check(event->disposition==AttemptDisposition::unknown,"retained write incorrectly settled");found=true;break; }
      }
      check(found,"missing terminal fact");
      auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-began).count();
      measurements.push_back(Json::object({{"elapsed_us",Json{JsonNumber{std::to_string(elapsed)}}},{"fake_mutation",Json{mode}},{"result",result},{"retained_unknown",Json{true}}}));
    }
    for (const auto phase:{"version","where","show"}) {
      write_file(base/"mode","ok"); invoke(config); std::filesystem::remove(base/"phase");
      if (std::string_view{phase}=="show") invoke(ready);
      write_file(base/"mode",std::string{"cancel_"}+phase);
      engine.cancelled=[&] { return std::filesystem::exists(base/"phase"); };
      bool interrupted=false;
      try { invoke(std::string_view{phase}=="show" ? claim : ready); }
      catch (const Error &e) { check(e.code==ErrorCode::interrupted,"binding cancellation error"); interrupted=true; }
      engine.cancelled={}; check(interrupted,"binding/preflight cancellation swallowed");
    }
    write_file(base/"mode","ok"); adapter.run(config); adapter.run(ready);
    adapter.observer=[](std::string_view,std::string_view) { throw std::bad_alloc{}; };
    bool allocation=false;
    try { adapter.run(Json::object({{"op",Json{"create"}},{"title",Json{"allocation test"}}})); }
    catch (const std::bad_alloc &) { allocation=true; }
    check(allocation && adapter.mutation_may_have_started(),"postspawn allocation lost uncertainty marker");
    auto path=argc>1?argv[1]:"/tmp/beads-measurements.json"; write_file(path,unwrap(dump_json(Json{measurements}))+"\n");
    std::filesystem::remove_all(base); std::cout<<"beads direct oracles passed\n";
  } catch (const std::exception &e) { std::cerr<<e.what()<<"\n";return 1; }
    catch (const Error &e) { std::cerr<<error_name(e.code)<<" "<<e.detail<<"\n";return 1; }
}
