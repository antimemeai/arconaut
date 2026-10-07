#pragma once
#include "blackbird/foundation.hpp"
#include <string>
#include <string_view>

namespace blackbird {
struct JsonNumber {
  std::string text;
  bool operator==(const JsonNumber &) const = default;
};
struct JsonLimits {
  std::size_t bytes = 16 * 1024 * 1024;
  std::size_t nodes = 500000;
  std::size_t depth = 128;
};
class Json {
public:
  using Array = std::vector<Json>;
  using Object = std::vector<std::pair<std::string, Json>>;
  using Value =
      std::variant<std::nullptr_t, bool, JsonNumber, std::string, Array, Object>;
  Json() : data_(nullptr) {}
  explicit Json(std::nullptr_t) : data_(nullptr) {}
  explicit Json(bool value) : data_(value) {}
  explicit Json(JsonNumber value) : data_(std::move(value)) {}
  explicit Json(std::string value) : data_(std::move(value)) {}
  explicit Json(const char *value) : data_(std::string{value}) {}
  explicit Json(Array value) : data_(std::move(value)) {}
  explicit Json(Object value) : data_(std::move(value)) {}
  static Json object(Object value) { return Json{std::move(value)}; }
  const Value &value() const noexcept { return data_; }
  const std::string &string() const { return std::get<std::string>(data_); }
  const JsonNumber &number() const { return std::get<JsonNumber>(data_); }
  const Array &array() const { return std::get<Array>(data_); }
  Array &array() { return std::get<Array>(data_); }
  const Object &object() const { return std::get<Object>(data_); }
  Object &object() { return std::get<Object>(data_); }
  const Json *find(std::string_view key) const noexcept;
  bool operator==(const Json &) const = default;

private:
  Value data_;
};
Result<Json> parse_json(std::string_view input, JsonLimits limits = {});
// Fully validates input and bounds; selected root object values become null.
// Nested same-named fields are unaffected. Not a partial/trusting parser.
Result<Json> parse_json_projection(std::string_view input,
    std::span<const std::string_view> omitted, JsonLimits limits = {});
Result<std::string> dump_json(const Json &value, JsonLimits limits = {});
} // namespace blackbird
