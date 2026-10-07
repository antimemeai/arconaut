#include "blackbird/coding.hpp"
#include <cerrno>
#include <chrono>
#include <iostream>
#include <set>
#include <unistd.h>
using namespace blackbird;
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
void atomic_admission_test(const std::string &path, int mode) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader h{id<EnvironmentId>(31),
                        id<AuditStreamId>(32),
                        3,
                        {8192, mode == 2 ? 16384U : 65536U},
                        std::nullopt};
  auto root =
      unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                       unwrap(NativeJournalDirectory::open(path))),
                                   "audit", h, {64 * 1024 * 1024, 10000}));
  AuditLog log{*root};
  ContextStore context{log};
  UsageProvider provider;
  CodingEngine engine{log, context, provider, "test"};
  engine.turn({"", "assert(rawequal(blackbird, arco)); assert(blackbird.module == "
                   "arco.module); arco.call('context_stats', {})"});
  const auto counts = [&] {
    std::array<std::size_t, 3> result{};
    for (const auto &fact : root->committed_facts()) {
      result[0] += std::holds_alternative<DecisionEvent>(fact.event.body);
      result[1] += std::holds_alternative<InvocationEvent>(fact.event.body);
      result[2] += std::holds_alternative<AttemptAdmissionEvent>(fact.event.body);
    }
    return result;
  };
  std::array<std::size_t, 3> before{};
  const auto marker = path + "/effect";
  const auto args =
      Json::object({{"path", Json{marker}},
                    {"content", Json{std::string(mode == 2 ? 7000 : 2048, 'x')}}});
  engine.status = [&](std::string_view description) {
    if (!description.starts_with("write_file"))
      return;
    before = counts();
    if (mode != 2) {
      const auto usage = root->journal_usage();
      JournalCapacity credit{1, usage.remaining_records() - 5};
      if (mode == 0) {
        std::optional<DecisionEvent> prior;
        for (const auto &fact : root->committed_facts())
          if (const auto *d = std::get_if<DecisionEvent>(&fact.event.body))
            prior = *d;
        if (!prior)
          throw Error{ErrorCode::corrupt};
        const auto input = unwrap(dump_json(args));
        const auto metadata = unwrap(dump_json(Json::object(
            {{"operation", Json{"write_file"}},
             {"input", args},
             {"revision", Json{context.head()}},
             {"generation", Json{hex_identity(prior->definition.bytes())}}})));
        const auto raw = std::as_bytes(std::span{metadata.data(), metadata.size()});
        const auto in = std::as_bytes(std::span{input.data(), input.size()});
        const auto encoded_size = [&](const RetainedEvent &event) {
          return unwrap(encode_retained_event(event, h.limits.max_payload)).size();
        };
        const auto issuer = 56 + journal_frame_header_size +
                            encoded_size({{}, IssuerReservationEvent{1}});
        const auto decision = encoded_size({{},
                                            DecisionEvent{id<DecisionId>(51),
                                                          prior->actor,
                                                          prior->conversation,
                                                          prior->workflow,
                                                          prior->definition,
                                                          prior->context,
                                                          {id<InvocationId>(52)},
                                                          {raw.begin(), raw.end()}}});
        const auto invocation = encoded_size({{},
                                              InvocationEvent{id<InvocationId>(52),
                                                              id<DecisionId>(51),
                                                              prior->definition,
                                                              {in.begin(), in.end()}}});
        // Exactly enough for three issuer reservations and two old transactions.
        const auto room =
            3 * issuer + 2 * (56 + journal_frame_header_size) + decision + invocation;
        if (!usage.remaining_bytes() || *usage.remaining_bytes() <= room)
          throw Error{ErrorCode::corrupt};
        credit = {*usage.remaining_bytes() - room, 1};
      }
      unwrap(root->refresh_settlement(credit));
    }
  };

  bool refused = false;
  try {
    engine.turn({"", "arco.call('write_file', arco.json.decode([==[" +
                         unwrap(dump_json(args)) + "]==]))"});
  } catch (const Error &error) {
    refused = error.code == ErrorCode::capacity;
  }
  const auto after_credit = root->protected_settlement();
  if (!refused || counts() != before || std::filesystem::exists(marker) ||
      provider.bytes != 0 || root->state() != JournalWriterState::live ||
      after_credit) {
    const auto actual = counts();
    std::cerr << "admission refusal mode=" << mode << " refused=" << refused
              << " committed deltas=" << actual[0] - before[0] << ','
              << actual[1] - before[1] << ',' << actual[2] - before[2] << '\n';
    throw Error{ErrorCode::corrupt};
  }
  engine.status = {};
  auto expected = counts();
  std::set<IdentityBytes> prior_ids;
  for (const auto &fact : root->committed_facts()) {
    if (const auto *d = std::get_if<DecisionEvent>(&fact.event.body))
      prior_ids.insert(d->decision.bytes());
    if (const auto *v = std::get_if<InvocationEvent>(&fact.event.body))
      prior_ids.insert(v->invocation.bytes());
    if (const auto *a = std::get_if<AttemptAdmissionEvent>(&fact.event.body))
      prior_ids.insert(a->attempt.bytes());
  }
  (void)invoke(engine, "write_file",
               Json::object({{"path", Json{marker}}, {"content", Json{"accepted"}}}));
  for (auto &value : expected)
    ++value;
  if (counts() != expected || read_file(marker) != "accepted")
    throw Error{ErrorCode::corrupt};
  const DecisionEvent *decision = nullptr;
  const InvocationEvent *invocation = nullptr;
  const AttemptAdmissionEvent *admission = nullptr;
  std::array<std::uint64_t, 3> sequence{};
  for (const auto &fact : root->committed_facts()) {
    if (const auto *d = std::get_if<DecisionEvent>(&fact.event.body)) {
      decision = d;
      sequence[0] = fact.record.sequence;
    }
    if (const auto *v = std::get_if<InvocationEvent>(&fact.event.body)) {
      invocation = v;
      sequence[1] = fact.record.sequence;
    }
    if (const auto *a = std::get_if<AttemptAdmissionEvent>(&fact.event.body)) {
      admission = a;
      sequence[2] = fact.record.sequence;
    }
  }
  if (!decision || !invocation || !admission ||
      invocation->decision != decision->decision ||
      admission->decision != decision->decision ||
      admission->invocation != invocation->invocation ||
      invocation->definition != decision->definition ||
      decision->planned_invocations != std::vector{invocation->invocation} ||
      prior_ids.contains(decision->decision.bytes()) ||
      prior_ids.contains(invocation->invocation.bytes()) ||
      prior_ids.contains(admission->attempt.bytes()) ||
      sequence[1] != sequence[0] + 1 || sequence[2] != sequence[1] + 1)
    throw Error{ErrorCode::corrupt};
  // Read only this small fixture; do not contend with the live journal lease.
  const auto retained = read_file(path + "/audit");
  const auto raw = std::as_bytes(std::span{retained.data(), retained.size()});
  std::array<bool, 3> found{};
  for (std::size_t offset = journal_header_size; offset != raw.size();) {
    const auto frame = unwrap(decode_journal_frame(raw.subspan(offset), h.limits));
    for (std::size_t i = 0; i != sequence.size(); ++i)
      if (frame.kind == FrameKind::semantic && frame.sequence == sequence[i]) {
        found[i] = true;
        if (frame.batch_first != sequence[0])
          throw Error{ErrorCode::corrupt};
      }
    offset += frame.encoded_size;
  }
  for (const auto present : found)
    if (!present)
      throw Error{ErrorCode::corrupt};
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

