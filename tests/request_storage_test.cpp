#include "blackbird/coding.hpp"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
using namespace blackbird;
template <class T> T id(unsigned char n) {
  IdentityBytes b{}; b[0] = std::byte{n}; return unwrap(T::from_bytes(b));
}
void check(bool ok) { if (!ok) throw Error{ErrorCode::corrupt}; }
struct Provider final : CodingProvider {
  std::string request;
  bool abrupt = false;
  Json respond(const Json &input, const std::function<void(std::string_view)> &) override {
    request = unwrap(dump_json(input));
    if (abrupt) _exit(0); // No response acceptance or C++ cleanup.
    return Json::object({{"output", Json{Json::Array{}}}});
  }
};
int main() {
  char name[] = "/tmp/blackbird-request-storage-XXXXXX";
  const auto base = mkdtemp(name);
  if (!base) return 2;
  try {
    const JournalHeader header{id<EnvironmentId>(1), id<AuditStreamId>(2), 3,
      {4 * 1024 * 1024, 16 * 1024 * 1024}, std::nullopt};
    const JournalCapacity capacity{32 * 1024 * 1024, 10000};
    for (bool abrupt : {false, true}) {
      const auto path = std::string{base} + (abrupt ? "/abrupt" : "/completed");
      std::filesystem::create_directory(path);
      std::filesystem::permissions(path, std::filesystem::perms::owner_all);
      auto run = [&] {
        auto root = unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
          unwrap(NativeJournalDirectory::open(path))), "audit", header, capacity));
        AuditLog log{*root}; ContextStore context{log}; Provider provider;
        provider.abrupt = abrupt;
        CodingEngine engine{log, context, provider, "test"};
        engine.turn({std::string(262144, 'x'), "arco.request()"});
      };
      if (abrupt) {
        const auto child = fork(); check(child >= 0);
        if (child == 0) { run(); _exit(3); }
        int status = 0; check(waitpid(child, &status, 0) == child);
        check(WIFEXITED(status) && WEXITSTATUS(status) == 0);
      } else run();
      auto root = unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(
        unwrap(NativeJournalDirectory::open(path))), "audit", header, capacity));
      unwrap(root->confirm_recovery()); recover_coding_session(*root);
      std::size_t requests = 0, decisions = 0, input_size = 0, metadata_size = 0;
      std::optional<OperationAttemptId> provider_attempt;
      for (std::size_t i = 0; i < root->fact_count(); ++i) {
        const auto fact = unwrap(root->fact(i));
        if (const auto *decision = std::get_if<DecisionEvent>(&fact.event.body)) {
          const auto meta = unwrap(parse_json(read_text(decision->continuation)));
          if (string_field(meta, "operation") != "provider") continue;
          ++decisions;
          check(!meta.find("input") && string_field(meta, "input_binding") == "invocation-v1");
          metadata_size += decision->continuation.size();
          check(decision->planned_invocations.size() == 1);
          const auto invocation = unwrap(root->invocation(decision->planned_invocations[0]));
          check(string_field(meta, "invocation") == hex_identity(invocation.invocation.bytes()));
          const auto parsed = unwrap(parse_json(read_text(invocation.input)));
          check(field(parsed, "input").array()[0].find("content"));
          input_size += invocation.input.size();
        }
        if (const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
            record && record->channel == ApplicationChannel::log) {
          const auto packet = unwrap(parse_json(read_text(record->payload)));
          if (string_field(packet, "label") != "provider.request") continue;
          ++requests;
          check(fact.event.dependencies.empty()); // No redundant request source frame.
          check(string_field(field(packet, "metadata"), "input_binding") == "attempt-invocation-v1");
        }
        if (const auto *admission = std::get_if<AttemptAdmissionEvent>(&fact.event.body)) {
          const auto decision = unwrap(root->decision(admission->decision));
          const auto meta = unwrap(parse_json(read_text(decision.continuation)));
          if (string_field(meta, "operation") == "provider") {
            provider_attempt = admission->attempt;
            check(admission->input == unwrap(root->invocation(admission->invocation)).input);
          }
        }
      }
      check(provider_attempt && requests == 1 && decisions == 1 && input_size > 262144 && metadata_size < 1024);
      const auto attempt = unwrap(root->attempt(*provider_attempt));
      check(attempt.observation && !attempt.reconciliation_required);
      check(attempt.observation->disposition == (abrupt ? AttemptDisposition::unknown : AttemptDisposition::success));
      struct NoReplay final : EffectBoundary {
        int calls = 0;
        Result<void> dispatch(const EffectIntent &) override { ++calls; return Result<void>::success(); }
      } no_replay;
      check(!unwrap(root->dispatch(*provider_attempt, no_replay)).dispatched && no_replay.calls == 0);
      std::cout << (abrupt ? "abrupt" : "completed") << ": input=" << input_size
        << " decision_metadata=" << metadata_size << " request_capture_body=0; two native input copies remain\n";
    }
    std::filesystem::remove_all(base);
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << ':' << e.detail << '\n';
    std::filesystem::remove_all(base); return 1;
  }
}
