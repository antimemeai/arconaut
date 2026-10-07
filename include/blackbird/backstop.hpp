#pragma once
#include "blackbird/coding.hpp"
namespace blackbird {
// Opt-in, cooperative recovery. Source custody remains held by the caller.
// Mission is a root-authored packet, not inferred authority from old context.
Json run_backstop(CodingEngine &source_engine, AuditLog &source_log,
                  const std::filesystem::path &source_session, const Json &mission,
                  CodingProvider &assessor, const SessionSettings &settings,
                  const std::function<bool()> &paused = {},
                  const std::function<void(std::string_view)> &display = {});
} // namespace blackbird
