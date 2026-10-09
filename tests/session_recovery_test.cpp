#include "blackbird/coding.hpp"
#include "blackbird/json.hpp"
#include "blackbird/packet.hpp"
#include <algorithm>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
using namespace blackbird;
template <class T> T id(unsigned char n) {
  IdentityBytes bytes{};
  bytes[0] = std::byte{n};
  return unwrap(T::from_bytes(bytes));
}
void check(bool good) {
  if (!good)
    throw Error{ErrorCode::corrupt};
}
struct Counter final : EffectBoundary {
  int calls = 0;
  Result<void> dispatch(const EffectIntent &) override {
    ++calls;
    return Result<void>::success();
  }
};
int main() {
  char name[] = "/tmp/arco-recovery-XXXXXX";
  const auto path = mkdtemp(name);
  if (!path)
    return 2;
  try {
    const JournalHeader header{id<EnvironmentId>(1),
                               id<AuditStreamId>(2),
                               3,
                               {1024 * 1024, 4 * 1024 * 1024},
                               std::nullopt};
    const JournalCapacity capacity{16 * 1024 * 1024, 10000};
    for (const std::string operation :
         {"provider", "unopened", "capacity", "badinput", "exec", "write_file",
          "unrecognized", "mixed", "bound-provider", "bound-unopened",
          "bound-wrong-invocation", "bound-wrong-tag", "bound-extra-input",
          "bound-bad-input"}) {
      const auto directory = std::filesystem::path{path} / operation;
      std::filesystem::create_directory(directory);
      check(::chmod(directory.c_str(), 0700) == 0);
      auto root = unwrap(
          RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                    NativeJournalDirectory::open(directory.string()))),
                                "audit", header, capacity));
      AuditLog log{*root};
      ContextStore context{log};
      context.append(
          {Value::object({{"role", Value{"user"}}, {"content", Value{"KEEP_ME"}}})},
          "fixture");
      const auto before = context.items();
      const bool provider_kind = operation == "provider" || operation == "mixed" ||
                                 operation == "unopened" || operation == "capacity" ||
                                 operation == "badinput" ||
                                 operation.starts_with("bound-");
      auto metadata_packet = Value::object(
          {{"operation", Value{provider_kind ? "provider" : operation}},
           {"input",
            operation == "badinput" ? Value{Value::Array{}} : Value::object({})},
           {"revision", Value{hex_identity(id<ContextRevisionId>(10).bytes())}},
           {"generation", Value{hex_identity(id<DefinitionGenerationId>(7).bytes())}}});
      if (operation.starts_with("bound-")) {
        auto &fields = metadata_packet.object();
        if (operation != "bound-extra-input")
          fields.erase(
              std::remove_if(fields.begin(), fields.end(),
                             [](const auto &entry) { return entry.first == "input"; }),
              fields.end());
        fields.emplace_back(
            "input_binding",
            Value{operation == "bound-wrong-tag" ? "unknown-v1" : "invocation-v1"});
        fields.emplace_back(
            "invocation",
            Value{hex_identity(
                id<InvocationId>(operation == "bound-wrong-invocation" ? 20 : 9)
                    .bytes())});
      }
      const auto metadata = unwrap(encode_packet_string(metadata_packet));
      const std::vector<std::byte> input_bytes =
          operation == "bound-bad-input"
              ? std::vector<std::byte>{std::byte{0x5b}, std::byte{0x5d}}
              : unwrap(encode_packet(Value::object({})));
      const auto raw = std::as_bytes(std::span{metadata.data(), metadata.size()});
      (void)unwrap(root->submit({{},
                                 DecisionEvent{id<DecisionId>(8),
                                               id<ParticipantId>(4),
                                               id<ConversationId>(5),
                                               id<WorkflowId>(6),
                                               id<DefinitionGenerationId>(7),
                                               id<ContextRevisionId>(10),
                                               {id<InvocationId>(9)},
                                               {raw.begin(), raw.end()}}}));
      (void)unwrap(
          root->submit({{},
                        InvocationEvent{id<InvocationId>(9), id<DecisionId>(8),
                                        id<DefinitionGenerationId>(7), input_bytes}}));
      (void)unwrap(root->submit(
          {{},
           AttemptAdmissionEvent{id<OperationAttemptId>(11), id<InvocationId>(9),
                                 id<DecisionId>(8), input_bytes}}));
      if (operation != "unopened" && operation != "bound-unopened")
        (void)unwrap(root->submit({{}, AttemptOpenEvent{id<OperationAttemptId>(11)}}));
      if (operation == "mixed") {
        const std::string metadata_exec = "{\"operation\":\"exec\"}";
        const auto exec_bytes =
            std::as_bytes(std::span{metadata_exec.data(), metadata_exec.size()});
        (void)unwrap(
            root->submit({{},
                          DecisionEvent{id<DecisionId>(13),
                                        id<ParticipantId>(4),
                                        id<ConversationId>(5),
                                        id<WorkflowId>(6),
                                        id<DefinitionGenerationId>(7),
                                        id<ContextRevisionId>(10),
                                        {id<InvocationId>(12)},
                                        {exec_bytes.begin(), exec_bytes.end()}}}));
        (void)unwrap(root->submit({{},
                                   InvocationEvent{id<InvocationId>(12),
                                                   id<DecisionId>(13),
                                                   id<DefinitionGenerationId>(7),
                                                   {}}}));
        (void)unwrap(root->submit({{},
                                   AttemptAdmissionEvent{id<OperationAttemptId>(14),
                                                         id<InvocationId>(12),
                                                         id<DecisionId>(13),
                                                         {}}}));
        (void)unwrap(root->submit({{}, AttemptOpenEvent{id<OperationAttemptId>(14)}}));
      }
      const auto saved_bytes = root->cursor().end_offset;
      root.reset();
      auto reopen = [&] {
        auto result = unwrap(RetainedState::open(
            std::make_unique<NativeJournalDirectory>(
                unwrap(NativeJournalDirectory::open(directory.string()))),
            "audit", header,
            operation == "capacity" ? JournalCapacity{saved_bytes, capacity.max_records}
                                    : capacity));
        unwrap(result->confirm_recovery());
        return result;
      };
      root = reopen();
      check(unwrap(root->attempt(id<OperationAttemptId>(11))).reconciliation_required);
      const auto prior_facts = root->committed_facts().size();
      bool blocked = false;
      try {
        recover_coding_session(*root);
      } catch (const Error &e) {
        blocked = e.code == (operation == "capacity" ? ErrorCode::capacity
                                                     : ErrorCode::external_unknown);
      }
      if (operation == "capacity") {
        check(blocked && root->state() == JournalWriterState::blocked);
        check(root->committed_facts().size() == prior_facts);
        check(
            unwrap(root->attempt(id<OperationAttemptId>(11))).reconciliation_required);
        Counter effect;
        check(!root->dispatch(id<OperationAttemptId>(11), effect).has_value() &&
              effect.calls == 0);
        continue;
      }
      if (operation != "provider" && operation != "unopened" &&
          operation != "bound-provider" && operation != "bound-unopened") {
        check(blocked && root->state() == JournalWriterState::recovered);
        check(root->committed_facts().size() == prior_facts);
        check(!unwrap(root->attempt(id<OperationAttemptId>(11))).observation);
        continue;
      }
      check(!blocked && root->state() == JournalWriterState::live);
      const auto attempt = unwrap(root->attempt(id<OperationAttemptId>(11)));
      check(!attempt.reconciliation_required && attempt.observation &&
            attempt.observation->disposition == AttemptDisposition::unknown);
      if (!attempt.observation)
        throw Error{ErrorCode::corrupt};
      const auto &observed = attempt.observation->observation;
      check(std::string_view{reinterpret_cast<const char *>(observed.data()),
                             observed.size()}
                .find("outcome unknown") != std::string_view::npos);
      Counter effect;
      check(!unwrap(root->dispatch(id<OperationAttemptId>(11), effect)).dispatched);
      check(effect.calls == 0);
      AuditLog recovered_log{*root};
      ContextStore recovered_context{recovered_log};
      check(recovered_context.items() == before);
      const auto count = root->committed_facts().size();
      root.reset();
      root = reopen();
      recover_coding_session(*root);
      check(root->committed_facts().size() == count);
    }
    std::filesystem::remove_all(path);
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << " (" << e.detail << ")" << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
}