class FlakyProvider final : public CodingProvider {
public:
  unsigned calls = 0, failures = 1;
  Error fault{ErrorCode::provider_transport, 92};
  Json first_request;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &capture) override {
    ++calls;
    if (calls == 1)
      first_request = request;
    if (request != first_request || request.find("retry_policy"))
      throw Error{ErrorCode::corrupt};
    if (calls <= failures) {
      capture("data: "
              "{\"type\":\"response.output_text.delta\",\"item_id\":\"partial\","
              "\"delta\":\"UNACCEPTED\"}\n\n");
      capture("data: "
              "{\"type\":\"response.output_item.done\",\"item\":{\"type\":\"function_"
              "call\",\"name\":\"write_file\",\"arguments\":\"{}\"}}\n\n");
      throw fault;
    }
    return Json::object(
        {{"output", Json{Json::Array{Json::object({{"type", Json{"function_call"}},
                                                   {"call_id", Json{"accepted"}},
                                                   {"name", Json{"context_stats"}},
                                                   {"arguments", Json{"{}"}}})}}}});
  }
};
void capacity_warning_test(const std::string &path) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  JournalHeader h{id<EnvironmentId>(41),
                  id<AuditStreamId>(42),
                  4,
                  {1024 * 1024, 2 * 1024 * 1024},
                  std::nullopt};
  auto root =
      unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                       unwrap(NativeJournalDirectory::open(path))),
                                   "audit", h, {256 * 1024, 1000}));
  AuditLog log{*root};
  ContextStore context{log};
  FlakyProvider provider;
  provider.failures = 0;
  CodingEngine engine{log, context, provider, "test"};
  const std::string filler(200 * 1024, 'x');
  log.original({"capacity diagnostic filler", filler, Json::object({})});
  unsigned warnings = 0;
  engine.status = [&](std::string_view s) {
    if (s.starts_with("audit headroom:") &&
        s.find("no handoff reserve") != std::string_view::npos)
      ++warnings;
  };
  bool refused = false;
  try {
    engine.turn({"warning", R"(arco.request())"});
  } catch (const Error &e) {
    if (e.code != ErrorCode::capacity)
      throw;
    refused = true;
  }
  if (!refused || warnings != 1 || provider.calls != 0 ||
      !std::get<bool>(field(field(engine.stats(), "audit"), "approaching").value()))
    throw Error{ErrorCode::corrupt};
}
void retry_tests(const std::string &path) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  JournalHeader h{id<EnvironmentId>(31),
                  id<AuditStreamId>(32),
                  3,
                  {4 * 1024 * 1024, 16 * 1024 * 1024},
                  std::nullopt};
  auto root =
      unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                       unwrap(NativeJournalDirectory::open(path))),
                                   "audit", h, {64 * 1024 * 1024, 10000}));
  AuditLog log{*root};
  const char *program =
      R"(local r=arco.request({retry_policy={max_attempts=3,base_ms=0,cap_ms=0}}); for _,v in ipairs(r.output) do local out=arco.call(v.name, {}); arco.append({{type="function_call_output",call_id=v.call_id,output=arco.json.encode(out)}}) end)";
  {
    ContextStore context{log};
    FlakyProvider provider;
    CodingEngine engine{log, context, provider, "test"};
    const auto stats = engine.stats();
    const auto &audit = field(stats, "audit");
    if (field(audit, "max_file_bytes").number().text != "67108864" ||
        field(audit, "max_records").number().text != "10000" ||
        field(audit, "remaining_bytes").number().text.empty())
      throw Error{ErrorCode::corrupt};
  }
  {
    ContextStore context{log};
    FlakyProvider provider;
    CodingEngine engine{log, context, provider, "test"};
    unsigned tool_count = 0;
    engine.operation_completed = [&](std::string_view event) {
      if (event.starts_with("context_stats completed"))
        ++tool_count;
    };
    engine.turn({"retry test", program});
    if (provider.calls != 2 || tool_count != 1 ||
        unwrap(dump_json(Json{context.items()})).find("UNACCEPTED") !=
            std::string::npos)
      throw Error{ErrorCode::corrupt};
    std::set<std::string> attempts;
    std::string group;
    unsigned streams = 0, requests = 0;
    for (const auto &fact : root->committed_facts()) {
      const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
      if (!record || record->channel != ApplicationChannel::log)
        continue;
      const auto original = unwrap(parse_json(
          std::string_view{reinterpret_cast<const char *>(record->payload.data()),
                           record->payload.size()}));
      if (!original.find("label"))
        continue;
      const auto &metadata = field(original, "metadata");
      if (string_field(original, "label") == "provider.request") {
        ++requests;
        attempts.insert(string_field(metadata, "attempt"));
        const auto current = string_field(metadata, "retry_group");
        if (!group.empty() && group != current)
          throw Error{ErrorCode::corrupt};
        group = current;
        if (field(metadata, "ordinal").number().text != std::to_string(requests))
          throw Error{ErrorCode::corrupt};
      }
      if (string_field(original, "label") == "provider.stream")
        ++streams;
    }
    if (attempts.size() != 2 || requests != 2 || streams != 2)
      throw Error{ErrorCode::corrupt};
  }
  for (const auto fault :
       {Error{ErrorCode::provider_transport, 92},
        Error{ErrorCode::external_unknown, 429},
        Error{ErrorCode::external_unknown, 401},
        Error{ErrorCode::provider_transport, 60}, Error{ErrorCode::interrupted},
        Error{ErrorCode::corrupt}, Error{ErrorCode::capacity}, Error{ErrorCode::io, 92},
        Error{ErrorCode::io, ETIMEDOUT}}) {
    ContextStore context{log};
    FlakyProvider provider;
    provider.failures = 10;
    provider.fault = fault;
    CodingEngine engine{log, context, provider, "test"};
    bool failed = false;
    try {
      engine.turn({"outage", program});
    } catch (const Error &e) {
      failed = e.code == fault.code && e.detail == fault.detail;
    }
    const unsigned expected =
        (fault.code == ErrorCode::provider_transport && fault.detail == 92) ||
                (fault.code == ErrorCode::external_unknown && fault.detail == 429)
            ? 3U
            : 1U;
    if (!failed || provider.calls != expected)
      throw Error{ErrorCode::corrupt};
  }
  {
    ContextStore context{log};
    FlakyProvider provider;
    provider.failures = 10;
    CodingEngine engine{log, context, provider, "test"};
    std::vector<std::string> waits;
    engine.status = [&](std::string_view event) {
      if (event.find("partial output not accepted") != std::string_view::npos)
        waits.emplace_back(event);
    };
    bool failed = false;
    try {
      engine.turn(
          {"bounded backoff",
           R"(arco.request({retry_policy={max_attempts=3,base_ms=2,cap_ms=3}}))"});
    } catch (const Error &e) {
      failed = e.code == ErrorCode::provider_transport;
    }
    if (!failed || provider.calls != 3 || waits.size() != 2 ||
        !waits[0].ends_with("in 2ms") || !waits[1].ends_with("in 3ms"))
      throw Error{ErrorCode::corrupt};
  }
  {
    ContextStore context{log};
    FlakyProvider provider;
    CodingEngine engine{log, context, provider, "test"};
    bool failed = false;
    try {
      engine.turn(
          {"invalid policy", R"(arco.request({retry_policy={max_attempts=0}}))"});
    } catch (const Error &e) {
      failed = e.code == ErrorCode::invalid_range;
    }
    if (!failed || provider.calls != 0)
      throw Error{ErrorCode::corrupt};
    engine.cancelled = [] { return true; };
    failed = false;
    try {
      engine.turn({"cancel before dispatch", R"(arco.request())"});
    } catch (const Error &e) {
      failed = e.code == ErrorCode::interrupted;
    }
    if (!failed || provider.calls != 0)
      throw Error{ErrorCode::corrupt};
  }
  {
    ContextStore context{log};
    FlakyProvider provider;
    CodingEngine engine{log, context, provider, "test"};
    bool cancel = false, failed = false;
    engine.status = [&](std::string_view event) {
      if (event.find("retry 2/") != std::string_view::npos)
        cancel = true;
    };
    engine.cancelled = [&] { return cancel; };
    const auto began = std::chrono::steady_clock::now();
    try {
      engine.turn({"cancel wait", R"(arco.request({retry_policy={base_ms=5000}}))"});
    } catch (const Error &e) {
      failed = e.code == ErrorCode::interrupted;
    }
    if (!failed || provider.calls != 1 ||
        std::chrono::steady_clock::now() - began > std::chrono::seconds{1})
      throw Error{ErrorCode::corrupt};
  }
}
class ExhaustingProvider final : public CodingProvider {
public:
  int calls = 0;
  bool returned = false;
  bool records = false;
  Json respond(const Json &,
               const std::function<void(std::string_view)> &capture) override {
    ++calls;
    capture("retained-first-fragment");
    for (int i = 0; i < 500; ++i)
      capture(std::string(records ? 80 : 120000, 'x'));
    returned = true;
    return Json::object({{"output", Json{Json::Array{}}}});
  }
};
void workflow_capacity_test(const std::string &path, bool records) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader h{id<EnvironmentId>(91),
                        id<AuditStreamId>(92),
                        9,
                        {1024 * 1024, 4 * 1024 * 1024},
                        std::nullopt};
  auto root = unwrap(RetainedState::create(
      std::make_unique<NativeJournalDirectory>(
          unwrap(NativeJournalDirectory::open(path))),
      "audit", h, {records ? 8 * 1024 * 1024U : 384 * 1024U, 180}));
  AuditLog log{*root};
  ContextStore context{log};
  ExhaustingProvider provider;
  provider.records = records;
  CodingEngine engine{log, context, provider, "test"};
  context.append({Json::object({{"role", Json{"assistant"}},
                                {"content", Json{std::string(12000, 'a')}}})},
                 "fixture");
  // Actual staged proposal + nested outstanding Lua attempt + streamed refusal.
  bool capacity = false;
  try {
    engine.turn(
        {"", "local v=arco.context(); arco.manage({base=v.base,mode='archive',"
             "ids={v.entries[#v.entries].id},reason='capacity',source='test'}); "
             "pcall(function() arco.call('lua',{code='return arco.request()'}) end); "
             "pcall(function() arco.request() end)"});
  } catch (const Error &e) {
    if (e.code != ErrorCode::capacity)
      throw;
    capacity = true;
  }
  if (!capacity || provider.calls != 1 || provider.returned ||
      root->state() != JournalWriterState::live || root->protected_settlement() ||
      context.pending_proposal() != Json{})
    throw Error{ErrorCode::corrupt};
  std::size_t unknown = 0;
  bool first = false, lost = false, cancelled = false;
  for (const auto &fact : root->committed_facts()) {
    if (const auto *o = std::get_if<AttemptObservationEvent>(&fact.event.body);
        o && o->phase == AttemptPhase::terminal &&
        o->disposition == AttemptDisposition::unknown)
      ++unknown;
    if (const auto *a = std::get_if<ApplicationRecordEvent>(&fact.event.body)) {
      const auto packet = unwrap(parse_json(std::string_view{
          reinterpret_cast<const char *>(a->payload.data()), a->payload.size()}));
      if (const auto *label = packet.find("label")) {
        if (*label == Json{"provider.stream"})
          for (const auto ref : fact.event.dependencies) {
            const auto bytes = unwrap(root->source(ref));
            if (std::string_view{reinterpret_cast<const char *>(bytes.data()),
                                 bytes.size()} == "retained-first-fragment")
              first = true;
          }
        if (*label == Json{"capacity.stop"} &&
            field(field(packet, "metadata"), "unretained_bytes").number().text != "0")
          lost = true;
      }
      if (const auto *outcome = packet.find("outcome"))
        if (const auto *reason = outcome->find("reason");
            reason && *reason == Json{"workflow-cancelled"})
          cancelled = true;
    }
  }
  if (unknown < 1 || !first || !lost || !cancelled)
    throw Error{ErrorCode::corrupt};
}

