#include "arconaut/coding.hpp"
#include <iostream>
#include <unistd.h>
using namespace arconaut;
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
class ManagedToolProvider final : public CodingProvider {
public:
  Json proposal;
  Json::Array last_input;
  int calls = 0;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &capture) override {
    last_input = field(request, "input").array();
    ++calls;
    Json::Array output;
    if (calls == 1)
      output.push_back(Json::object(
          {{"type", Json{"function_call"}},
           {"call_id", Json{"managed-real"}},
           {"name", Json{"context_manage"}},
           {"arguments",
            Json{unwrap(dump_json(Json::object({{"proposal", proposal}})))}}}));
    if (calls == 2)
      output.push_back(Json::object({{"type", Json{"function_call"}},
                                     {"call_id", Json{"managed-second"}},
                                     {"name", Json{"context_stats"}},
                                     {"arguments", Json{"{}"}}}));
    auto response = Json::object({{"output", Json{std::move(output)}}});
    capture(unwrap(dump_json(response)));
    return response;
  }
};
class Scripted final : public CodingProvider {
public:
  std::string path;
  int calls = 0;
  bool interrupt = false;
  std::function<void()> check_preview;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &capture) override {
    ++calls;
    if (interrupt) {
      capture("data: "
              "{\"type\":\"response.output_text.delta\",\"item_id\":\"partial\","
              "\"delta\":\"PARTIAL\"}\n\n");
      capture("data: incomplete\n");
      throw Error{ErrorCode::interrupted};
    }
    if (string_field(field(request, "reasoning"), "effort") != "medium")
      throw Error{ErrorCode::corrupt};
    const auto &input = field(request, "input").array();
    Json response;
    if (calls == 1) {
      response = Json::object(
          {{"output",
            Json{Json::Array{
                Json::object({{"type", Json{"reasoning"}},
                              {"encrypted_content", Json{"opaque=="}}}),
                Json::object(
                    {{"type", Json{"function_call"}},
                     {"call_id", Json{"write-1"}},
                     {"name", Json{"write_file"}},
                     {"arguments",
                      Json{unwrap(dump_json(Json::object(
                          {{"path", Json{path}},
                           {"content", Json{"int answer = 42;\n"}}})))}}})}}}});
    } else {
      if (input.size() != 4 ||
          string_field(input[1], "encrypted_content") != "opaque==" ||
          string_field(input[3], "call_id") != "write-1" ||
          string_field(input[3], "type") != "function_call_output")
        throw Error{ErrorCode::corrupt};
      response = Json::object(
          {{"output", Json{Json::Array{Json::object(
                          {{"type", Json{"message"}},
                           {"id", Json{"m1"}},
                           {"role", Json{"assistant"}},
                           {"content", Json{Json::Array{Json::object(
                                           {{"type", Json{"output_text"}},
                                            {"text", Json{"done"}}})}}}})}}}});
    }
    if (calls == 2) {
      capture("data: "
              "{\"type\":\"response.output_text.delta\",\"item_id\":\"m1\",\"delta\":"
              "\"do\"}\n\n");
      if (check_preview)
        check_preview();
    }
    capture(unwrap(dump_json(response)));
    return response;
  }
};
class StoppedTool final : public CodingProvider {
public:
  int calls = 0;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &capture) override {
    ++calls;
    Json item;
    if (calls == 1)
      item = Json::object(
          {{"type", Json{"function_call"}},
           {"call_id", Json{"stop-tool"}},
           {"name", Json{"exec"}},
           {"arguments",
            Json{"{\"command\":\"printf BEFORE_; printf STOP; sleep 30\"}"}}});
    else {
      const auto &input = field(request, "input").array();
      std::size_t matched = 0;
      for (const auto &old : input) {
        const auto *type = old.find("type");
        if (type && type->string() == "function_call_output" &&
            string_field(old, "call_id") == "stop-tool") {
          if (string_field(old, "output").find("turn_interrupted") == std::string::npos)
            throw Error{ErrorCode::corrupt};
          ++matched;
        }
      }
      if (matched != 1)
        throw Error{ErrorCode::corrupt};
      item = Json::object(
          {{"type", Json{"message"}},
           {"content",
            Json{Json::Array{Json::object(
                {{"type", Json{"output_text"}}, {"text", Json{"CONTINUED"}}})}}}});
    }
    const auto result = Json::object({{"output", Json{Json::Array{std::move(item)}}}});
    capture(unwrap(dump_json(result)));
    return result;
  }
};
class TimeoutRecoveryProvider final : public CodingProvider {
public:
  std::string marker;
  int calls = 0;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &capture) override {
    ++calls;
    Json::Array output;
    if (calls == 1) {
      const auto args =
          Json::object({{"command", Json{"printf X >> '" + marker +
                                         "'; printf PARTIAL; exec sleep 10"}},
                        {"timeout_seconds", Json{JsonNumber{"1"}}}});
      output.push_back(Json::object({{"type", Json{"function_call"}},
                                     {"call_id", Json{"deadline-first"}},
                                     {"name", Json{"exec"}},
                                     {"arguments", Json{unwrap(dump_json(args))}}}));
    } else if (calls == 2) {
      const auto &last = field(request, "input").array().back();
      const auto result = unwrap(parse_json(string_field(last, "output")));
      if (string_field(last, "call_id") != "deadline-first" ||
          !std::get<bool>(field(result, "timed_out").value()) ||
          string_field(result, "output_ref").empty())
        throw Error{ErrorCode::corrupt};
      // A new approach, rather than automatically replaying the prior effect.
      output.push_back(
          Json::object({{"type", Json{"function_call"}},
                        {"call_id", Json{"deadline-next"}},
                        {"name", Json{"exec"}},
                        {"arguments", Json{R"({"command":"printf RECOVERED"})"}}}));
    } else if (calls == 3) {
      const auto &last = field(request, "input").array().back();
      const auto result = unwrap(parse_json(string_field(last, "output")));
      if (string_field(last, "call_id") != "deadline-next" ||
          string_field(result, "output") != "RECOVERED")
        throw Error{ErrorCode::corrupt};
    } else {
      throw Error{ErrorCode::corrupt};
    }
    auto result = Json::object({{"output", Json{std::move(output)}}});
    capture(unwrap(dump_json(result)));
    return result;
  }
};
class UsageProvider final : public CodingProvider {
public:
  std::size_t bytes = 0;
  bool usage = true;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &) override {
    bytes = unwrap(dump_json(request)).size();
    auto result = Json::object({{"output", Json{Json::Array{}}}});
    if (usage)
      result.object().emplace_back(
          "usage",
          unwrap(parse_json(
              R"({"input_tokens":123,"output_tokens":4,"total_tokens":127,"secret":"DO_NOT_SHOW","output_tokens_details":{"reasoning_tokens":2,"secret":"HIDDEN"}})")));
    return result;
  }
};
Json invoke(CodingEngine &engine, std::string_view name, const Json &args) {
  std::string shown;
  engine.display = [&](std::string_view text) { shown += text; };
  const auto code = "local r = arco.call('" + std::string{name} +
                    "', arco.json.decode([==[" + unwrap(dump_json(args)) +
                    "]==])); arco.display(arco.json.encode(r))";
  engine.turn({"", code});
  engine.display = {};
  return unwrap(parse_json(shown));
}
void retained_output_test(const std::string &path) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader h{id<EnvironmentId>(11),
                        id<AuditStreamId>(12),
                        3,
                        {4 * 1024 * 1024, 16 * 1024 * 1024},
                        std::nullopt};
  const JournalCapacity capacity{64 * 1024 * 1024, 10000};
  std::string reference, timed_reference;
  const auto marker = path + "/count";
  {
    auto root =
        unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                         unwrap(NativeJournalDirectory::open(path))),
                                     "audit", h, capacity));
    AuditLog log{*root};
    ContextStore context{log};
    Scripted provider;
    CodingEngine engine{log, context, provider, "test"};
    auto result =
        invoke(engine, "exec",
               Json::object({{"command", Json{"printf X >> '" + marker +
                                              "'; printf abc; printf def >&2; exit 7"}},
                             {"output_max_bytes", Json{JsonNumber{"2"}}}}));
    if (string_field(result, "output") != "ab" ||
        field(result, "exit_code").number().text != "7")
      throw Error{ErrorCode::corrupt};
    reference = string_field(result, "output_ref");
    const auto timed_result =
        invoke(engine, "exec",
               Json::object({{"command", Json{"printf PARTIAL; sleep 10"}},
                             {"timeout_seconds", Json{JsonNumber{"1"}}},
                             {"output_max_bytes", Json{JsonNumber{"0"}}}}));
    if (!std::get<bool>(field(timed_result, "timed_out").value()) ||
        string_field(timed_result, "error") != "io" ||
        string_field(timed_result, "effect_outcome") != "unknown")
      throw Error{ErrorCode::corrupt};
    for (const auto &fact : root->committed_facts()) {
      const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
      if (!record || record->channel != ApplicationChannel::log)
        continue;
      const auto packet = unwrap(parse_json(
          std::string_view{reinterpret_cast<const char *>(record->payload.data()),
                           record->payload.size()}));
      if (string_field(packet, "label") == "process.output")
        timed_reference = string_field(field(packet, "metadata"), "attempt");
    }
  }
  {
    auto root =
        unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(
                                       unwrap(NativeJournalDirectory::open(path))),
                                   "audit", h, capacity));
    unwrap(root->confirm_recovery());
    recover_coding_session(*root);
    AuditLog log{*root};
    ContextStore context{log};
    Scripted provider;
    CodingEngine engine{log, context, provider, "test"};
    auto result = invoke(engine, "read_process_output",
                         Json::object({{"output_ref", Json{reference}},
                                       {"byte_start", Json{JsonNumber{"2"}}}}));
    if (string_field(result, "output") != "cdef" || read_file(marker) != "X" ||
        field(result, "output_bytes").number().text != "6" || provider.calls != 0)
      throw Error{ErrorCode::corrupt};
    auto partial = invoke(engine, "read_process_output",
                          Json::object({{"output_ref", Json{timed_reference}}}));
    if (string_field(partial, "output") != "PARTIAL")
      throw Error{ErrorCode::corrupt};
    // The explorer must expose the actual unknown disposition and causal links,
    // rather than interpreting a terminal record as successful completion.
    bool unknown = false, linked = false;
    const auto end = log.root().committed_facts().size();
    for (std::size_t cursor = 0; cursor < end; cursor += 64) {
      auto page = log.inspect(
          Json::object({{"cursor", Json{JsonNumber{std::to_string(cursor)}}},
                        {"end", Json{JsonNumber{std::to_string(end)}}},
                        {"count", Json{JsonNumber{"64"}}}}));
      for (const auto &row : field(page, "records").array()) {
        const auto *a = row.find("attempt");
        if (a && a->string() == timed_reference) {
          if (const auto *o = row.find("outcome"); o && o->string() == "unknown")
            unknown = true;
          if (row.find("invocation") && row.find("decision"))
            linked = true;
        }
      }
    }
    if (!unknown || !linked)
      throw Error{ErrorCode::corrupt};
    const std::string old_path = std::getenv("PATH") ? std::getenv("PATH") : "";
    const auto fake_bin = path + "/fake-bin";
    std::filesystem::create_directory(fake_bin);
    const auto argv_file = path + "/bead-argv";
    write_file(fake_bin + "/bd", "#!/bin/sh\nprintf '%s\\n' \"$@\" > '" + argv_file +
                                     "'\nprintf '{\"id\":\"fake-bead\"}'\n");
    std::filesystem::permissions(fake_bin + "/bd", std::filesystem::perms::owner_all);
    if (setenv("PATH", fake_bin.c_str(), 1) != 0)
      throw Error{ErrorCode::io};
    auto delivered =
        invoke(engine, "rageshake",
               Json::object({{"observation", Json{"PRIVATE_SUCCESS_OBSERVATION"}}}));
    if (setenv("PATH", old_path.c_str(), 1) != 0)
      throw Error{ErrorCode::io};
    if (!std::get<bool>(field(delivered, "bead_created").value()) ||
        read_file(argv_file).find("PRIVATE_SUCCESS_OBSERVATION") != std::string::npos ||
        read_file(argv_file).find(string_field(delivered, "complaint")) ==
            std::string::npos)
      throw Error{ErrorCode::corrupt};
    auto oversized = invoke(
        engine, "rageshake",
        Json::object(
            {{"observation", Json{"bounded"}},
             {"references", Json::object({{"blob", Json{std::string(65537, 'x')}}})}}));
    if (string_field(oversized, "error") != "invalid_range")
      throw Error{ErrorCode::corrupt};
    if (setenv("PATH", "/arco-test-no-bd", 1) != 0)
      throw Error{ErrorCode::io};
    auto complaint =
        invoke(engine, "rageshake",
               Json::object({{"observation", Json{"PRIVATE_TEST_OBSERVATION"}},
                             {"references",
                              Json::object({{"attempt", Json{timed_reference}}})}}));
    if (setenv("PATH", old_path.c_str(), 1) != 0)
      throw Error{ErrorCode::io};
    if (!std::get<bool>(field(complaint, "retained_locally").value()) ||
        std::get<bool>(field(complaint, "bead_created").value()))
      throw Error{ErrorCode::corrupt};
    const auto complaint_id = string_field(complaint, "complaint");
    bool captured = false;
    for (const auto &fact : root->committed_facts()) {
      if (const auto *e = std::get_if<ApplicationRecordEvent>(&fact.event.body);
          e && hex_identity(e->identity.bytes()) == complaint_id) {
        for (const auto dep : fact.event.dependencies) {
          const auto raw = unwrap(root->source(dep));
          const std::string value{reinterpret_cast<const char *>(raw.data()),
                                  raw.size()};
          if (value.find("PRIVATE_TEST_OBSERVATION") != std::string::npos &&
              value.find(timed_reference) != std::string::npos)
            captured = true;
        }
      }
    }
    if (!captured || provider.calls != 0)
      throw Error{ErrorCode::corrupt};
    TimeoutRecoveryProvider recovery;
    recovery.marker = marker + "-timeout";
    CodingEngine recovery_engine{log, context, recovery, "test"};
    recovery_engine.turn({"recover timeout", read_file("programs/turn.lua")});
    if (recovery.calls != 3 || read_file(recovery.marker) != "X")
      throw Error{ErrorCode::corrupt};
    recovery_engine.validate_restart();
    auto bad = invoke(engine, "read_process_output",
                      Json::object({{"output_ref", Json{"missing"}}}));
    if (!bad.find("error"))
      throw Error{ErrorCode::corrupt};
  }
}
int main() {
  char name[] = "/tmp/arco-coding-XXXXXX";
  auto path = mkdtemp(name);
  if (!path)
    return 2;
  try {
    retained_output_test(std::string{path} + "/output");
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
    Scripted provider;
    provider.path = std::string{path} + "/source.cpp";
    CodingEngine engine{log, context, provider, "test"};
    std::string displayed;
    engine.display = [&](std::string_view s) { displayed += s; };
    provider.check_preview = [&] {
      if (!displayed.ends_with("do"))
        throw Error{ErrorCode::corrupt};
    };
    engine.turn({"write a source", read_file("programs/turn.lua")});
    if (provider.calls != 2 || read_file(provider.path) != "int answer = 42;\n" ||
        !displayed.ends_with("done"))
      throw Error{ErrorCode::corrupt};
    std::size_t terminal = 0;
    for (const auto &fact : root->committed_facts())
      if (const auto *o = std::get_if<AttemptObservationEvent>(&fact.event.body);
          o && o->phase == AttemptPhase::terminal)
        ++terminal;
    if (terminal != 3)
      throw Error{ErrorCode::corrupt};
    const auto before = context.items();
    engine.turn(
        {"",
         R"(local v=arco.context(); local id=v.entries[1].id; v.entries=arco.array({}); assert(arco.edit(v).accepted); arco.restore(id); assert(arco.context().entries[1].id==id); local original=arco.originals(); assert(#original>=1); local j=arco.json.decode('{"n":9007199254740993,"z":null,"a":[]}'); assert(arco.json.encode(j):find('9007199254740993',1,true)); assert(arco.json.encode(j):find('null',1,true)); assert(arco.json.encode(j):find('[]',1,true)))"});
    if (context.items().size() != 1 || context.items()[0] != before[0])
      throw Error{ErrorCode::corrupt};
    engine.turn(
        {"",
         R"(assert(not pcall(arco.call, "provider", {})); assert(io == nil and os == nil and package == nil and debug == nil); assert(dofile == nil and loadfile == nil); print("audited", 42))"});
    if (!displayed.ends_with("doneaudited\t42\n"))
      throw Error{ErrorCode::corrupt};
    bool print_retained = false;
    for (const auto &fact : root->committed_facts()) {
      if (const auto *a = std::get_if<ApplicationRecordEvent>(&fact.event.body)) {
        const auto packet = unwrap(parse_json(std::string_view{
            reinterpret_cast<const char *>(a->payload.data()), a->payload.size()}));
        if (const auto *label = packet.find("label");
            label && std::holds_alternative<std::string>(label->value()) &&
            label->string() == "display") {
          if (fact.event.dependencies.size() != 1)
            throw Error{ErrorCode::corrupt};
          const auto raw = unwrap(root->source(fact.event.dependencies[0]));
          if (std::string_view{reinterpret_cast<const char *>(raw.data()),
                               raw.size()} == "audited\t42\n")
            print_retained = true;
        }
      }
    }
    if (!print_retained)
      throw Error{ErrorCode::corrupt};
    provider.interrupt = true;
    bool stopped = false;
    try {
      engine.turn({"interruption fixture", read_file("programs/turn.lua")});
    } catch (const Error &e) {
      stopped = e.code == ErrorCode::interrupted;
    }
    if (!stopped || !displayed.ends_with("PARTIAL"))
      throw Error{ErrorCode::corrupt};
    bool unknown = false;
    for (const auto &fact : root->committed_facts())
      if (const auto *o = std::get_if<AttemptObservationEvent>(&fact.event.body);
          o && o->disposition == AttemptDisposition::unknown)
        unknown = true;
    if (!unknown || read_file(provider.path) != "int answer = 42;\n")
      throw Error{ErrorCode::corrupt};
    StoppedTool stopped_provider;
    CodingEngine stopped_engine{log, context, stopped_provider, "test"};
    bool stop_requested = false;
    stopped_engine.cancelled = [&] { return stop_requested; };
    std::string process;
    stopped_engine.process_output = [&](std::string_view s) {
      process += s;
      if (process.starts_with("BEFORE_STOP"))
        stop_requested = true;
    };
    bool tool_stopped = false;
    try {
      stopped_engine.turn({"test stopping", read_file("programs/turn.lua")});
    } catch (const Error &e) {
      tool_stopped = e.code == ErrorCode::interrupted;
    }
    if (!tool_stopped || !process.starts_with("BEFORE_STOP"))
      throw Error{ErrorCode::corrupt};
    auto last_disposition = AttemptDisposition::failure;
    for (const auto &fact : root->committed_facts())
      if (const auto *observation =
              std::get_if<AttemptObservationEvent>(&fact.event.body);
          observation && observation->phase == AttemptPhase::terminal)
        last_disposition = observation->disposition;
    if (last_disposition != AttemptDisposition::unknown)
      throw Error{ErrorCode::corrupt};
    stopped_engine.cancelled = {};
    stopped_engine.display = [&](std::string_view s) { displayed += s; };
    stopped_engine.turn({"continue", read_file("programs/turn.lua")});
    if (stopped_provider.calls != 2 || !displayed.ends_with("CONTINUED"))
      throw Error{ErrorCode::corrupt};

    UsageProvider usage_provider;
    CodingEngine usage_engine{log, context, usage_provider, "test"};
    std::string summary;
    usage_engine.display = [&](std::string_view text) { summary += text; };
    usage_engine.turn({"", "arco.request({model='override'}); "
                           "arco.display(arco.json.encode(arco.stats()))"});
    if (summary.find("DO_NOT_SHOW") != std::string::npos ||
        summary.find("HIDDEN") != std::string::npos ||
        summary.find("123") == std::string::npos ||
        summary.find(std::to_string(usage_provider.bytes)) == std::string::npos)
      throw Error{ErrorCode::corrupt};
    auto stats = invoke(usage_engine, "context_stats", Json::object({}));
    if (field(stats, "last_request_bytes").number().text !=
            std::to_string(usage_provider.bytes) ||
        field(field(stats, "usage"), "input_tokens").number().text != "123")
      throw Error{ErrorCode::corrupt};
    usage_provider.usage = false;
    usage_engine.turn({"", "arco.request()"});
    stats = invoke(usage_engine, "context_stats", Json::object({}));
    if (!std::holds_alternative<std::nullptr_t>(field(stats, "usage").value()))
      throw Error{ErrorCode::corrupt};

    // Managed publication waits for workflow success, including a proposing tool
    // result.
    context.append({Json::object({{"role", Json{"assistant"}},
                                  {"content", Json{"old expendable fact"}}})},
                   "managed-engine");
    const auto old_entry =
        string_field(field(context.view(), "entries").array().back(), "id");
    const auto args = Json::object(
        {{"proposal", Json::object({{"base", Json{context.head()}},
                                    {"mode", Json{"archive"}},
                                    {"ids", Json{Json::Array{Json{old_entry}}}},
                                    {"reason", Json{"engine direct test"}},
                                    {"source", Json{"explicit"}}})}});
    const auto source = "local r=arco.call('context_manage',arco.json.decode(" +
                        unwrap(dump_json(Json{unwrap(dump_json(args))})) +
                        ")); assert(r.staged); "
                        "local v=arco.context(); assert(v.entries[#v.entries].id=='" +
                        old_entry + "')";
    usage_engine.turn({"", source});
    for (const auto &entry : [&] { return field(context.view(), "entries").array(); }())
      if (string_field(entry, "id") == old_entry)
        throw Error{ErrorCode::corrupt};
    context.append(
        {Json::object({{"role", Json{"assistant"}}, {"content", Json{"cancel fact"}}})},
        "managed-engine");
    const auto cancel_entry =
        string_field(field(context.view(), "entries").array().back(), "id");
    const auto failure_source =
        "local v=arco.context(); arco.manage({base=v.base,mode='archive',"
        "ids={v.entries[#v.entries].id},reason='cancel',source='explicit'}); "
        "error('deliberate failure')";
    bool failed_workflow = false;
    try {
      usage_engine.turn({"", failure_source});
    } catch (const Error &) {
      failed_workflow = true;
    }
    if (!failed_workflow ||
        string_field(field(context.view(), "entries").array().back(), "id") !=
            cancel_entry)
      throw Error{ErrorCode::corrupt};
    usage_engine.turn(
        {"", "local v=arco.context(); arco.manage({base=v.base,mode='archive',"
             "ids={v.entries[#v.entries].id},reason='RRC',source='explicit'}); "
             "arco.call('restart',{note='managed settled'})"});
    if (!usage_engine.restart_note())
      throw Error{ErrorCode::corrupt};
    usage_engine.validate_restart();
    for (const auto &entry : [&] { return field(context.view(), "entries").array(); }())
      if (string_field(entry, "id") == cancel_entry)
        throw Error{ErrorCode::corrupt};

    // Drive actual provider response append -> tool call -> result append ->
    // settlement.
    context.append({Json::object({{"role", Json{"assistant"}},
                                  {"content", Json{"provider-selected-away"}}})},
                   "managed-provider");
    const auto provider_discard =
        string_field(field(context.view(), "entries").array().back(), "id");
    Json::Array selected_ids;
    const auto provider_view = context.view();
    for (const auto &candidate : field(provider_view, "entries").array())
      if (string_field(candidate, "id") != provider_discard)
        selected_ids.push_back(field(candidate, "id"));
    ManagedToolProvider managed_provider;
    managed_provider.proposal =
        Json::object({{"mode", Json{"select"}},
                      {"ids", Json{selected_ids}},
                      {"reason", Json{"provider-order direct oracle"}},
                      {"source", Json{"explicit selection"}}});
    CodingEngine managed_engine{log, context, managed_provider, "test"};
    managed_engine.turn({"", read_file("programs/turn.lua")});
    if (managed_provider.calls != 3)
      throw Error{ErrorCode::corrupt};
    const auto managed_view = context.view();
    for (const auto &candidate : field(managed_view, "entries").array())
      if (string_field(candidate, "id") == provider_discard)
        throw Error{ErrorCode::corrupt};
    if (string_field(context.items().back(), "call_id") != "managed-second" ||
        string_field(context.items()[context.items().size() - 3], "output")
                .find("staged") == std::string::npos)
      throw Error{ErrorCode::corrupt};
    managed_engine.validate_restart();
    if (string_field(context.items()[context.items().size() - 3], "call_id") !=
        "managed-real")
      throw Error{ErrorCode::corrupt};
    // Request2 was still in the same workflow: old bytes + proposing exchange remain.
    if (unwrap(dump_json(Json{managed_provider.last_input}))
                .find("provider-selected-away") == std::string::npos ||
        string_field(managed_provider.last_input.back(), "call_id") != "managed-second")
      throw Error{ErrorCode::corrupt};
    const auto expected_compacted_input = context.items();
    managed_engine.turn({"", "arco.request()"});
    if (managed_provider.last_input != expected_compacted_input ||
        unwrap(dump_json(Json{managed_provider.last_input}))
                .find("provider-selected-away") != std::string::npos)
      throw Error{ErrorCode::corrupt};

    bool cancel_restart = false;

    stopped_engine.cancelled = [&] { return cancel_restart; };
    stopped_engine.display = [&](std::string_view) { cancel_restart = true; };
    bool interrupted_restart = false;
    try {
      stopped_engine.turn(
          {"",
           R"(arco.call("restart", {note="must cancel"}); print("stop"); while true do end)"});
    } catch (const Error &e) {
      interrupted_restart = e.code == ErrorCode::interrupted;
    }
    if (!interrupted_restart || stopped_engine.restart_note())
      throw Error{ErrorCode::corrupt};

  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
    std::filesystem::remove_all(path);
    return 1;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
  std::filesystem::remove_all(path);
}
