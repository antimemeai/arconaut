#include "blackbird/colleague.hpp"
#include "blackbird/tools.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace blackbird;
void check(bool b, const char *why) {
  if (!b)
    throw std::runtime_error{why};
}
Json make(std::string provider) {
  return Json::object(
      {{"request_id", Json{"test"}},
       {"from", Json{"main"}},
       {"to", Json{"peer"}},
       {"provider", Json{std::move(provider)}},
       {"model", Json{"selected-model"}},
       {"task", Json{"Review selected code"}},
       {"context",
        Json{Json::Array{Json::object(
            {{"id", Json{"code"}}, {"text", Json{"selected-source-only"}}})}}},
       {"profile", Json::object({{"name", Json{"test"}},
                                 {"provenance", Json{"test"}},
                                 {"timeout_seconds", Json{JsonNumber{"10"}}},
                                 {"tools", Json{"none"}},
                                 {"requests", Json{JsonNumber{"1"}}}})}});
}
int main() {
  char directory[] = "/tmp/blackbird-provider-colleague-XXXXXX";
  const auto *tmp = ::mkdtemp(directory);
  if (!tmp)
    return 1;
  const auto *old = std::getenv("BLACKBIRD_AUTH_HOME");
  const std::string prior = old ? old : "";
  const bool had = old;
  const auto root = std::filesystem::path{tmp} / "auth";
  (void)::setenv("BLACKBIRD_AUTH_HOME", root.c_str(), 1);
  try {
    ProviderAuthConfig config;
    config.directory = root;
    ProviderAuth auth{config};
    const auto *old_key = std::getenv("OPENAI_API_KEY");
    const std::string previous_key = old_key ? old_key : "";
    (void)::setenv("OPENAI_API_KEY", "synthetic-ambient-key", 1);
    const bool ambient_switched =
        prepare_colleague(make("openai")).find("native") != nullptr;
    if (old_key)
      (void)::setenv("OPENAI_API_KEY", previous_key.c_str(), 1);
    else
      (void)::unsetenv("OPENAI_API_KEY");
    check(!ambient_switched, "ambient key changed existing subscription route");
    int calls = 0;
    std::string raw;
    const auto capture = [&](std::string_view, std::string_view bytes) {
      check(bytes.find("private-key") == std::string_view::npos,
            "credential entered capture");
      raw += bytes;
    };
    for (const std::string provider :
         {"kimi", "mimo", "openai", "anthropic", "grok", "openrouter"}) {
      auth.set_key(provider, "test", "private-key");
      config.http = [&](const ProviderHttpRequest &wire) {
        ++calls;
        check(wire.body.find("selected-source-only") != std::string::npos,
              "source missing");
        bool key = false;
        for (const auto &[name, value] : wire.headers)
          if (name == "Authorization" || name == "x-api-key")
            key = value.find("private-key") != std::string::npos;
        check(key, "native credential not injected");
        const auto d = auth.descriptor(provider);
        check(wire.url.starts_with(d.find("base_url")->string()),
              "cross-provider routing");
        const auto protocol = d.find("protocol")->string();
        if (protocol == "responses")
          return ProviderHttpResponse{
              200,
              "data: "
              "{\"type\":\"response.completed\",\"response\":{\"status\":\"completed\","
              "\"model\":\"observed\",\"output\":[{\"type\":\"message\",\"role\":"
              "\"assistant\",\"content\":[{\"type\":\"output_text\",\"text\":"
              "\"answer\"}]}],\"usage\":{\"input_tokens\":1}}}\n\n"};
        if (protocol == "messages")
          return ProviderHttpResponse{
              200,
              R"({"type":"message","model":"observed","stop_reason":"end_turn","content":[{"type":"thinking","thinking":"opaque"},{"type":"text","text":"answer"}],"usage":{"input_tokens":1}})"};
        return ProviderHttpResponse{
            200,
            R"({"model":"observed","choices":[{"finish_reason":"stop","message":{"role":"assistant","content":"answer"}}],"usage":{"prompt_tokens":1}})"};
      };
      const auto reply = call_colleague(
          make(provider), capture,
          [&](const Json &prepared, const ColleagueCapture &sink) {
            return native_colleague_transport(prepared, sink, {}, &config);
          });
      check(reply.find("status")->string() == "completed", "native colleague failed");
      check(reply.find("text")->string() == "answer", "wrong native text");
      check(reply.find("actual_model")->string() == "observed", "model alias inferred");
    }
    check(calls == 6, "extra dispatch");
    for (const int status : {401, 403, 429}) {
      config.http = [&](const ProviderHttpRequest &) {
        ++calls;
        return ProviderHttpResponse{status, R"({"error":"rejected"})"};
      };
      const auto reply = call_colleague(
          make("mimo"), capture,
          [&](const Json &prepared, const ColleagueCapture &sink) {
            return native_colleague_transport(prepared, sink, {}, &config);
          });
      check(reply.find("status")->string() == "failed", "auth rejection unknown");
      check(reply.find("remote_disposition")->string() == "reported_failure",
            "rejection disposition");
    }
    const auto prepared = prepare_colleague(make("mimo"));
    auth.set_key("mimo", "other", "private-key");
    const auto before = calls;
    try {
      (void)native_colleague_transport(prepared, capture, {}, &config);
      check(false, "changed account dispatched");
    } catch (const Error &e) {
      check(e.code == ErrorCode::conflict, "account change wrong failure");
    }
    check(calls == before, "dispatch after account change");
    config.http = [](const ProviderHttpRequest &) -> ProviderHttpResponse {
      throw Error{ErrorCode::provider_transport, 28};
    };
    const auto timeout = call_colleague(
        make("mimo"), capture, [&](const Json &p, const ColleagueCapture &sink) {
          return native_colleague_transport(p, sink, {}, &config);
        });
    check(timeout.find("remote_disposition")->string() == "unknown",
          "timeout treated as auth rejection");
    std::filesystem::remove_all(tmp);
    if (had)
      (void)::setenv("BLACKBIRD_AUTH_HOME", prior.c_str(), 1);
    else
      (void)::unsetenv("BLACKBIRD_AUTH_HOME");
    std::cout << "provider colleague checks passed\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << ':' << e.detail << '\n';
  }
  std::filesystem::remove_all(tmp);
  if (had)
    (void)::setenv("BLACKBIRD_AUTH_HOME", prior.c_str(), 1);
  else
    (void)::unsetenv("BLACKBIRD_AUTH_HOME");
  return 1;
}
