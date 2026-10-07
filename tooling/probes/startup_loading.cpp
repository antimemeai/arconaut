// Private-fixture restoration and actual CodingEngine admission; deterministic
// local provider only, with known success and no external service or side effect.
#include "blackbird/coding.hpp"
#include "blackbird/local_timing.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
using namespace blackbird;
template<class T> T fixed(unsigned char n) {
  IdentityBytes b{}; b[0]=std::byte{n}; return unwrap(T::from_bytes(b));
}
int main(int argc, char **argv) {
#if BLACKBIRD_DEBUG
  LocalTimingSession timing_session; // explicit per-process opt-in; shutdown flush
#endif
  if(argc < 3) return 2;
  try {
    const auto start=std::chrono::steady_clock::now();
    const std::filesystem::path path{argv[2]};
    std::filesystem::create_directories(path);
    JournalHeader h{fixed<EnvironmentId>(1),fixed<AuditStreamId>(2),3,
                    {2*1024*1024,4*1024*1024},std::nullopt};
    JournalCapacity cap{1024ULL*1024*1024,200000};
    auto directory=std::make_unique<NativeJournalDirectory>(unwrap(NativeJournalDirectory::open(path.string())));
    const bool create=std::string_view{argv[1]}=="create";
    LocalSpan replay_timing{"startup.replay", "tooling/probes/startup_loading.cpp"};
    auto root=create ? unwrap(RetainedState::create(std::move(directory),"audit",h,cap))
                     : unwrap(RetainedState::open(std::move(directory),"audit",h,cap));
    if(!create) {
      unwrap(root->confirm_recovery());
      struct EmptyCustody final : CustodyVerifier {
        Result<void> verify(std::span<const AttemptState> attempts) override {
          return attempts.empty() ? Result<void>::success()
                                 : Result<void>::failure({ErrorCode::conflict});
        }
      } custody;
      unwrap(root->reconcile(custody));
    }
    replay_timing.outcome("success");
    replay_timing.finish();
    const auto initial_bytes=std::filesystem::file_size(path/"audit");
    const auto initial_facts=root->committed_facts().size();
    const auto replay=std::chrono::steady_clock::now();
    LocalSpan context_timing{"startup.context", "tooling/probes/startup_loading.cpp"};
    AuditLog log{*root}; ContextStore context{log};
    context_timing.outcome("success");
    context_timing.finish();
    if(create) {
      context.append({Json::object({{"role",Json{"user"}},{"content",Json{"fixture"}}})},"fixture");
      const std::string payload(512*1024,'x');
      const unsigned count=argc>3 ? static_cast<unsigned>(std::stoul(argv[3])) : 640;
      for(unsigned i=0;i<count;++i) {
        const auto identity=log.issue();
        log.record(identity,ApplicationChannel::context,Json::object({
          {"op",Json{"edit"}},{"revision",Json{hex_identity(identity.bytes())}},
          {"observed",Json{context.head()}},{"accepted",Json{false}},
          {"candidate",Json{payload}},{"entries",Json{Json::Array{}}}}));
      }
      std::cout << "created_bytes=" << std::filesystem::file_size(path/"audit") << " facts=" << root->committed_facts().size() << '\n';
      return 0;
    }
    const auto restored=std::chrono::steady_clock::now();
    const auto items=context.items(); // actual usable live prompt, not a banner
    const auto ready=std::chrono::steady_clock::now();
    const auto serialized=unwrap(dump_json(Json{items}));
    struct LocalProvider final : CodingProvider {
      RetainedState &root;
      std::chrono::steady_clock::time_point admitted;
      std::size_t input_bytes = 0;
      std::size_t first_fact;
      std::optional<AttemptAdmissionEvent> admission;
      explicit LocalProvider(RetainedState &r) : root(r), first_fact(r.committed_facts().size()) {}
      std::string_view provider_identity() const noexcept override { return "deterministic-local-fixture"; }
      Json respond(const Json &request,
                   const std::function<void(std::string_view)> &capture) override {
        admitted = std::chrono::steady_clock::now(); // before any probe validation
        // CodingEngine has prepared and durably admitted/opened the actual
        // request. This provider performs no external effect and returns known
        // success, which the engine settles normally (never a fabricated unknown).
        const auto encoded = unwrap(dump_json(request));
        ImmutableBytes expected{reinterpret_cast<const std::byte *>(encoded.data()),
                                reinterpret_cast<const std::byte *>(encoded.data() + encoded.size())};
        const DecisionEvent *decision = nullptr;
        const InvocationEvent *invocation = nullptr;
        bool opened = false;
        const auto &facts = root.committed_facts();
        for (std::size_t i = first_fact; i < facts.size(); ++i) {
          const auto &body = facts[i].event.body;
          if (const auto *a = std::get_if<AttemptAdmissionEvent>(&body); a && a->input == expected) {
            if (admission) throw Error{ErrorCode::conflict};
            admission = *a;
          }
        }
        if (!admission) throw Error{ErrorCode::corrupt};
        for (std::size_t i = first_fact; i < facts.size(); ++i) {
          const auto &body = facts[i].event.body;
          if (const auto *d = std::get_if<DecisionEvent>(&body); d && d->decision == admission->decision) decision = d;
          if (const auto *v = std::get_if<InvocationEvent>(&body); v && v->invocation == admission->invocation) invocation = v;
          if (const auto *o = std::get_if<AttemptOpenEvent>(&body); o && o->attempt == admission->attempt) opened = true;
        }
        if (!decision || !invocation || !opened || invocation->input != expected ||
            invocation->decision != decision->decision || invocation->definition != decision->definition ||
            decision->planned_invocations != std::vector<InvocationId>{invocation->invocation})
          throw Error{ErrorCode::corrupt};
        const auto meta = unwrap(parse_json({reinterpret_cast<const char *>(decision->continuation.data()), decision->continuation.size()}));
        if (string_field(meta, "operation") != "provider" || field(meta, "input") != request ||
            string_field(meta, "revision") != hex_identity(decision->context.bytes()) ||
            string_field(meta, "generation") != hex_identity(decision->definition.bytes()))
          throw Error{ErrorCode::corrupt};
        input_bytes = unwrap(dump_json(field(request, "input"))).size();
        auto response = Json::object({{"output", Json{Json::Array{}}}});
        capture(unwrap(dump_json(response)));
        return response;
      }
    } provider{*root};
    LocalSpan admission_timing{"startup.request_admission", "tooling/probes/startup_loading.cpp"};
    CodingEngine engine{log, context, provider, "deterministic-fixture"};
    engine.turn({"", "return blackbird.request()"});
    // This native span includes known-success settlement; admitted_ms above
    // ends earlier, at provider entry. Do not conflate those two boundaries.
    admission_timing.outcome("success");
    admission_timing.finish();
    if (!provider.admission) throw Error{ErrorCode::corrupt};
    const auto settled = unwrap(root->attempt(provider.admission->attempt));
    if (!settled.opened || settled.reconciliation_required || !settled.observation ||
        settled.observation->phase != AttemptPhase::terminal ||
        settled.observation->disposition != AttemptDisposition::success)
      throw Error{ErrorCode::corrupt};
    const auto admitted=provider.admitted;
    if(admitted == std::chrono::steady_clock::time_point{}) throw Error{ErrorCode::corrupt};
    auto ms=[&](auto t){return std::chrono::duration<double,std::milli>(t-start).count();};
    std::cout << "initial_bytes=" << initial_bytes << " initial_facts=" << initial_facts << " bytes=" << std::filesystem::file_size(path/"audit") << " facts=" << root->committed_facts().size()
              << " live_bytes=" << serialized.size() << " request_input_bytes=" << provider.input_bytes << " replay_ms=" << ms(replay)
              << " restored_ms=" << ms(restored) << " prompt_ms=" << ms(ready) << " admitted_ms=" << ms(admitted) << '\n';
  } catch(const Error &e) {std::cerr << "error=" << error_name(e.code) << " detail=" << e.detail << '\n';return 1;}
}