void workflow_process_capacity_test(const std::string &path) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader h{id<EnvironmentId>(93),
                        id<AuditStreamId>(94),
                        10,
                        {1024 * 1024, 4 * 1024 * 1024},
                        std::nullopt};
  auto root =
      unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                       unwrap(NativeJournalDirectory::open(path))),
                                   "audit", h, {384 * 1024, 1000}));
  AuditLog log{*root};
  ContextStore context{log};
  UsageProvider provider;
  CodingEngine engine{log, context, provider, "test"};
  const auto marker = path + "/effect";
  const auto args = Json::object(
      {{"command", Json{"printf X > '" + marker +
                        "'; printf FIRST; head -c 2000000 /dev/zero; sleep 20"}}});
  const auto began = std::chrono::steady_clock::now();
  bool capacity = false;
  try {
    engine.turn({"", "arco.append({{type='function_call',call_id='capacity-exec',"
                     "name='exec',arguments='{}'}}); "
                     "arco.call('exec',arco.json.decode([==[" +
                         unwrap(dump_json(args)) + "]==]))"});
  } catch (const Error &e) {
    if (e.code != ErrorCode::capacity)
      throw;
    capacity = true;
  }
  bool linked = false, partial = false, unknown = false;
  for (const auto &item : context.items())
    if (const auto *type = item.find("type");
        type && *type == Json{"function_call_output"})
      linked = string_field(item, "call_id") == "capacity-exec" &&
               string_field(item, "output").find("turn_stopped") != std::string::npos;
  for (const auto &fact : root->committed_facts()) {
    if (const auto *o = std::get_if<AttemptObservationEvent>(&fact.event.body);
        o && o->phase == AttemptPhase::terminal &&
        o->disposition == AttemptDisposition::unknown)
      unknown = true;
    if (const auto *a = std::get_if<ApplicationRecordEvent>(&fact.event.body);
        a && a->channel == ApplicationChannel::log) {
      const auto p = unwrap(parse_json(std::string_view{
          reinterpret_cast<const char *>(a->payload.data()), a->payload.size()}));
      if (field(p, "label") == Json{"process.output"})
        for (const auto ref : fact.event.dependencies) {
          const auto raw = unwrap(root->source(ref));
          partial |=
              std::string_view{reinterpret_cast<const char *>(raw.data()), raw.size()}
                  .starts_with("FIRST");
        }
    }
  }
  if (!capacity || !linked || !partial || !unknown || read_file(marker) != "X" ||
      root->state() != JournalWriterState::live ||
      std::chrono::steady_clock::now() - began > std::chrono::seconds{5})
    throw Error{ErrorCode::corrupt};
}

