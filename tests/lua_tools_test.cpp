#include "blackbird/coding.hpp"
#include <iostream>
#include <unistd.h>
using namespace blackbird;
void require(bool b) {
  if (!b)
    throw std::runtime_error("Lua tool oracle failed");
}
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
class Observer final : public CodingProvider {
public:
  bool expected = false;
  std::string delivery_path;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &capture) override {
    bool found = false;
    for (const auto &t : field(request, "tools").array())
      if (string_field(t, "name") == "deliver_note")
        found = true;
    require(found == expected);
    Json::Array output;
    if (!delivery_path.empty()) {
      output.push_back(Json::object(
          {{"type", Json{"function_call"}},
           {"call_id", Json{"live-lua-delivery"}},
           {"name", Json{"deliver_note"}},
           {"arguments",
            Json{unwrap(dump_json(Json::object({{"path", Json{delivery_path}}})))}}}));
      delivery_path.clear();
    }
    auto r = Json::object({{"output", Json{std::move(output)}}});

    capture(unwrap(dump_json(r)));
    return r;
  }
};
int main() {
  char name[] = "/tmp/arco-lua-tools-XXXXXX";
  if (!mkdtemp(name))
    return 1;
  const std::filesystem::path path{name};
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
      const std::string definition =
          R"lua(local d={name='deliver_note',description='Write the operating note',parameters={type='object',properties={path={type='string'}},required=arco.array({'path'}),additionalProperties=false},source="return arco.call('write_file',{path=args.path,content='Use retained outputs; never replay unknown effects.\\n'})"}
)lua";
      {
        CodingEngine engine{log, context, provider, "test"};
        engine.turn({"", definition + R"lua(
        assert(arco.define_tool(d).staged)
        assert(#arco.call('tool_registry',{}).effective==0)
        assert(#arco.call('tool_registry',{}).pending==1)
        arco.request()
      )lua"});
        provider.expected = true;
        provider.delivery_path = (path / "requested-note").string();
        engine.turn({"", read_file("programs/turn.lua")});
        require(read_file(path / "requested-note") ==
                "Use retained outputs; never replay unknown effects.\n");

        engine.turn({"", "arco.request(); local r=arco.call('deliver_note',{path=" +
                             unwrap(dump_json(Json{(path / "note").string()})) +
                             "}); assert(r.written)"});
        require(read_file(path / "note") ==
                "Use retained outputs; never replay unknown effects.\n");
        engine.turn({"", definition + R"lua(
        for _,name in ipairs({'exec','lua','tool_define','provider'}) do
          d.name=name; assert(not arco.define_tool(d).staged)
        end
        d.name='deliver_note'; d.source='not lua syntax !!!'; assert(not arco.define_tool(d).staged)
        d.source='return 99'; d.parameters.properties.path.type='mystery'; assert(not arco.define_tool(d).staged)
        assert(arco.call('deliver_note',{}).error)
        assert(arco.call('deliver_note',{path=42}).error)
        assert(arco.call('deliver_note',{path='no',extra=true}).error)
        assert(#arco.call('tool_registry',{}).effective==1)
      )lua"});
        try {
          engine.turn(
              {"",
               definition + "d.source='return 99'; assert(arco.define_tool(d).staged); "
                            "error('failed workflow')"});
          require(false);
        } catch (const Error &) {
        }
        engine.turn({"", "assert(arco.call('deliver_note',{path=" +
                             unwrap(dump_json(Json{(path / "note2").string()})) +
                             "}).written)"});
        engine.turn({"", definition + R"lua(
        local v=arco.call('tool_registry',{})
        assert(not arco.call('tool_define',{base='stale',definition=d}).staged)
        d.name='roundtrip'; d.source='return args.path'
        assert(arco.define_tool(d).staged)
        d.name='latent_error'; d.source="error('runtime only')"
        assert(arco.define_tool(d).staged)
        d.name='timeout_probe'; d.source="return arco.call('exec',{argv={'/bin/sh','-c','printf original; sleep 2'},timeout_seconds=1})"
        assert(arco.define_tool(d).staged)
      )lua"});
        engine.turn({"", R"lua(
        local text='quote" slash\\ newline\n snowman ☃ nul\0 end'
        assert(arco.call('roundtrip',{path=text})==text)
        assert(arco.call('latent_error',{path='ignored'}).error)
        local r=arco.call('timeout_probe',{path='ignored'})
        assert(r.timed_out and r.effect_outcome=='unknown' and r.output_ref)
        assert(arco.call('read_process_output',{output_ref=r.output_ref}).output=='original')
      )lua"});
        bool paused = false;
        engine.cancelled = [&] { return paused; };
        engine.operation_completed = [&](std::string_view v) {
          if (v.starts_with("tool_define"))
            paused = true;
        };
        try {
          engine.turn(
              {"", definition +
                       "d.source='return 99'; assert(arco.define_tool(d).staged)"});
          require(false);
        } catch (const Error &e) {
          require(e.code == ErrorCode::interrupted);
        }
        engine.cancelled = {};
        engine.operation_completed = {};
        engine.turn({"", "assert(arco.call('deliver_note',{path=" +
                             unwrap(dump_json(Json{(path / "after-pause").string()})) +
                             "}).written)"});
        engine.effect_policy = [](std::string_view n, const Json &) {
          if (n == "write_file")
            throw Error{ErrorCode::conflict};
        };
        engine.turn({"", "assert(arco.call('deliver_note',{path=" +
                             unwrap(dump_json(Json{(path / "blocked").string()})) +
                             "}).error)"});
        require(!std::filesystem::exists(path / "blocked"));
        engine.effect_policy = {};
        engine.turn({"", definition + "d.name='bounded_recursion'; d.source=\"return "
                                      "arco.call('bounded_recursion',args)\"; "
                                      "assert(arco.define_tool(d).staged)"});
        try {
          engine.turn({"", "arco.call('bounded_recursion',{path='none'})"});
          require(false);
        } catch (const Error &e) {
          require(e.code == ErrorCode::capacity);
        }
      }
      {
        CodingEngine reopened{log, context, provider, "test"};
        reopened.turn(
            {"",
             "arco.request(); assert(#arco.call('tool_registry',{}).effective==5)"});
      }
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
      provider.expected = true;
      CodingEngine engine{log, context, provider, "reopened"};
      engine.turn(
          {"", "arco.request(); assert(#arco.call('tool_registry',{}).effective==5); "
               "assert(arco.call('deliver_note',{path=" +
                   unwrap(dump_json(Json{(path / "disk-reopen").string()})) +
                   "}).written)"});
      require(read_file(path / "disk-reopen") ==
              "Use retained outputs; never replay unknown effects.\n");
    }
    std::filesystem::remove_all(path);
    std::cout << "Lua tools oracle passed\n";
    return 0;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << "\n";
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
  }
  std::cerr << "Evidence retained at " << path << "\n";
  return 1;
}
