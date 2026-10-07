#pragma once
#include "blackbird/json.hpp"
#include <filesystem>
#include <functional>

namespace blackbird {
struct DecisionModelConfig {
  std::string curl_executable = "curl";
  std::filesystem::path credential_file;
  std::size_t request_limit = 1024 * 1024;
  std::size_t response_limit = 1024 * 1024;
};
// Host configuration is separate from model arguments. Construction performs no I/O.
class DecisionModels {
public:
  explicit DecisionModels(DecisionModelConfig config = {})
      : config_(std::move(config)) {}
  std::function<bool()> cancelled;
  std::function<void(std::string_view, std::string_view)> observer;
  Json evaluate(const Json &arguments);

private:
  DecisionModelConfig config_;
};
// Pure validators also serve native callers and deterministic service fixtures.
Json jev_request(const Json &arguments);
void validate_jev_response(const Json &request, const Json &response);
} // namespace blackbird
