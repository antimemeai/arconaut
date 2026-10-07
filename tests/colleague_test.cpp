#include "arconaut/colleague.hpp"
#include "arconaut/tools.hpp"
#include <iostream>
#include <stdexcept>
using namespace arconaut;
void check(bool condition) {
  if (!condition)
    throw std::runtime_error{"check failed"};
}
Json request(const char *provider = "claude") {
  auto r = unwrap(parse_json(
      R"({"request_id":"r1","from":"arco","to":"diagnostician","provider":"claude","model":"sonnet","task":"Diagnose the selected code","context":[{"id":"source:1","text":"selected bytes"}],"profile":{"name":"context-only","provenance":"operator/local","timeout_seconds":2,"tools":"none","requests":1}})"));
  for (auto &[key, value] : r.object())
    if (key == "provider")
      value = Json{provider};
  return r;
}
int main() {
  try {
    std::vector<std::string> events;
    int calls = 0;
    auto capture = [&](std::string_view name, std::string_view) {
      events.emplace_back(name);
    };
    const auto transport = [&](const Json &prepared, const ColleagueCapture &) {
      ++calls;
      check(events.back() == "admission");
      check(prepared.find("prompt")->string().find("selected bytes") !=
            std::string::npos);
      return unwrap(parse_json(
          R"({"exit_code":0,"output":"{\"type\":\"result\",\"subtype\":\"success\",\"is_error\":false,\"result\":\"diagnosis\",\"modelUsage\":{\"claude-actual\":{}},\"usage\":{\"input_tokens\":12}}"})"));
    };
    auto reply = call_colleague(request(), capture, transport);
    check(reply.find("status")->string() == "completed" && calls == 1);
    check(reply.find("from")->string() == "diagnostician" &&
          reply.find("to")->string() == "arco");
    check(reply.find("actual_model")->string() == "claude-actual");
    check(reply.find("model_usage")->find("claude-actual") != nullptr);
    check(reply.find("usage")->find("input_tokens")->number().text == "12");
    auto invalid = request("unconfigured");
    reply = call_colleague(invalid, capture, transport);
    check(reply.find("status")->string() == "refused" && calls == 1);
    auto bad_profile = request();
    for (auto &[key, value] : bad_profile.object())
      if (key == "profile")
        for (auto &[k, v] : value.object())
          if (k == "tools")
            v = Json{"shell"};
    reply = call_colleague(bad_profile, capture, transport);
    check(reply.find("status")->string() == "refused" && calls == 1);
    try {
      (void)call_colleague(
          request(),
          [](std::string_view, std::string_view) {
            throw Error{ErrorCode::audit_unavailable};
          },
          transport);
      check(false);
    } catch (const Error &) {
      check(calls == 1);
    }
    reply = call_colleague(request(), capture,
                           [&](const Json &, const ColleagueCapture &c) -> Json {
                             ++calls;
                             c("raw", "partial");
                             throw Error{ErrorCode::external_unknown};
                           });
    check(reply.find("status")->string() == "unknown" && calls == 2);
    check(events[events.size() - 2] == "raw");
    reply =
        call_colleague(request(), capture, [](const Json &, const ColleagueCapture &) {
          return unwrap(parse_json(R"({"exit_code":0,"output":"partial prose"})"));
        });
    check(reply.find("status")->string() == "unknown");
    reply =
        call_colleague(request(), capture, [](const Json &, const ColleagueCapture &) {
          return unwrap(parse_json(R"({"exit_code":7,"output":"refused"})"));
        });
    check(reply.find("status")->string() == "failed" &&
          reply.find("remote_disposition")->string() == "unknown");
    reply = call_colleague(
        request("openai"), capture, [](const Json &p, const ColleagueCapture &) {
          check(p.find("upstream")->find("tools")->array().empty());
          return unwrap(parse_json(
              R"({"status":"completed","model":"gpt-actual","output":[{"type":"message","role":"assistant","content":[{"type":"output_text","text":"native diagnosis"}]}],"usage":{"output_tokens":3}})"));
        });
    check(reply.find("status")->string() == "completed" &&
          reply.find("actual_model")->string() == "gpt-actual");
    reply = call_colleague(Json{}, capture, transport);
    check(reply.find("status")->string() == "refused" && calls == 2);
    reply = call_colleague(request(), capture, [](const Json &, const ColleagueCapture &) {
      return unwrap(parse_json(
          R"({"exit_code":0,"output":"{\"type\":\"result\",\"is_error\":false,\"subtype\":\"success\",\"result\":7}"})"));
    });
    check(reply.find("status")->string() == "unknown");
    reply = call_colleague(request(), capture, [](const Json &, const ColleagueCapture &) {
      return unwrap(parse_json(
          R"({"exit_code":1,"output":"{\"is_error\":true,\"result\":\"OAuth expired\",\"api_error_status\":401}"})"));
    });
    check(reply.find("status")->string() == "failed" &&
          reply.find("reported_error")->find("api_error_status")->number().text ==
              "401");
    reply = call_colleague(
        request("openai"), capture, [](const Json &, const ColleagueCapture &) {
          return unwrap(parse_json(
              R"({"status":"completed","output":[{"type":"message","role":"assistant","content":[{"type":"refusal","refusal":"No"}]}]})"));
        });
    check(reply.find("status")->string() == "refused" &&
          reply.find("remote_disposition")->string() == "completed");
    reply = call_colleague(
        request("openai"), capture, [](const Json &, const ColleagueCapture &) {
          return unwrap(parse_json(
              R"({"status":"completed","output":[{"type":"function_call","name":"shell","arguments":"{}"}]})"));
        });
    check(reply.find("status")->string() == "unknown");
    // Direct installed exec seam oracle: partial output + timeout is unknown, no
    // replay.
    auto p = prepare_colleague(request());
    for (auto &[key, value] : p.object())
      if (key == "exec")
        value = unwrap(parse_json(
            R"({"argv":["/bin/sh","-c","printf partial; sleep 4"],"timeout_seconds":1})"));
    reply = call_colleague(request(), capture,
                           [&](const Json &, const ColleagueCapture &c) {
                             return native_colleague_transport(p, c);
                           });
    check(reply.find("status")->string() == "unknown");
    std::cout << "colleague contract checks passed\n";
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
    return 1;
  }
}
