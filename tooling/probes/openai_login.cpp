#include "blackbird/json.hpp"
#include "blackbird/openai.hpp"
#include <iostream>
using namespace blackbird;
int main(int argc, char **argv) {
  const bool live = argc == 3 && std::string_view{argv[1]} == "--live";
  if (argc != 1 && !live) {
    std::cerr << "usage: openai_login_probe [--live MODEL]\n";
    return 2;
  }
  OpenAiConfig config;
  auto login = codex_login(config);
  if (!login.has_value()) {
    std::cerr << "login: " << error_name(login.error().code) << '\n';
    return 1;
  }
  auto catalog = openai_http(config, login.value(), "models?client_version=0.160.0");
  if (!catalog.has_value()) {
    std::cerr << "catalog: " << error_name(catalog.error().code) << ' '
              << catalog.error().detail << '\n';
    return 1;
  }
  auto models = parse_json(catalog.value());
  if (!models.has_value() || models.value().find("models") == nullptr)
    return 1;
  std::cout << "Codex ChatGPT login accepted; "
            << models.value().find("models")->array().size() << " models available\n";
  if (!live)
    return 0;
  auto request =
      Value::object({{"model", Value{argv[2]}},
                     {"instructions", Value{"Reply exactly ARCO_LOGIN_OK."}},
                     {"input", Value{Value::Array{Value::object(
                                   {{"role", Value{"user"}},
                                    {"content", Value{"Verify this connection."}}})}}},
                     {"store", Value{false}},
                     {"stream", Value{true}}});
  auto stream = openai_http(config, login.value(), "responses", &request);
  if (!stream.has_value()) {
    std::cerr << "inference: " << error_name(stream.error().code) << ' '
              << stream.error().detail << '\n';
    return 1;
  }
  auto response = completed_response(stream.value());
  if (!response.has_value()) {
    std::cerr << "stream: " << error_name(response.error().code) << '\n';
    return 1;
  }
  bool found = false;
  for (const auto &item : response.value().find("output")->array()) {
    const auto *content = item.find("content");
    if (content == nullptr)
      continue;
    for (const auto &part : content->array()) {
      const auto *value = part.find("text");
      if (value != nullptr && value->string() == "ARCO_LOGIN_OK")
        found = true;
    }
  }
  if (!found) {
    std::cerr << "inference did not produce expected text; output items="
              << response.value().find("output")->array().size() << '\n';
    for (const auto &item : response.value().find("output")->array()) {
      if (const auto *type = item.find("type"))
        std::cerr << "item type=" << type->string() << '\n';
      if (const auto *parts = item.find("content"))
        for (const auto &part : parts->array())
          if (const auto *txt = part.find("text"))
            std::cerr << "text=" << dump_json(*txt).value() << '\n';
    }
    return 1;
  }
  std::cout << "Completed native Arco Responses request: ARCO_LOGIN_OK\n";
}