void workflow_mutation_capacity_test(const std::string &path, bool nesting) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader h{id<EnvironmentId>(95),
                        id<AuditStreamId>(96),
                        11,
                        {1024 * 1024, 4 * 1024 * 1024},
                        std::nullopt};
  auto root = unwrap(RetainedState::create(
      std::make_unique<NativeJournalDirectory>(
          unwrap(NativeJournalDirectory::open(path))),
      "audit", h, {nesting ? 8 * 1024 * 1024U : 384 * 1024U, 2000}));
  AuditLog log{*root};
  ContextStore context{log};
  UsageProvider provider;
  CodingEngine engine{log, context, provider, "test"};
  const auto marker = path + "/forbidden";
  const auto old = context.items();
  std::string code =
      nesting
          ? "function recurse(n) if n>0 then return arco.call('lua',{code='return "
            "recurse('..(n-1)..')'}) end end; pcall(function() recurse(18) end); "
          : "pcall(function() "
            "arco.append({{role='assistant',content=string.rep('x',500000)}}) end); ";
  code += "pcall(function() arco.call('write_file',{path='" + marker +
          "',content='BAD'}) end)";
  bool capacity = false;
  try {
    engine.turn({"", code});
  } catch (const Error &e) {
    if (e.code != ErrorCode::capacity)
      throw;
    capacity = true;
  }
  std::size_t admissions = 0, terminals = 0;
  for (const auto &fact : root->committed_facts()) {
    admissions += std::holds_alternative<AttemptAdmissionEvent>(fact.event.body);
    if (const auto *o = std::get_if<AttemptObservationEvent>(&fact.event.body);
        o && o->phase == AttemptPhase::terminal)
      ++terminals;
  }
  if (!capacity || std::filesystem::exists(marker) || context.items() != old ||
      root->state() != JournalWriterState::live || admissions != (nesting ? 16U : 0U) ||
      terminals != admissions)
    throw Error{ErrorCode::corrupt};
}

