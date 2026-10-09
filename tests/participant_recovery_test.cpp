#include "blackbird/coding.hpp"
#include "blackbird/json.hpp"
#include "blackbird/packet.hpp"
#include <iostream>
#include <unistd.h>
using namespace blackbird;
void check(bool value) {
  if (!value)
    throw Error{ErrorCode::corrupt};
}
template <class T> T id(unsigned char value) {
  IdentityBytes bytes{};
  bytes[0] = std::byte{value};
  return unwrap(T::from_bytes(bytes));
}
class Provider final : public CodingProvider {
public:
  int calls = 0;
  Value respond(const Value &, const std::function<void(std::string_view)> &) override {
    ++calls;
    throw Error{ErrorCode::corrupt};
  }
};
struct Boundary final : EffectBoundary {
  Result<void> dispatch(const EffectIntent &) override {
    return Result<void>::success();
  }
};
void scenario(const std::filesystem::path &path, std::string_view operation,
              bool allowed) {
  std::filesystem::create_directories(path);
  std::filesystem::permissions(path, std::filesystem::perms::owner_all);
  const JournalHeader header{id<EnvironmentId>(1),
                             id<AuditStreamId>(2),
                             3,
                             {1024 * 1024, 4 * 1024 * 1024},
                             std::nullopt};
  const JournalCapacity capacity{32 * 1024 * 1024, 10000};
  std::string run_id;
  {
    auto root =
        unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                         NativeJournalDirectory::open(path.string()))),
                                     "audit", header, capacity));
    AuditLog log{*root};
    ContextStore context{log};
    context.append({}, "initial");
    const auto identity = session_identity(log);
    const auto generation = unwrap(root->issue<DefinitionGenerationId>());
    const auto decision = unwrap(root->issue<DecisionId>());
    const auto invocation = unwrap(root->issue<InvocationId>());
    const auto attempt = unwrap(root->issue<OperationAttemptId>());
    run_id = hex_identity(attempt.bytes());
    IdentityBytes revision_bytes{};
    for (std::size_t i = 0; i < 16; ++i)
      revision_bytes[i] = static_cast<std::byte>(
          std::stoul(context.head().substr(2 * i, 2), nullptr, 16));
    const auto revision = unwrap(ContextRevisionId::from_bytes(revision_bytes));
    const auto input =
        unwrap(encode_packet_string(Value::object({{"request", Value::object({})}})));
    const auto bytes = std::as_bytes(std::span{input.data(), input.size()});
    const auto metadata = unwrap(encode_packet_string(
        Value::object({{"operation", Value{std::string{operation}}},
                       {"input_binding", Value{"invocation-v1"}},
                       {"invocation", Value{hex_identity(invocation.bytes())}},
                       {"generation", Value{hex_identity(generation.bytes())}},
                       {"revision", Value{context.head()}}})));
    const auto continuation =
        std::as_bytes(std::span{metadata.data(), metadata.size()});
    const std::array<RetainedEvent, 3> admission{
        RetainedEvent{{},
                      DecisionEvent{decision,
                                    identity.actor,
                                    identity.conversation,
                                    identity.workflow,
                                    generation,
                                    revision,
                                    {invocation},
                                    {continuation.begin(), continuation.end()}}},
        RetainedEvent{
            {},
            InvocationEvent{
                invocation, decision, generation, {bytes.begin(), bytes.end()}}},
        RetainedEvent{
            {},
            AttemptAdmissionEvent{
                attempt, invocation, decision, {bytes.begin(), bytes.end()}, true}}};
    unwrap(root->append(root->cursor(), {}, admission));
    if (allowed)
      log.record(ApplicationChannel::program,
                 Value::object({{"label", Value{"participants-state-v1"}},
                                {"concurrency", Value{Number{"2"}}},
                                {"runs", Value{Value::Array{Value::object(
                                             {{"run_id", Value{run_id}},
                                              {"state", Value{"running"}},
                                              {"cancel_requested", Value{false}},
                                              {"task_id", Value{""}},
                                              {"provider", Value{"openai"}},
                                              {"model", Value{"fake"}},
                                              {"from", Value{"main"}},
                                              {"to", Value{"peer"}},
                                              {"result", Value{}}})}}}}));
    Boundary boundary;
    const auto report = unwrap(root->dispatch(attempt, boundary));
    unwrap(report.recording);
    check(report.dispatched);
    // Reopen with a dispatched admission but no terminal observation, as after crash.
  }
  {
    auto root =
        unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(unwrap(
                                       NativeJournalDirectory::open(path.string()))),
                                   "audit", header, capacity));
    unwrap(root->confirm_recovery());
    bool refused = false;
    try {
      recover_coding_session(*root);
    } catch (const Error &error) {
      refused = error.code == ErrorCode::external_unknown;
    }
    check(refused != allowed);
    if (!allowed)
      return;
    check(unwrap(root->unresolved_attempts()).empty());
    AuditLog log{*root};
    ContextStore context{log};
    Provider provider;
    CodingEngine engine{log, context, provider, "fake"};
    const auto result = engine.operator_call(
        "participant_read", Value::object({{"run_id", Value{run_id}}}));
    check(string_field(result, "state") == "unknown" &&
          field(result, "reopened") == Value{true} && provider.calls == 0);
  }
}
int main() {
  char name[] = "/tmp/blackbird-participant-recovery-XXXXXX";
  const auto path = mkdtemp(name);
  if (!path)
    return 2;
  try {
    scenario(std::filesystem::path{path} / "start", "participant_start", true);
    scenario(std::filesystem::path{path} / "exec", "exec", false);
    std::filesystem::remove_all(path);
  } catch (const Error &error) {
    std::cerr << error_name(error.code) << ':' << error.detail << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
}
