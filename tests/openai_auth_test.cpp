#include "blackbird/openai.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <source_location>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
using namespace blackbird;
namespace {
void check(bool good, std::source_location where = std::source_location::current()) {
  if (!good)
    throw std::runtime_error("native authentication contract line " +
                             std::to_string(where.line()));
}
void fixture(const std::filesystem::path &home,
             std::string_view token = "private-access",
             std::string_view account = "private-account",
             std::string_view mode = "chatgpt") {
  std::ofstream file{home / "auth.json"};
  file << "{\"auth_mode\":\"" << mode << "\",\"tokens\":{\"access_token\":\"" << token
       << "\",\"account_id\":\"" << account << "\"}}";
  file.close();
  check(::chmod((home / "auth.json").c_str(), 0600) == 0);
}
int auth_helper() {
  const char *home = std::getenv("CODEX_HOME");
  if (home == nullptr)
    return 3;
  std::string mode;
  std::ifstream{std::filesystem::path{home} / "mode"} >> mode;
  std::string line;
  if (!std::getline(std::cin, line) ||
      line.find("\"method\":\"initialize\"") == std::string::npos)
    return 4;
  std::cout << "{\"id\":\"init\",\"result\":{}}\n" << std::flush;
  if (!std::getline(std::cin, line) ||
      line.find("\"method\":\"initialized\"") == std::string::npos)
    return 5;
  if (!std::getline(std::cin, line) ||
      line.find("\"includeToken\":true") == std::string::npos)
    return 6;
  if (mode == "missing")
    std::cout << "{\"id\":\"auth\",\"result\":{\"authMethod\":\"chatgpt\","
                 "\"authToken\":null}}\n";
  else {
    const bool refresh = line.find("\"refreshToken\":true") != std::string::npos;
    const auto token = refresh ? "private-refreshed" : "private-access";
    if (refresh || mode == "switch")
      fixture(home, token, mode == "switch" ? "other-account" : "private-account");
    std::cout
        << "{\"id\":\"auth\",\"result\":{\"authMethod\":\"chatgpt\",\"authToken\":\""
        << token << "\"}}\n";
  }
  std::cout.flush();
  return 0;
}
int curl_helper(int argc, char **argv) {
  for (int i = 1; i < argc; ++i)
    if (std::string_view{argv[i]}.find("private-") != std::string_view::npos)
      return 7;
  std::string config{std::istreambuf_iterator<char>{std::cin},
                     std::istreambuf_iterator<char>{}};
  if (config.find("Authorization: Bearer private-") == std::string::npos ||
      config.find("ChatGPT-Account-ID: private-account") == std::string::npos)
    return 8;
  if (config.find("https://chatgpt.com/backend-api/codex/") == std::string::npos)
    return 9;
  if (config.find("triggerTimeout") != std::string::npos) {
    std::cout << "data: before timeout\n" << std::flush;
    ::sleep(3);
    return 0;
  }
  if (config.find("trigger92") != std::string::npos) {
    std::cout << "data: partial stream\n" << std::flush;
    return 92;
  }
  if (config.find("trigger401") != std::string::npos) {
    std::cout << "{\"error\":{\"message\":\"PRIVATE_BODY_MUST_NOT_ESCAPE\"}}\n401";
    return 0;
  }
  if (config.find("data-binary =") != std::string::npos) {
    if (config.find(R"(data-binary = "{\"opaque\":\"A\\\\B\\\"C\"}")") ==
        std::string::npos)
      return 10;
    std::cout << "data: "
                 "{\"type\":\"response.completed\",\"response\":{\"status\":"
                 "\"completed\",\"output\":[]}}\n\n\n200";
  } else
    std::cout << "{\"models\":[{\"slug\":\"test\"}]}\n200";
  return 0;
}
struct Temporary {
  std::filesystem::path home;
  Temporary() {
    char name[] = "/tmp/arco-auth-XXXXXX";
    const char *path = ::mkdtemp(name);
    if (path == nullptr)
      throw std::runtime_error("mkdtemp");
    home = path;
  }
  ~Temporary() {
    std::error_code ignored;
    std::filesystem::remove_all(home, ignored);
  }
};
} // namespace
int main(int argc, char **argv) {
  if (argc > 1) {
    if (std::string_view{argv[1]} == "app-server")
      return auth_helper();
    if (std::string_view{argv[1]} == "--disable")
      return curl_helper(argc, argv);
    return 2;
  }
  try {
    Temporary tmp;
    fixture(tmp.home);
    OpenAiConfig config;
    config.codex_executable = std::filesystem::absolute(argv[0]).string();
    config.curl_executable = config.codex_executable;
    config.codex_home = tmp.home;
    auto login = codex_login(config);
    check(login.has_value());
    auto models = openai_http(config, login.value(), "models?client_version=0.160.0");
    check(models.has_value() && models.value() == "{\"models\":[{\"slug\":\"test\"}]}");
    auto request = Json::object({{"opaque", Json{"A\\B\"C"}}});
    auto stream = openai_http(config, login.value(), "responses", &request);
    check(stream.has_value() && completed_response(stream.value()).has_value());
    check(!openai_http(config, login.value(), "https://other.invalid/").has_value());
    std::string partial;
    config.response_observer = [&](std::string_view bytes) { partial += bytes; };
    auto dropped_request = Json::object({{"trigger92", Json{true}}});
    auto dropped = openai_http(config, login.value(), "responses", &dropped_request);
    check(!dropped.has_value() &&
          dropped.error().code == ErrorCode::provider_transport &&
          dropped.error().detail == 92 && partial == "data: partial stream\n");
    config.timeout_seconds = 1;
    auto deadline_request = Json::object({{"triggerTimeout", Json{true}}});
    auto deadline_failure =
        openai_http(config, login.value(), "responses", &deadline_request);
    check(!deadline_failure.has_value() &&
          deadline_failure.error().code == ErrorCode::provider_transport &&
          deadline_failure.error().detail == 28 &&
          partial.ends_with("data: before timeout\n"));
    config.timeout_seconds = 120;
    config.response_observer = {};
    auto failure = Json::object({{"trigger401", Json{true}}});
    auto refused = openai_http(config, login.value(), "responses", &failure);
    check(!refused.has_value() &&
          refused.error() == Error{ErrorCode::external_unknown, 401});
    config.response_limit = 8;
    auto bounded = openai_http(config, login.value(), "models?client_version=0.160.0");
    check(!bounded.has_value() && bounded.error().code == ErrorCode::capacity);
    config.response_limit = 1024;
    check(codex_login(config, true).has_value());
    fixture(tmp.home);
    {
      std::ofstream{tmp.home / "mode"} << "switch";
    }
    auto switched = codex_login(config);
    check(!switched.has_value() && switched.error().code == ErrorCode::conflict);
    fixture(tmp.home);
    {
      std::ofstream{tmp.home / "mode"} << "missing";
    }
    auto missing = codex_login(config);
    check(!missing.has_value() && missing.error().code == ErrorCode::corrupt);
    fixture(tmp.home, "private-access", "private-account", "apikey");
    auto wrong_mode = codex_login(config);
    check(!wrong_mode.has_value() && wrong_mode.error().code == ErrorCode::unsupported);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
