#pragma once
#include "blackbird/context.hpp"
#include <functional>
namespace blackbird {
struct ObservationTime {
  std::int64_t utc_ns = 0, monotonic_ns = 0;
  std::uint64_t sampling_span_ns = 0;
};
ObservationTime observation_time();
// Observes values; never governs their authoring, activation or restoration.
class Observations {
public:
  explicit Observations(AuditLog &log,
                        std::function<ObservationTime()> clock = observation_time);
  Json time() const;
  Json sample(std::string_view name, const Json &value, const Json &source,
              std::string_view status = "observed");
  Json git(const Json &query, const std::function<bool()> &cancelled = {});
  void doctrine(std::string_view content, InvocationId invocation,
                OperationAttemptId attempt);

private:
  AuditLog &log_;
  std::function<ObservationTime()> clock_;
  std::string clock_id_, doctrine_id_, doctrine_content_;
};
// A point plot over a finite observed page. Never fills intervals between samples.
std::string correlation_plot(const Json &page);
} // namespace blackbird
