#pragma once
#include "blackbird/provider_auth.hpp"
#include "blackbird/value.hpp"
#include <functional>

namespace blackbird {
// One bounded, context-only request. No implicit context, continuation or retries.
// Capture must retain bytes or throw; an admission capture failure prevents dispatch.
using ColleagueCapture = std::function<void(std::string_view, std::string_view)>;
using ColleagueTransport =
    std::function<Value(const Value &, const ColleagueCapture &)>;
Value prepare_colleague(const Value &request);
Value call_colleague(const Value &request, const ColleagueCapture &capture,
                     const ColleagueTransport &transport);
Value native_colleague_transport(const Value &prepared, const ColleagueCapture &capture,
                                 const std::function<bool()> &cancelled = {},
                                 const ProviderAuthConfig *owned_config = nullptr);
Value colleague_catalog();
} // namespace blackbird
