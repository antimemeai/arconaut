#include "blackbird/coding.hpp"
#include <filesystem>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
Json num(std::size_t n) { return Json{JsonNumber{std::to_string(n)}}; }
void check(bool good, const char *message) {
  if (!good)
    throw std::runtime_error(message);
}
bool same_json(const Json &a, const Json &b) {
  if (a.value().index() != b.value().index())
    return false;
  if (std::holds_alternative<Json::Object>(a.value())) {
    if (a.object().size() != b.object().size())
      return false;
    for (const auto &[key, value] : a.object()) {
      const auto *other = b.find(key);
      if (!other || !same_json(value, *other))
        return false;
    }
    return true;
  }
  if (std::holds_alternative<Json::Array>(a.value())) {
    if (a.array().size() != b.array().size())
      return false;
    for (std::size_t i = 0; i < a.array().size(); ++i)
      if (!same_json(a.array()[i], b.array()[i]))
        return false;
    return true;
  }
  return a == b;
}
class Provider final : public CodingProvider {
public:
  Json arguments, expected;
  unsigned calls = 0;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &) override {
    if (++calls == 1)
      return Json::object(
          {{"output", Json{Json::Array{Json::object(
                          {{"type", Json{"function_call"}},
                           {"call_id", Json{"trace-call"}},
                           {"name", Json{"trajectory_read"}},
                           {"arguments", Json{unwrap(dump_json(arguments))}}})}}}});
    bool found = false;
    for (const auto &item : field(request, "input").array())
      if (const auto *type = item.find("type");
          type && type->string() == "function_call_output") {
        check(same_json(unwrap(parse_json(string_field(item, "output"))), expected),
              "model received a different pinned trajectory");
        found = true;
      }
    check(found, "model tool output absent");
    return unwrap(parse_json(
        R"({"output":[{"type":"message","role":"assistant","id":"m1","content":[{"type":"output_text","text":"done"}]}]})"));
  }
};
int main() {
  char name[] = "/tmp/blackbird-trajectory-XXXXXX";
  const auto *directory = mkdtemp(name);
  if (!directory)
    return 2;
  try {
    const JournalHeader header{id<EnvironmentId>(1),
                               id<AuditStreamId>(2),
                               3,
                               {1024 * 1024, 4 * 1024 * 1024},
                               std::nullopt};
    const JournalCapacity cap{32 * 1024 * 1024, 10000};
    Json pinned, query;
    {
      auto root =
          unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                           NativeJournalDirectory::open(directory))),
                                       "audit", header, cap));
      AuditLog log{*root};
      const auto decision = id<DecisionId>(10);
      const auto invocation = id<InvocationId>(11);
      const auto attempt = id<OperationAttemptId>(12);
      const auto sibling = id<OperationAttemptId>(13);
      const auto sibling_invocation = id<InvocationId>(14);
      const std::string input = "PRIVATE_INPUT_NOT_IN_METADATA";
      const auto view = std::as_bytes(std::span{input.data(), input.size()});
      const ImmutableBytes bytes{view.begin(), view.end()};
      const auto meta = std::as_bytes(std::span{"{\"operation\":\"exec\"}", 20});
      const std::array<RetainedEvent, 5> admitted{
          RetainedEvent{{},
                        DecisionEvent{decision,
                                      id<ParticipantId>(4),
                                      id<ConversationId>(5),
                                      id<WorkflowId>(6),
                                      id<DefinitionGenerationId>(7),
                                      id<ContextRevisionId>(8),
                                      {invocation, sibling_invocation},
                                      {meta.begin(), meta.end()}}},
          RetainedEvent{{},
                        InvocationEvent{invocation, decision,
                                        id<DefinitionGenerationId>(7), bytes}},
          RetainedEvent{{},
                        AttemptAdmissionEvent{attempt, invocation, decision, bytes}},
          RetainedEvent{{},
                        InvocationEvent{sibling_invocation, decision,
                                        id<DefinitionGenerationId>(7), bytes}},
          RetainedEvent{
              {}, AttemptAdmissionEvent{sibling, sibling_invocation, decision, bytes}}};
      unwrap(root->append(root->cursor(), {}, admitted));
      query = Json::object({{"cursor", num(0)},
                            {"end", num(root->fact_count())},
                            {"count", num(64)},
                            {"scan", num(256)},
                            {"attempt", Json{hex_identity(attempt.bytes())}}});
      pinned = log.trajectory(query);
      check(field(pinned, "events").array().size() == 3,
            "filter included sibling attempt");
      check(string_field(field(pinned, "events").array()[0], "operation") == "exec",
            "operation name missing");
      check(string_field(field(pinned, "events").array()[2], "outcome") ==
                "not established by admission",
            "admission appeared successful");
      unwrap(root->submit({{}, AttemptOpenEvent{attempt}}));
      const std::string binary{"A\0Z", 3};
      log.original({"operation.result", binary,
                    Json::object({{"attempt", Json{hex_identity(attempt.bytes())}}})});
      unwrap(
          root->submit({{},
                        AttemptObservationEvent{attempt, AttemptPhase::terminal,
                                                AttemptDisposition::unknown, bytes}}));
      check(log.trajectory(query) == pinned,
            "later settlement leaked into pinned prefix");
      auto current = query;
      for (auto &[key, value] : current.object())
        if (key == "end")
          value = num(root->fact_count());
      const auto events = field(log.trajectory(current), "events").array();
      check(events.size() == 6 && string_field(events.back(), "outcome") == "unknown",
            "unknown settlement or linked capture lost");
      check(unwrap(dump_json(events.back())).find(input) == std::string::npos,
            "original payload copied into metadata");
      for (unsigned i = 0; i < 40; ++i)
        log.record(ApplicationChannel::program, Json::object({{"noise", num(i)}}));
      const auto first = root->fact_count() - 40;
      const auto empty = log.trajectory(
          Json::object({{"cursor", num(first)},
                        {"scan", num(7)},
                        {"attempt", Json{hex_identity(attempt.bytes())}}}));
      check(field(empty, "events").array().empty() &&
                field(empty, "next").number().text == std::to_string(first + 7) &&
                field(empty, "scanned").number().text == "7",
            "empty filter scanned beyond bound");
      const auto small =
          log.trajectory(Json::object({{"cursor", num(first)}, {"count", num(2)}}));
      check(field(small, "events").array().size() == 2 &&
                field(small, "next").number().text == std::to_string(first + 2),
            "page skipped records");
      for (const auto &bad : {R"({"count":0})", R"({"scan":257})", R"({"cursor":-1})",
                              R"({"attempt":"bad"})", R"({"secret":true})"}) {
        bool refused = false;
        try {
          (void)log.trajectory(unwrap(parse_json(bad)));
        } catch (const Error &e) {
          refused = e.code == ErrorCode::invalid_range;
        }
        check(refused, "invalid query accepted");
      }
      const auto large_index = root->fact_count();
      const std::string large =
          "{\"label\":\"large\",\"value\":\"" + std::string(65537, 'X') + "\"}";
      bool allow_read = true;
      auto cold = ImmutableBytes::cold(large.size(), [&]() {
        check(allow_read, "oversized cold payload read by metadata query");
        const auto data = std::as_bytes(std::span{large.data(), large.size()});
        return Result<std::vector<std::byte>>::success({data.begin(), data.end()});
      });
      unwrap(root->submit(
          {{}, ApplicationRecordEvent{log.issue(), ApplicationChannel::log, cold}}));
      allow_read = false;
      const auto large_page =
          log.trajectory(Json::object({{"cursor", num(large_index)}}));
      check(!field(large_page, "events").array().back().find("label"),
            "oversized payload parsed");
      allow_read = true;
      ContextStore context{log};
      Provider provider;
      CodingEngine engine{log, context, provider, "fake"};
      const auto arguments = Json::object({{"query", query}});
      const auto operator_result = engine.operator_call("trajectory_read", arguments);
      check(same_json(operator_result, pinned), "operator trajectory differs");
      engine.turn({"", "return blackbird.call('trajectory_read', blackbird.decode([=[" +
                           unwrap(dump_json(arguments)) + "]=]))"});
      check(same_json(engine.workflow_result(), pinned), "Lua trajectory differs");
      provider.arguments = arguments;
      provider.expected = pinned;
      engine.turn(
          {"read trace",
           R"(for i=1,2 do local r=blackbird.request(); for _,o in ipairs(r.output) do if o.type=="function_call" then blackbird.append({{type="function_call_output",call_id=o.call_id,output=blackbird.encode(blackbird.call(o.name,o.arguments))}}) end end end)"});
      check(provider.calls == 2, "model tool path not exercised");
    }
    {
      auto root =
          unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(unwrap(
                                         NativeJournalDirectory::open(directory))),
                                     "audit", header, cap));
      unwrap(root->confirm_recovery());
      AuditLog log{*root};
      check(log.trajectory(query) == pinned, "pinned trajectory changed on reopen");
    }
    std::filesystem::remove_all(directory);
    return 0;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  std::filesystem::remove_all(directory);
  return 1;
}