void workflow_settlement_limits_test(const std::string &path, bool batch) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader h{id<EnvironmentId>(97),
                        id<AuditStreamId>(98),
                        12,
                        {batch ? 3762U : 3800U, batch ? 3850U : 16384U},
                        std::nullopt};
  auto root =
      unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                       unwrap(NativeJournalDirectory::open(path))),
                                   "audit", h, {2 * 1024 * 1024, 2000}));
  AuditLog log{*root};
  ContextStore context{log};
  UsageProvider provider;
  CodingEngine engine{log, context, provider, "test"};
  for (int i = 0; i < 5; ++i)
    context.append({Json::object({{"role", Json{"assistant"}},
                                  {"content", Json{std::string(500, 'x')}}})},
                   "fixture");
  context.append({Json::object({{"role", Json{"assistant"}},
                                {"content", Json{std::string(100, 'y')}}})},
                 "fixture");
  const auto before = context.items();
  const auto marker = path + "/forbidden";
  bool refused = false;
  try {
    engine.turn(
        {"",
         "pcall(function() arco.append({{type='function_call',call_id='c',name='exec',"
         "arguments='{}'}}) end); "
         "pcall(function() arco.call('write_file',{path='" +
             marker + "',content='BAD'}) end)"});
  } catch (const Error &e) {
    if (e.code != ErrorCode::capacity)
      throw;
    refused = true;
  }
  if (!refused || context.items() != before || std::filesystem::exists(marker) ||
      root->state() != JournalWriterState::live) {
    std::cerr << "settlement limits batch=" << batch << " refused=" << refused
              << " changed=" << (context.items() != before)
              << " effect=" << std::filesystem::exists(marker) << '\n';
    throw Error{ErrorCode::corrupt};
  }
}

