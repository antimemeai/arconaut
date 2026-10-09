#pragma once
#include "blackbird/json.hpp"
#include <filesystem>
#include <functional>

namespace blackbird {
// Secret-bearing inputs and replies never enter the model/tool/audit interface.
struct ProviderHttpRequest {
  std::string url, method = "POST", body;
  std::vector<std::pair<std::string, std::string>> headers;
  int timeout_seconds = 30;
};
struct ProviderHttpResponse {
  int status = 0;
  std::string body;
};
using ProviderHttp = std::function<ProviderHttpResponse(const ProviderHttpRequest &)>;
struct ProviderAuthConfig {
  std::filesystem::path directory;
  std::string curl_executable = "/usr/bin/curl";
  std::string openssl_executable = "/usr/bin/openssl";
  std::function<bool()> cancelled;
  ProviderHttp http; // Deterministic transport injection, not a model setting.
  std::function<std::int64_t()> now;
};
ProviderHttpResponse
provider_http(const ProviderHttpRequest &request, const ProviderAuthConfig &config = {},
              const std::function<void(std::string_view)> &observer = {});
void require_provider_success(int status);

class ProviderAuth {
public:
  explicit ProviderAuth(ProviderAuthConfig config = {});
  // Metadata only. Does not refresh, probe an account, or expose tokens.
  Json catalog() const;
  Json status() const;
  bool selected(std::string_view provider) const;
  void set_key(std::string_view provider, std::string_view account, std::string key);
  void use(std::string_view provider, std::string_view account);
  void logout(std::string_view provider, std::string_view account);
  void add_provider(std::string id, std::string base_url, std::string protocol,
                    std::string environment);
  Json descriptor(std::string_view provider) const;
  Json registration(std::string_view provider, std::string_view account) const;
  Json binding(std::string_view provider) const;
  // One OAuth transaction at a time per account. Returned device data is safe
  // for the operator, not the token/device_code. Poll resumes private state.
  Json device_begin(std::string_view provider, std::string_view account);
  Json device_poll(std::string_view provider, std::string_view account);
  void browser_login(std::string_view provider, std::string_view account,
                     const std::function<void(std::string_view)> &notice,
                     const std::function<std::string()> &read_secret);
  // Browser flow implemented separately; commits only a complete token record.
  void save_oauth(std::string_view provider, std::string_view account,
                  const Json &tokens, std::string client_id, std::string token_url,
                  Json metadata = Json::object({}), std::int64_t expected_revision = -2,
                  std::int64_t expected_selection = -1);
  ProviderHttpResponse
  request(std::string_view provider, std::string_view route, const Json &body,
          int timeout_seconds,
          const std::function<void(std::string_view)> &observer = {},
          const Json *expected_binding = nullptr);
  std::filesystem::path directory() const;
  const ProviderAuthConfig &config() const { return config_; }

private:
  ProviderAuthConfig config_;
};
// Dedicated operator process; keys/callback codes never go through chat history.
int provider_auth_cli(int argc, char **argv);
// Direct deterministic checks of PKCE/JWT validation use these owning helpers.
std::string auth_pkce_challenge(std::string_view verifier,
                                const ProviderAuthConfig &config = {});
Json auth_validate_openai_identity(std::string_view id_token,
                                   std::string_view client_id, std::string_view nonce,
                                   const Json &jwks, std::int64_t now,
                                   const ProviderAuthConfig &config = {});
} // namespace blackbird
