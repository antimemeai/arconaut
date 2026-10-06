#include "arconaut/coding.hpp"
#include <iostream>
#include <unistd.h>
using namespace arconaut;
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
void check(bool good) {
  if (!good)
    throw Error{ErrorCode::corrupt};
}
int main() {
  char name[] = "/tmp/arco-session-XXXXXX";
  const auto path = mkdtemp(name);
  if (!path)
    return 2;
  const JournalHeader header{id<EnvironmentId>(1),
                             id<AuditStreamId>(2),
                             3,
                             {1024 * 1024, 4 * 1024 * 1024},
                             std::nullopt};
  const JournalCapacity capacity{16 * 1024 * 1024, 10000};
  std::string actor, conversation, workflow;
  try {
    {
      auto root =
          unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                           unwrap(NativeJournalDirectory::open(path))),
                                       "audit", header, capacity));
      AuditLog log{*root};
      ContextStore context{log};
      SessionStore store{log};
      store.save({"chosen", "high", "/absolute/turn.lua"});
      const auto count = root->committed_facts().size();
      bool refused = false;
      try {
        store.save({"invalid", "nonsense", "/absolute/turn.lua"});
      } catch (const Error &e) {
        refused = e.code == ErrorCode::invalid_range;
      }
      check(refused && root->committed_facts().size() == count &&
            store.settings().model == "chosen");
      refused = false;
      try {
        store.save({std::string(1025, 'x'), "high", "/absolute/turn.lua"});
      } catch (const Error &e) {
        refused = e.code == ErrorCode::invalid_range;
      }
      check(refused && root->committed_facts().size() == count);
      const auto identity = session_identity(log);
      actor = hex_identity(identity.actor.bytes());
      conversation = hex_identity(identity.conversation.bytes());
      workflow = hex_identity(identity.workflow.bytes());
      check(session_identity(log).conversation == identity.conversation);
      save_session_info(path, store.settings(), identity);
      const auto audit_before = read_file(std::filesystem::path{path} / "audit");
      auto listed = list_sessions(path);
      check(field(listed, "sessions").array().size() == 1 &&
            string_field(field(listed, "sessions").array()[0], "metadata_status") ==
                "snapshot");
      write_file(std::filesystem::path{path} / "session-info.json",
                 std::string(1024 * 1024 + 1, 'x'));
      listed = list_sessions(path);
      check(string_field(field(listed, "sessions").array()[0], "metadata_status") ==
            "damaged");
      check(read_file(std::filesystem::path{path} / "audit") == audit_before);
      save_session_info(path, store.settings(), identity);
      store.restart("Retain this note");
      const auto &fact = root->committed_facts().back();
      const auto &record = std::get<ApplicationRecordEvent>(fact.event.body);
      const auto packet = unwrap(parse_json(
          std::string_view{reinterpret_cast<const char *>(record.payload.data()),
                           record.payload.size()}));
      const auto origin = "rrc:" + string_field(packet, "token");
      // Simulate a crash after the context append, before consuming the intent.
      context.append({Json::object({{"role", Json{"user"}},
                                    {"content", Json{"continue\nRetain this note"}}})},
                     origin);
    }
    {
      auto root =
          unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(
                                         unwrap(NativeJournalDirectory::open(path))),
                                     "audit", header, capacity));
      unwrap(root->confirm_recovery());
      recover_coding_session(*root);
      AuditLog log{*root};
      ContextStore context{log};
      SessionStore store{log};
      check(store.settings() ==
            SessionSettings{"chosen", "high", "/absolute/turn.lua"});
      const auto identity = session_identity(log);
      check(actor == hex_identity(identity.actor.bytes()) &&
            conversation == hex_identity(identity.conversation.bytes()) &&
            workflow == hex_identity(identity.workflow.bytes()));
      check(context.items().size() == 1 && store.resume(context) &&
            context.items().size() == 1);
      check(!store.resume(context));
      SessionStore replay{log};
      check(!replay.resume(context));
    }
    std::filesystem::remove_all(path);
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
}
