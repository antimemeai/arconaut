#include "blackbird/coding.hpp"
#include "blackbird/decision_models.hpp"
#include "blackbird/json.hpp"
#include <iostream>
#include <unistd.h>

using namespace blackbird;
namespace {
void check(bool value, const char *message) {
  if (!value)
    throw std::runtime_error(message);
}
Value parse(std::string_view s) { return unwrap(parse_json(s)); }
Value request() {
  return parse(R"({"state":{"message":"Compiler error: mismatched types"},"questions":{
    "route":{"type":"choice","instructions":"Classify message","criteria":{"compiler":"Compilation error","other":null}},
    "failure":{"type":"noul","instructions":{"question":"Does message report failure?"}},
    "severity":{"type":"score","instructions":"Rate severity","criteria":["None","Moderate","Severe"]}}})");
}
std::string response() {
  return R"({"model":"jev-1.13.0","answers":{
    "route":{"type":"choice","choice":"compiler","probabilities":{"compiler":0.8,"other":0.2},"confidence":0.7},
    "failure":{"type":"noul","noul":0.9},
    "severity":{"type":"score","score":1.5,"probabilities":{"0":0,"1":0.5,"2":0.5},"legend":{"0":"None","1":"Moderate","2":"Severe"},"confidence":0.5}},
    "usage":{"input_tokens":421,"output_tokens":14}})";
}
void set(Value &v, std::string_view key, Value value) {
  for (auto &[k, x] : v.object())
    if (k == key) {
      x = std::move(value);
      return;
    }
  v.object().emplace_back(std::string{key}, std::move(value));
}
template <class F> void rejects(F f, ErrorCode code) {
  try {
    f();
  } catch (const Error &e) {
    check(e.code == code, "wrong failure class");
    return;
  }
  throw std::runtime_error("invalid case accepted");
}
int fake(int argc, char **argv) {
  const auto base = std::filesystem::path(std::getenv("BB_JEV_TEST_ROOT"));
  Value::Array args;
  for (int i = 1; i < argc; ++i)
    args.emplace_back(std::string{argv[i]});
  write_file(base / "argv", unwrap(dump_json(Value{args})));
  std::string input{std::istreambuf_iterator<char>{std::cin},
                    std::istreambuf_iterator<char>{}};
  if (input.find("Authorization: Bearer native-test-secret") == std::string::npos ||
      input.find("https://api.typesafe.ai/v1/systemone") == std::string::npos)
    return 11;
  const auto mode = read_file(base / "mode");
  if (mode == "curl")
    return 56;
  if (mode == "http") {
    std::cout << "{\"error\":\"overload\"}\n529";
    return 0;
  }
  if (mode == "redirect") {
    std::cout << "\n302";
    return 0;
  }
  if (mode == "overflow") {
    std::cout << std::string(16384, 'x');
    return 0;
  }
  if (mode == "cancel" || mode == "timeout") {
    write_file(base / "started", "yes");
    sleep(3);
  }
  if (mode == "large") {
    std::cout << response() << std::string(131072, ' ') << "\n200";
    return 0;
  }
  if (mode == "malformed")
    std::cout << "not JSON\n200";
  else
    std::cout << response() << "\n200";
  return 0;
}
template <class T> T id(unsigned char value) {
  IdentityBytes b{};
  b[0] = std::byte{value};
  return unwrap(T::from_bytes(b));
}
class NoProvider final : public CodingProvider {
  Value respond(const Value &, const std::function<void(std::string_view)> &) override {
    throw std::runtime_error("unexpected conversational call");
  }
};
} // namespace
int main(int argc, char **argv) {
  if (argc > 1 && std::string_view{argv[1]} == "--disable")
    return fake(argc, argv);
  try {
    auto q = request();
    const auto service = jev_request(q);
    check(string_field(service, "model") == "jev-1.13.0", "default not pinned");
    validate_jev_response(service, parse(response()));
    for (const auto &bad : {"{}", "{\"state\":1,\"questions\":{}}",
                            "{\"state\":\"x\",\"questions\":{\"q\":{\"type\":\"score\","
                            "\"instructions\":\"x\",\"criteria\":[\"only\"]}}}",
                            "{\"state\":\"x\",\"questions\":{\"q\":{\"type\":"
                            "\"unknown\",\"instructions\":\"x\"}}}"})
      rejects([&] { (void)jev_request(parse(bad)); }, ErrorCode::invalid_range);
    auto credentials = q;
    set(credentials, "api_key", Value{"never accept"});
    rejects([&] { (void)jev_request(credentials); }, ErrorCode::invalid_range);
    auto alien = q;
    set(alien, "provider", Value{"alien"});
    rejects([&] { (void)jev_request(alien); }, ErrorCode::unsupported);
    auto model = parse(response());
    set(model, "model", Value{"jev-1.14.0"});
    rejects([&] { validate_jev_response(service, model); }, ErrorCode::corrupt);
    auto alias = q;
    set(alias, "model", Value{"jev-latest"});
    validate_jev_response(jev_request(alias), model);
    for (const auto &bad : {"\"noul\":1.9", "\"score\":1.7", "\"choice\":\"other\"",
                            "\"input_tokens\":-1"}) {
      auto bytes = response();
      const std::string key =
          std::string{bad}.substr(0, std::string{bad}.find(':') + 1);
      auto pos = bytes.find(key);
      auto end = bytes.find_first_of(",}", pos);
      bytes.replace(pos, end - pos, bad);
      rejects([&] { validate_jev_response(service, parse(bytes)); },
              ErrorCode::corrupt);
    }
    auto wrong_legend = parse(response());
    auto answers = field(wrong_legend, "answers");
    auto severity = field(answers, "severity");
    set(severity, "legend", parse(R"({"0":"Severe","1":"Moderate","2":"None"})"));
    set(answers, "severity", severity);
    set(wrong_legend, "answers", answers);
    rejects([&] { validate_jev_response(service, wrong_legend); }, ErrorCode::corrupt);
    auto missing = parse(response());
    auto fewer = field(missing, "answers");
    fewer.object().pop_back();
    set(missing, "answers", fewer);
    rejects([&] { validate_jev_response(service, missing); }, ErrorCode::corrupt);
    auto base = std::filesystem::temp_directory_path() /
                ("blackbird-jev-" + std::to_string(getpid()));
    std::filesystem::create_directories(base);
    std::filesystem::permissions(base, std::filesystem::perms::owner_all);
    (void)setenv("BB_JEV_TEST_ROOT", base.c_str(), 1);
    (void)unsetenv("TYPESAFE_API_KEY");
    (void)unsetenv("jev");
    write_file(base / "credentials", "export jev='native-test-secret' # local test\n");
    write_file(base / "mode", "ok");
    DecisionModelConfig config;
    config.curl_executable = std::filesystem::canonical(argv[0]);
    config.credential_file = base / "credentials";
    config.response_limit = 8192;
    DecisionModels adapter{config};
    std::string observed;
    adapter.observer = [&](std::string_view label, std::string_view bytes) {
      observed += label;
      observed += bytes;
    };
    const auto valid = adapter.evaluate(q);
    check(field(valid, "response") == parse(response()), "raw judgments changed");
    const auto args = read_file(base / "argv");
    check(args.find("native-test-secret") == std::string::npos &&
              observed.find("native-test-secret") == std::string::npos,
          "secret escaped transport");
    check(args.find("--disable") != std::string::npos &&
              args.find("--config") != std::string::npos &&
              args.find("=https") != std::string::npos &&
              args.find("--location") == std::string::npos,
          "curl controls missing");
    const auto &before = args;
    rejects([&] { (void)adapter.evaluate(credentials); }, ErrorCode::invalid_range);
    check(read_file(base / "argv") == before, "invalid request dispatched");
    observed.clear();
    write_file(base / "mode", "http");
    rejects([&] { (void)adapter.evaluate(q); }, ErrorCode::external_unknown);
    check(observed.find("overload") != std::string::npos &&
              observed.find("529") != std::string::npos,
          "HTTP failure bytes lost");
    write_file(base / "mode", "redirect");
    rejects([&] { (void)adapter.evaluate(q); }, ErrorCode::external_unknown);
    write_file(base / "mode", "curl");
    rejects([&] { (void)adapter.evaluate(q); }, ErrorCode::provider_transport);
    write_file(base / "mode", "overflow");
    rejects([&] { (void)adapter.evaluate(q); }, ErrorCode::capacity);
    write_file(base / "mode", "malformed");
    rejects([&] { (void)adapter.evaluate(q); }, ErrorCode::corrupt);
    write_file(base / "mode", "large");
    auto large_config = config;
    large_config.response_limit = 262144;
    DecisionModels large{large_config};
    auto short_request = q;
    set(short_request, "timeout_seconds", Value{Number{"1"}});
    check(string_field(large.evaluate(short_request), "status") == "ok",
          "quiet stderr stalled body drainage");
    write_file(base / "mode", "cancel");
    adapter.cancelled = [&] { return std::filesystem::exists(base / "started"); };
    rejects([&] { (void)adapter.evaluate(q); }, ErrorCode::interrupted);
    adapter.cancelled = {};
    write_file(base / "mode", "timeout");
    auto timed = q;
    set(timed, "timeout_seconds", Value{Number{"1"}});
    rejects([&] { (void)adapter.evaluate(timed); }, ErrorCode::provider_transport);
    write_file(base / "credentials", "jev='bad\nheader'\n");
    rejects([&] { (void)adapter.evaluate(q); }, ErrorCode::unsupported);
    write_file(base / "credentials", "jev=native-test-secret\n");
    write_file(base / "mode", "ok");
    const auto auditdir = base / "session";
    std::filesystem::create_directory(auditdir);
    std::filesystem::permissions(auditdir, std::filesystem::perms::owner_all);
    JournalHeader h{
        id<EnvironmentId>(51), id<AuditStreamId>(52), 3, {32768, 131072}, std::nullopt};
    auto root = unwrap(
        RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                  NativeJournalDirectory::open(auditdir.string()))),
                              "audit", h, {64 * 1024 * 1024, 10000}));
    AuditLog log{*root};
    ContextStore context{log};
    NoProvider provider;
    CodingEngine engine{log, context, provider, "test", config};
    engine.turn({"", "return blackbird.decide(blackbird.json.decode([=[" +
                         unwrap(dump_json(q)) + "]=]))"});
    const auto result = engine.workflow_result();
    const auto ref = string_field(result, "audit_ref");
    bool settled = false;
    for (const auto &fact : root->committed_facts())
      if (const auto *admission = std::get_if<AttemptAdmissionEvent>(&fact.event.body);
          admission && hex_identity(admission->attempt.bytes()) == ref) {
        const auto attempt = unwrap(root->attempt(admission->attempt));
        settled = attempt.opened && attempt.observation &&
                  attempt.observation->disposition == AttemptDisposition::success;
      }
    check(settled, "native effect not linked/settled");
    check(read_file(auditdir / "audit").find("native-test-secret") == std::string::npos,
          "credential in audit");
    check(read_file(auditdir / "audit").find("decision_model.response") !=
              std::string::npos,
          "raw response absent from audit");
    bool registered = false;
    const auto definitions = tool_definitions();
    for (const auto &tool : definitions.array())
      if (string_field(tool, "name") == "decision_model")
        registered = true;
    check(registered, "model tool missing");
    std::filesystem::remove_all(base);
    std::cout << "decision models passed\n";
    return 0;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << ' ' << e.detail << '\n';
    return 1;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
