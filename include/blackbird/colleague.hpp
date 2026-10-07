#pragma once
#include "blackbird/json.hpp"
#include <functional>

namespace blackbird {
// One bounded, context-only request. No implicit context, continuation or retries.
// Capture must retain bytes or throw; an admission capture failure prevents dispatch.
using ColleagueCapture = std::function<void(std::string_view, std::string_view)>;
using ColleagueTransport = std::function<Json(const Json &, const ColleagueCapture &)>;
Json prepare_colleague(const Json &request);
Json call_colleague(const Json &request, const ColleagueCapture &capture,
                    const ColleagueTransport &transport);
Json native_colleague_transport(const Json &prepared, const ColleagueCapture &capture);
} // namespace blackbird
