#pragma once
#include "blackbird/value.hpp"
namespace blackbird {
// JSON exists only as an external interchange/export adapter over native values.
Result<Value> parse_json(std::string_view input, ValueLimits limits = {});
Result<Value> parse_json_projection(std::string_view input,
                                    std::span<const std::string_view> omitted,
                                    ValueLimits limits = {});
Result<std::string> dump_json(const Value &value, ValueLimits limits = {});
Value import_provider_response(Value response);
Value export_provider_request(Value request);
} // namespace blackbird
