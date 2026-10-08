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
std::string read_text(const ImmutableBytes &bytes);
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
  void retain_program(std::string_view source, DefinitionGenerationId generation);
  Json inspect(const Json &query);
  RetainedState &root() noexcept { return root_; }

private:
  RetainedState &root_;
};
class ContextStore {
public:
  explicit ContextStore(AuditLog &log);
  ~ContextStore();
  ContextStore(const ContextStore &) = delete;
  ContextStore &operator=(const ContextStore &) = delete;
  Result<void> checkpoint();
  const std::optional<Error> &checkpoint_error() const noexcept {
    return checkpoint_error_;
  }
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
  ContextStore(AuditLog &log, bool restore_saved);
  std::optional<Json> original_entry(std::string_view id) const;
  void maybe_checkpoint();
  mutable bool archive_loaded_ = true;
  std::optional<Error> checkpoint_error_;
  AuditLog &log_;
  std::weak_ptr<void> root_lifetime_;
  RetainedState::FactHistory historical_;
  std::string head_;
  Json::Array entries_;
  struct Original {
    ImmutableBytes payload;
    std::size_t index = 0;
    bool packet = false;
    std::string id;
    Original(const Json &entry);
    Original(ImmutableBytes source, std::size_t ordinal, std::string identity)
        : payload(std::move(source)), index(ordinal), packet(true),
          id(std::move(identity)) {}
    Json entry() const;
  };
  mutable std::map<std::string, Original> originals_;
  mutable std::vector<Original> captured_;
  Json::Array captured_entries() const;
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
