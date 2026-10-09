#include "blackbird/coding.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
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
      std::size_t requests = 0, decisions = 0, input_size = 0, metadata_size = 0,
                  admission_wire = 0;
      std::optional<OperationAttemptId> provider_attempt;
      for (std::size_t i = 0; i < root->fact_count(); ++i) {
        const auto fact = unwrap(root->fact(i));
        if (const auto *decision = std::get_if<DecisionEvent>(&fact.event.body)) {
          const auto meta = unwrap(read_packet(decision->continuation));
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
          const auto packet = unwrap(read_packet(record->payload));
          if (string_field(packet, "label") != "provider.request") continue;
          ++requests;
          check(fact.event.dependencies.empty()); // No redundant request source frame.
          check(string_field(field(packet, "metadata"), "input_binding") == "attempt-invocation-v1");
        }
        if (const auto *admission = std::get_if<AttemptAdmissionEvent>(&fact.event.body)) {
          const auto decision = unwrap(root->decision(admission->decision));
          const auto meta = unwrap(read_packet(decision.continuation));
          if (string_field(meta, "operation") == "provider") {
            provider_attempt = admission->attempt;
            check(admission->input_from_invocation);
            admission_wire +=
                unwrap(encode_retained_event(fact.event, header.limits.max_payload))
                    .size();
            check(admission->input == unwrap(root->invocation(admission->invocation)).input);
          }
        }
      }
      check(provider_attempt && requests == 1 && decisions == 1 &&
            input_size > 262144 && metadata_size < 1024 && admission_wire == 56);
      const auto raw = unwrap(root->read_original_range(
          0, static_cast<std::size_t>(root->cursor().end_offset)));
      std::size_t offset = journal_header_size, persisted_admissions = 0;
      while (offset < raw.bytes.size()) {
        const auto frame = unwrap(
            decode_journal_frame(ByteView{raw.bytes}.subspan(offset), header.limits));
        if (frame.kind == FrameKind::semantic) {
          const auto event =
              unwrap(decode_retained_event(frame.payload, header.limits.max_payload));
          if (const auto *admitted = std::get_if<AttemptAdmissionEvent>(&event.body);
              admitted && admitted->attempt == *provider_attempt) {
            check(admitted->input_from_invocation && admitted->input.empty());
            check(frame.payload.size() == 56);
            ++persisted_admissions;
          }
        }
        offset += frame.encoded_size;
      }
      check(offset == raw.bytes.size() && persisted_admissions == 1);
      const auto attempt = unwrap(root->attempt(*provider_attempt));
      check(attempt.observation && !attempt.reconciliation_required);
      check(attempt.observation->disposition == (abrupt ? AttemptDisposition::unknown : AttemptDisposition::success));
      struct NoReplay final : EffectBoundary {
        int calls = 0;
        Result<void> dispatch(const EffectIntent &) override { ++calls; return Result<void>::success(); }
      } no_replay;
      check(!unwrap(root->dispatch(*provider_attempt, no_replay)).dispatched && no_replay.calls == 0);
      std::cout << (abrupt ? "abrupt" : "completed") << ": input=" << input_size
                << " decision_metadata=" << metadata_size
                << " admission_wire=" << admission_wire << " request_capture_body=0\n";
    }
    {
      const auto path = std::string{base} + "/large-result";
      std::filesystem::create_directory(path);
      std::filesystem::permissions(path, std::filesystem::perms::owner_all);
      const auto file = path + "/input";
      { std::ofstream out{file}; out << std::string(262144, 'x'); check(bool(out)); }
      std::string expected;
      std::size_t terminal_record = 0;
      {
        auto root = unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
            unwrap(NativeJournalDirectory::open(path))), "audit", header, capacity));
        AuditLog log{*root}; ContextStore context{log}; Provider provider;
        CodingEngine engine{log, context, provider, "test"};
        const auto result = engine.operator_call("read_file", Json::object({{"path", Json{file}}}));
        check(string_field(result, "content").size() == 262144 && provider.request.empty());
        expected = unwrap(dump_json(result));
        terminal_record = root->fact_count() - 1;
      }
      auto root = unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(
          unwrap(NativeJournalDirectory::open(path))), "audit", header, capacity));
      unwrap(root->confirm_recovery()); recover_coding_session(*root);
      const auto terminal = unwrap(root->fact(terminal_record));
      const auto &settlement = std::get<AttemptObservationEvent>(terminal.event.body);
      check(settlement.disposition == AttemptDisposition::success && settlement.observation.size() < 64);
      const auto link = unwrap(read_packet(settlement.observation));
      const auto record = std::stoull(field(link, "result_record").number().text);
      const auto original = unwrap(root->fact(record));
      const auto &capture = std::get<ApplicationRecordEvent>(original.event.body);
      check(string_field(link, "result_id") == hex_identity(capture.identity.bytes()) &&
            original.event.dependencies.size() == 1);
      const auto source = unwrap(root->source(original.event.dependencies[0]));
      check(std::string_view{reinterpret_cast<const char *>(source.data()), source.size()} == expected);
      std::size_t source_copies = 0;
      const auto raw = unwrap(root->read_original_range(0, static_cast<std::size_t>(root->cursor().end_offset)));
      for (std::size_t at = journal_header_size; at < raw.bytes.size();) {
        const auto frame = unwrap(decode_journal_frame(ByteView{raw.bytes}.subspan(at), header.limits));
        if (frame.kind == FrameKind::source && frame.payload.size() >= expected.size() &&
            std::search(frame.payload.begin(), frame.payload.end(), source.begin(), source.end()) != frame.payload.end())
          ++source_copies;
        at += frame.encoded_size;
      }
      check(source_copies == 1);
      std::cout << "Large result original=" << expected.size() << " settlement="
                << settlement.observation.size() << " bytes\n";
    }
    std::filesystem::remove_all(base);
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << ':' << e.detail << '\n';
    std::filesystem::remove_all(base); return 1;
  }
}
