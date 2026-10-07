#pragma once
#include "blackbird/json.hpp"
#include "blackbird/retained_state.hpp"
#include <functional>
#include <map>

namespace blackbird {
template <class T> T unwrap(Result<T> result) {
  if (!result.has_value())
    throw result.error();
  return std::move(result).value();
}
inline void unwrap(Result<void> result) {
  if (!result.has_value())
    throw result.error();
}
std::string hex_identity(const IdentityBytes &bytes);
const Json &field(const Json &value, std::string_view name);
const std::string &string_field(const Json &value, std::string_view name);
struct OriginalCapture {
  std::string_view label;
  std::string_view bytes;
  Json metadata;
};
class AuditLog {
public:
  explicit AuditLog(RetainedState &root) : root_(root) {}
  ApplicationRecordId issue() { return unwrap(root_.issue<ApplicationRecordId>()); }
  std::string record(ApplicationChannel channel, const Json &packet);
  void record(ApplicationRecordId identity, ApplicationChannel channel,
              const Json &packet);
  // Atomically append managed context plus a native successful-boundary program record.
  void record_boundary(ApplicationRecordId identity, const Json &context_packet,
                       const Json &program_packet);
  std::string original(OriginalCapture capture);
  Json inspect(const Json &query);
  RetainedState &root() noexcept { return root_; }

private:
  RetainedState &root_;
};
class ContextStore {
public:
  explicit ContextStore(AuditLog &log);
  const std::string &head() const noexcept { return head_; }
  Json view() const;
  Json stats() const;
  Json::Array items() const;
  void append(Json::Array items, std::string_view origin);
  static void validate_successor_seed(const Json &seed);
  void seed_successor(const Json &seed);
  Json edit(const Json &candidate);
  void restore(std::string_view entry);
  Json originals() const;
  Json manage(const Json &proposal);
  Json inspect(const Json &query) const;
  // Native workflow owner validates prospective obligations before publication.
  std::function<void(const Json::Array &, const Json &)> protect;
  static Json::Array stop_outputs(const Json::Array &entries, bool interrupted);
  JournalCapacity cancellation_budget(const Json::Array &entries,
                                      const Json &proposal) const;
  Json pending_proposal() const;
  void begin_workflow();
  Json finish_workflow(bool success, const Json &boundary_program = Json{});

private:
  AuditLog &log_;
  std::string head_;
  Json::Array entries_;
  std::map<std::string, Json> originals_;
  Json::Array captured_;
  struct HistoryRecord {
    ImmutableBytes payload;
    std::string revision;
  };
  std::vector<HistoryRecord> history_;
  static HistoryRecord history_record(const Json &packet);
  bool workflow_ = false;
  struct Pending {
    Json proposal;
    Json::Array snapshot;
    std::string expected, stage;
  };
  std::optional<Pending> pending_;
  Json publish_managed(const Json &proposal, const Json::Array &basis,
                       std::string_view stage, const Json &boundary_program = Json{});
  Json reject_managed(const Json &proposal, std::string_view reason);
  bool valid_entries(const Json &entries) const;
  void append_impl(Json::Array items, std::string_view origin, const Json *lineage);
};
} // namespace blackbird