void workflow_interruption_budget_test(const std::string &path) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader h{id<EnvironmentId>(99),
                        id<AuditStreamId>(100),
                        13,
                        {4 * 1024 * 1024, 8 * 1024 * 1024},
                        std::nullopt};
  auto root =
      unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                       unwrap(NativeJournalDirectory::open(path))),
                                   "audit", h, {8 * 1024 * 1024, 2000}));
  AuditLog log{*root};
  ContextStore context{log};
  UsageProvider provider;
  CodingEngine engine{log, context, provider, "test"};
  bool filled = false;
  engine.cancelled = [&] {
    if (context.items().size() < 2000)
      return false;
    if (!filled) {
      const auto floor = *root->protected_settlement();
      auto remove = *root->journal_usage().remaining_bytes() - floor.max_file_bytes;
      const auto overhead = 56 + journal_frame_header_size;
      // Leave less than one further frame's room, never raise the physical cap.
      while (remove >= overhead) {
        const auto count = std::min<std::uint64_t>(65536, remove - overhead);
        std::vector<std::byte> filler(count);
        const ByteView source{filler};
        (void)unwrap(root->append(root->cursor(), std::span{&source, 1}, {}));
        remove -= count + overhead;
      }
      filled = true;
    }
    return true;
  };
  bool interrupted = false;
  try {
    engine.turn({"", "local items={} for i=1,2000 do items[i]={type='function_call',"
                     "call_id='large-interrupt-'..i,name='exec',arguments='{}'} end "
                     "arco.append(items)"});
  } catch (const Error &e) {
    if (e.code != ErrorCode::interrupted)
      throw;
    interrupted = true;
  }
  std::size_t outputs = 0;
  for (const auto &item : context.items())
    if (const auto *type = item.find("type");
        type && *type == Json{"function_call_output"}) {
      if (string_field(item, "output").find("turn_interrupted") == std::string::npos)
        throw Error{ErrorCode::corrupt};
      ++outputs;
    }
  engine.validate_restart(); // direct protocol validation, not a native restart
  if (!interrupted || !filled || outputs != 2000 ||
      root->state() != JournalWriterState::live)
    throw Error{ErrorCode::corrupt};
}

