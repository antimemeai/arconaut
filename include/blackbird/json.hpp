#pragma once
#include "blackbird/foundation.hpp"
#include <memory>
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
  Json() : data_(std::make_shared<Value>(nullptr)) {}
  explicit Json(std::nullptr_t) : Json() {}
  explicit Json(bool value) : data_(std::make_shared<Value>(value)) {}
  explicit Json(JsonNumber value) : data_(std::make_shared<Value>(std::move(value))) {}
  explicit Json(std::string value) : data_(std::make_shared<Value>(std::move(value))) {}
  explicit Json(const char *value)
      : data_(std::make_shared<Value>(std::string{value})) {}
  explicit Json(Array value) : data_(std::make_shared<Value>(std::move(value))) {}
  explicit Json(Object value) : data_(std::make_shared<Value>(std::move(value))) {}
  // Mutable container borrows must remain private even if a later copy is made.
  Json(const Json &other)
      : data_(other.exposed_ ? std::make_shared<Value>(other.value()) : other.data_) {}
  Json &operator=(const Json &other) {
    if (this != &other) {
      Json next{other};
      swap(next);
    }
    return *this;
  }
  Json(Json &&) noexcept = default;
  Json &operator=(Json &&) noexcept = default;
  void swap(Json &other) noexcept {
    data_.swap(other.data_);
    std::swap(exposed_, other.exposed_);
  }
  static Json object(Object value) { return Json{std::move(value)}; }
  const Value &value() const noexcept {
    static const Value empty{nullptr};
    return data_ ? *data_ : empty;
  }
  const std::string &string() const { return std::get<std::string>(value()); }
  const JsonNumber &number() const { return std::get<JsonNumber>(value()); }
  const Array &array() const { return std::get<Array>(value()); }
  Array &array() { return std::get<Array>(write()); }
  const Object &object() const { return std::get<Object>(value()); }
  Object &object() { return std::get<Object>(write()); }
  const Json *find(std::string_view key) const noexcept;
  bool operator==(const Json &other) const {
    return data_ == other.data_ || value() == other.value();
  }

private:
  Value &write() {
    if (data_.use_count() != 1)
      data_ = std::make_shared<Value>(value());
    exposed_ = true;
    return *data_;
  }
  std::shared_ptr<Value> data_;
  bool exposed_ = false;
};
Result<Json> parse_json(std::string_view input, JsonLimits limits = {});
// Fully validates input and bounds; selected root object values become null.
// Nested same-named fields are unaffected. Not a partial/trusting parser.
Result<Json> parse_json_projection(std::string_view input,
    std::span<const std::string_view> omitted, JsonLimits limits = {});
Result<std::string> dump_json(const Json &value, JsonLimits limits = {});
} // namespace blackbird
