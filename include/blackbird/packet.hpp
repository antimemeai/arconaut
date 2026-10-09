#pragma once
#include "blackbird/json.hpp"

namespace blackbird {
// BBM version 1 metadata packets; JSON is accepted only for existing history.
Result<std::vector<std::byte>> encode_packet(const Json &value, JsonLimits limits = {});
Result<Json> decode_packet(ByteView bytes, JsonLimits limits = {});
} // namespace blackbird
