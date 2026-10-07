#include "../src/native_process.hpp"
#include "arconaut/coding.hpp"
#include <iostream>
#include <unistd.h>
using namespace arconaut;
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
void require(bool value) {
  if (!value)
    throw Error{ErrorCode::corrupt};
}
class Provider final : public CodingProvider {
public:
  int calls = 0;
  Json::Array input;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &capture) override {
    input = field(request, "input").array();
    ++calls;
    auto response = unwrap(parse_json(
        calls == 1
            ? R"({"output":[{"type":"function_call","call_id":"ran","name":"exec","arguments":"{}"},{"type":"function_call","call_id":"not-run","name":"exec","arguments":"{}"}]})"
            : R"({"output":[]})"));
    capture(unwrap(dump_json(response)));
    return response;
  }
};
int main() {
  char name[] = "/tmp/arco-repair-XXXXXX";
  auto path = mkdtemp(name);
  if (!path)
    return 2;
  try {
    JournalHeader h{id<EnvironmentId>(1),
                    id<AuditStreamId>(2),
                    3,
                    {4 * 1024 * 1024, 16 * 1024 * 1024},
                    std::nullopt};
    auto root =
        unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                         unwrap(NativeJournalDirectory::open(path))),
                                     "audit", h, {64 * 1024 * 1024, 10000}));
    AuditLog log{*root};
    ContextStore context{log};
    Provider provider;
    CodingEngine engine{log, context, provider, "test"};
    const auto marker = std::string{path} + "/effect";
    context.append(
        unwrap(
            parse_json(
                R"([{"type":"function_call","call_id":"known","name":"read_file","arguments":"{}"},{"type":"function_call_output","call_id":"known","output":"known-result"}])"))
            .array(),
        "test.known");
    const auto command = "printf x >> " + marker;
    const auto args = unwrap(dump_json(Json::object({{"command", Json{command}}})));
    try {
      engine.turn({"failed work", "arco.request(); arco.call('exec', [==[" + args +
                                      "]==]); error('custom failure before append')"});
      require(false);
    } catch (const Error &e) {
      require(e.code == ErrorCode::external_unknown);
    }
    require(provider.calls == 1);
    const auto originals = context.originals();
    try {
      engine.turn({"", "arco.request()"});
      require(false);
    } catch (const Error &e) {
      require(e.code == ErrorCode::conflict);
    }
    require(provider.calls == 1);
    {
      detail::Child child;
      child.start({"/bin/sleep", "30"});
      require(detail::owned_children.load() == 1);
      engine.turn(
          {"",
           R"(local r=arco.call('context_repair',{base=arco.context().base}); assert(r.error=='conflict'))"});
      require(provider.calls == 1);
    }
    require(detail::owned_children.load() == 0);
    engine.turn(
        {"",
         R"(local r=arco.call('context_repair',{base=arco.context().base}); assert(tonumber(r.repaired[1])==2); arco.request())"});
    require(provider.calls == 2);
    std::size_t outputs = 0;
    for (const auto &item : provider.input) {
      if (item.find("type") && field(item, "type") == Json{"function_call_output"}) {
        if (string_field(item, "call_id") == "known") {
          require(string_field(item, "output") == "known-result");
          continue;
        }
        auto output = unwrap(parse_json(string_field(item, "output")));
        require(field(output, "effect_outcome") == Json{"unknown"});
        require(field(output, "replayed") == Json{false});
        ++outputs;
      }
    }
    require(outputs == 2);
    const auto repaired_originals = context.originals();
    for (const auto &old : originals.array()) {
      bool found = false;
      for (const auto &now : repaired_originals.array())
        if (now == old)
          found = true;
      require(found);
    }
    engine.turn(
        {"",
         R"(local r=arco.call('context_repair',{base=arco.context().base}); assert(tonumber(r.repaired[1])==0))"});
    engine.turn(
        {"",
         R"(local r=arco.call('context_repair',{base='stale'}); assert(r.error=='conflict'))"});
    bool paused = true;
    engine.cancelled = [&] { return paused; };
    try {
      engine.turn({"", "arco.call('context_repair',{}); arco.request()"});
      require(false);
    } catch (const Error &e) {
      require(e.code == ErrorCode::interrupted);
    }
    require(provider.calls == 2);
    paused = false;
    engine.turn(
        {"",
         R"(arco.append({{type='function_call',call_id='active',name='exec',arguments='{}'}}); local r=arco.call('context_repair',{base=arco.context().base}); assert(r.error=='conflict'))"});
    // A subsequent explicit workflow may close it; never the workflow that created it.
    engine.turn(
        {"",
         R"(local r=arco.call('context_repair',{base=arco.context().base}); assert(tonumber(r.repaired[1])==1))"});
    engine.turn({"", "local r=arco.call('read_file',{path='" + marker +
                         "'}); assert(r.content=='x')"});
    ContextStore reloaded{log};
    require(reloaded.items() == context.items());
    require(reloaded.originals() == context.originals());
    engine.turn(
        {"",
         R"(arco.append({{type='function_call',call_id='duplicate',name='exec',arguments='{}'},{type='function_call',call_id='duplicate',name='exec',arguments='{}'}}))"});
    const auto malformed = context.items();
    engine.turn(
        {"",
         R"(local r=arco.call('context_repair',{base=arco.context().base}); assert(r.error=='conflict'))"});
    require(context.items() == malformed);
    std::cout
        << "workflow repair: unknowns, no replay, originals, request, pause, CAS, "
           "active-call guard, malformed rejection and retained reload passed\n";
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << ':' << e.detail << '\n';
    return 1;
  }
  std::filesystem::remove_all(path);
}
