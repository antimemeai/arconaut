// Private-fixture restoration and audited local admission, no provider or dispatch.
#include "blackbird/context.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
using namespace blackbird;
template<class T> T fixed(unsigned char n) {
  IdentityBytes b{}; b[0]=std::byte{n}; return unwrap(T::from_bytes(b));
}
int main(int argc, char **argv) {
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
    auto root=create ? unwrap(RetainedState::create(std::move(directory),"audit",h,cap))
                     : unwrap(RetainedState::open(std::move(directory),"audit",h,cap));
    if(!create) unwrap(root->confirm_recovery());
    const auto replay=std::chrono::steady_clock::now();
    AuditLog log{*root}; ContextStore context{log};
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
    const auto items=context.items(); // actual usable live prompt, not a banner
    const auto ready=std::chrono::steady_clock::now();
    const auto serialized=unwrap(dump_json(Json{items}));
    const auto raw=std::as_bytes(std::span{serialized.data(),serialized.size()});
    ImmutableBytes bytes{raw.begin(),raw.end()};
    const auto decision=unwrap(root->issue<DecisionId>());
    const auto invocation=unwrap(root->issue<InvocationId>());
    const auto attempt=unwrap(root->issue<OperationAttemptId>());
    const std::array<RetainedEvent,3> batch{
      RetainedEvent{{},DecisionEvent{decision,fixed<ParticipantId>(4),fixed<ConversationId>(5),fixed<WorkflowId>(6),fixed<DefinitionGenerationId>(7),fixed<ContextRevisionId>(8),{invocation},bytes}},
      RetainedEvent{{},InvocationEvent{invocation,decision,fixed<DefinitionGenerationId>(7),bytes}},
      RetainedEvent{{},AttemptAdmissionEvent{attempt,invocation,decision,bytes}}};
    (void)unwrap(root->append(root->cursor(),{},batch)); // authoritative admission; intentionally no effect open
    const auto admitted=std::chrono::steady_clock::now();
    auto ms=[&](auto t){return std::chrono::duration<double,std::milli>(t-start).count();};
    std::cout << "bytes=" << std::filesystem::file_size(path/"audit") << " facts=" << root->committed_facts().size()
              << " live_bytes=" << serialized.size() << " replay_ms=" << ms(replay)
              << " prompt_ms=" << ms(ready) << " admitted_ms=" << ms(admitted) << '\n';
  } catch(const Error &e) {std::cerr << "error=" << static_cast<int>(e.code) << '\n';return 1;}
}
