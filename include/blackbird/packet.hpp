#pragma once
#include "blackbird/value.hpp"

namespace blackbird {
// BBM version 2 metadata packets. No format sniffing or legacy fallback.
Result<std::vector<std::byte>> encode_packet(const Value &value,
                                             ValueLimits limits = {});
Result<Value> decode_packet(ByteView bytes, ValueLimits limits = {});
Result<Value> decode_packet_projection(ByteView bytes,
                                       std::span<const std::string_view> omitted,
                                       ValueLimits limits = {});
Result<std::string> encode_packet_string(const Value &value, ValueLimits limits = {});
Result<Value> decode_packet_value(const Value &bytes, ValueLimits limits = {});
Result<Value> decode_packet_string(std::string_view bytes, ValueLimits limits = {});
} // namespace blackbird
