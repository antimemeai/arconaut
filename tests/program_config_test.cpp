#include "blackbird/coding.hpp"
#include <iostream>
#include <unistd.h>
using namespace blackbird;
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
class Observer final : public CodingProvider {
public:
  std::string model = "test", effort = "medium";
  Json respond(const Json &r,
               const std::function<void(std::string_view)> &capture) override {
    if (string_field(r, "model") != model ||
        string_field(field(r, "reasoning"), "effort") != effort)
      throw Error{ErrorCode::corrupt};
    auto out = Json::object({{"output", Json{Json::Array{}}}});
    capture(unwrap(dump_json(out)));
    return out;
  }
};
int main() {
  char name[] = "/tmp/arco-program-config-XXXXXX";
  auto made = mkdtemp(name);
  if (!made)
    return 1;
  std::filesystem::path path{made};
  try {
    {
      auto root = unwrap(
          RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                    NativeJournalDirectory::open(path.string()))),
                                "audit",
                                {id<EnvironmentId>(1),
                                 id<AuditStreamId>(2),
                                 7,
                                 {1024 * 1024, 4 * 1024 * 1024},
                                 std::nullopt},
                                {64 * 1024 * 1024, 20000}));
      AuditLog log{*root};
      ContextStore context{log};
      Observer provider;
      CodingEngine engine{log, context, provider, "test"};
      engine.turn({"", R"(
        local v=arco.call('program_config',{})
        assert(v.revision and #v.effective.modules==0)
        assert(arco.call('program_config',{proposal={modules=arco.array({{name='calc',source='return {answer=42}'},{name='late',source='return 17'}}),model='configured',effort='low'},base=v.revision}).staged)
        assert(not pcall(arco.module,'calc'))
        assert(arco.call('program_config',{}).effective.model=='')
        arco.request()
      )"});
      provider.model = "configured";
      provider.effort = "low";
      engine.turn({"", R"(
        local m=arco.module('calc'); assert(m.answer==42 and m==arco.module('calc'))
        local v=arco.call('program_config',{})
        assert(not arco.call('program_config',{proposal={modules=arco.array({{name='calc',source='bad syntax !'}}),model='',effort=''},base=v.revision}).staged)
        assert(not arco.call('program_config',{proposal=v.effective,base='stale'}).staged)
        for _,p in ipairs({{modules=arco.array({{name='a',source='return 1'},{name='a',source='return 2'}}),model='',effort=''}, {modules=arco.array({}),model='',effort='nonsense'}, {modules=arco.array({}),model='',effort='',extra=true}}) do
          assert(not arco.call('program_config',{proposal=p}).staged)
        end
        assert(arco.call('program_config',{proposal={modules=arco.array({{name='calc',source='return {answer=99}'}}),model='',effort=''}}).staged)
        assert(arco.module('calc').answer==42 and arco.module('late')==17)
        arco.request({model='configured'})
      )"});
      provider.model = "test";
      provider.effort = "medium";
      bool failed = false;
      try {
        engine.turn({"", R"(
        assert(arco.module('calc').answer==99)
        arco.call('program_config',{proposal={modules=arco.array({}),model='bad',effort='low'}})
        error('candidate workflow fails')
      )"});
      } catch (const Error &) {
        failed = true;
      }
      if (!failed)
        throw Error{ErrorCode::corrupt};
      engine.turn({"", "assert(arco.module('calc').answer==99); arco.request()"});
      bool stop = false;
      engine.cancelled = [&] { return stop; };
      engine.display = [&](std::string_view) { stop = true; };
      try {
        engine.turn(
            {"",
             R"(arco.call('program_config',{proposal={modules=arco.array({}),model='paused',effort='low'}}); print('stop'); while true do end)"});
      } catch (const Error &e) {
        if (e.code != ErrorCode::interrupted)
          throw;
      }
      engine.cancelled = {};
      engine.display = {};
      engine.turn({"", "assert(arco.module('calc').answer==99)"});
      const auto effect_source = "arco.call('write_file',{path='" +
                                 (path / "effect").string() +
                                 "',content='observed'}); error('not rollback')";
      auto proposal = Json::object(
          {{"modules",
            Json{Json::Array{
                Json::object(
                    {{"name", Json{"calc"}}, {"source", Json{"return {answer=99}"}}}),
                Json::object({{"name", Json{"cycle"}},
                              {"source", Json{"return arco.module('cycle')"}}}),
                Json::object(
                    {{"name", Json{"effects"}}, {"source", Json{effect_source}}})}}},
           {"model", Json{""}},
           {"effort", Json{""}}});
      engine.turn({"", "assert(arco.call('program_config',arco.json.decode([==[" +
                           unwrap(dump_json(Json::object({{"proposal", proposal}}))) +
                           "]==])).staged)"});
      if (std::filesystem::exists(path / "effect"))
        throw Error{ErrorCode::corrupt};
      if (std::filesystem::exists(path / "effect"))
        throw Error{ErrorCode::corrupt};
      engine.turn({"", R"lua(
        assert(not pcall(arco.module,'cycle'))
        assert(not pcall(arco.module,'effects'))
        assert(arco.module('calc').answer==99)
      )lua"});
      if (read_file(path / "effect") != "observed")
        throw Error{ErrorCode::corrupt};
    }
    {
      auto root =
          unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(unwrap(
                                         NativeJournalDirectory::open(path.string()))),
                                     "audit",
                                     {id<EnvironmentId>(1),
                                      id<AuditStreamId>(2),
                                      7,
                                      {1024 * 1024, 4 * 1024 * 1024},
                                      std::nullopt},
                                     {64 * 1024 * 1024, 20000}));
      unwrap(root->confirm_recovery());
      recover_coding_session(*root);
      AuditLog log{*root};
      ContextStore context{log};
      Observer provider;
      CodingEngine engine{log, context, provider, "test"};
      engine.turn({"", "assert(arco.module('calc').answer==99); "
                       "assert(arco.call('program_config',{}).effective.model==''); "
                       "arco.request()"});
    }
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << ':' << e.detail << '\n';
    return 1;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  std::filesystem::remove_all(path);
}