void file_selector_test(const std::string &path) {
  std::filesystem::create_directory(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader h{
      id<EnvironmentId>(91), id<AuditStreamId>(92), 3, {8192, 65536}, std::nullopt};
  auto root =
      unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                       unwrap(NativeJournalDirectory::open(path))),
                                   "audit", h, {1024 * 1024, 1000}));
  AuditLog log{*root};
  ContextStore context{log};
  Scripted provider;
  CodingEngine engine{log, context, provider, "test"};
  const auto file = path + "/source";
  write_file(file, "first\nsecond\n");
  const auto bad =
      invoke(engine, "read_file",
             unwrap(parse_json("{\"path\":\"" + file +
                               "\",\"range\":{\"mode\":\"lines\"},\"byte_start\":0}")));
  if (string_field(bad, "error") != "invalid_range" ||
      string_field(bad, "message").find("one range object") == std::string::npos)
    throw Error{ErrorCode::corrupt};
  const auto corrected = invoke(engine, "read_file", field(bad, "example"));
  if (string_field(corrected, "content") != "first\nsecond\n")
    throw Error{ErrorCode::corrupt};
  std::size_t failed = 0;
  for (const auto &fact : root->committed_facts())
    if (const auto *o = std::get_if<AttemptObservationEvent>(&fact.event.body))
      if (o->disposition == AttemptDisposition::failure)
        ++failed;
  if (failed != 1)
    throw Error{ErrorCode::corrupt};
}
int main() {
  char name[] = "/tmp/arco-coding-XXXXXX";
  auto path = mkdtemp(name);
  if (!path)
    return 2;
  try {
    file_selector_test(std::string{path} + "/file-selector");
    workflow_interruption_budget_test(std::string{path} + "/interruption-floor");
    workflow_settlement_limits_test(std::string{path} + "/settlement-payload", false);
    workflow_settlement_limits_test(std::string{path} + "/settlement-batch", true);
    workflow_mutation_capacity_test(std::string{path} + "/workflow-mutation", false);
    workflow_mutation_capacity_test(std::string{path} + "/workflow-nesting", true);
    workflow_process_capacity_test(std::string{path} + "/workflow-process");
    workflow_capacity_test(std::string{path} + "/workflow-bytes", false);
    workflow_capacity_test(std::string{path} + "/workflow-records", true);
    for (int mode = 0; mode != 3; ++mode)
      atomic_admission_test(std::string{path} + "/admission-" + std::to_string(mode),
                            mode);
    capacity_warning_test(std::string{path} + "/capacity");
    retry_tests(std::string{path} + "/retries");
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
    std::cerr << error_name(e.code) << ':' << e.detail << '\n';
    std::filesystem::remove_all(path);
    return 1;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
  std::filesystem::remove_all(path);
}
