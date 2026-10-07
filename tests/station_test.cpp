#include "blackbird/coding.hpp"

#include "blackbird/station.hpp"
#include <filesystem>
#include <iostream>

#include <unistd.h>
using namespace blackbird;
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
void check(bool ok) {
  if (!ok)
    throw Error{ErrorCode::corrupt};
}
Json event(std::string name) {
  return Json::object({{"source", Json{"repo"}},
                       {"id", Json{name}},
                       {"cursor", Json{"commit-42"}},
                       {"prompt", Json{"inspect actual source"}}});
}
Json command(std::string name, std::string action) {
  return Json::object({{"id", Json{name}}, {"action", Json{action}}});
}
int main() {
  char name[] = "/tmp/arco-station-XXXXXX";
  auto made = mkdtemp(name);
  if (!made)
    return 1;
  std::filesystem::path path{made};
  const JournalHeader header{id<EnvironmentId>(1),
                             id<AuditStreamId>(2),
                             7,
                             {1024 * 1024, 4 * 1024 * 1024},
                             std::nullopt};
  try {
    unsigned dispatches = 0;
    {
      auto root = unwrap(
          RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                    NativeJournalDirectory::open(path.string()))),
                                "audit", header, {64 * 1024 * 1024, 20000}));
      AuditLog log{*root};
      StationStore station{log};
      check(station.admit(event("done")));
      ++dispatches;
      station.finish(event("done"), true);
      check(!station.admit(event("done")));
      check(station.control(command("p", "pause")));
      check(!station.admit(event("pending")));
      check(station.control(command("r", "resume")));
      check(!station.control(command("p", "pause")) && !station.paused());
      auto steer = command("s", "steer");
      steer.object().emplace_back("text", Json{"narrow native ownership map"});
      check(station.control(steer));
      check(station.prompt(event("pending")).find("narrow native ownership map") !=
            std::string::npos);
      check(station.admit(event("uncertain")));
      ++dispatches;
      // No finish: emulate a died owner, not a settled external effect.
    }
    {
      auto root =
          unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(unwrap(
                                         NativeJournalDirectory::open(path.string()))),
                                     "audit", header, {64 * 1024 * 1024, 20000}));
      unwrap(root->confirm_recovery());
      recover_coding_session(*root);

      AuditLog log{*root};
      StationStore station{log};
      check(station.paused());
      check(!station.admit(event("uncertain")));
      check(station.control(command("r2", "resume")));
      check(!station.admit(event("uncertain")) && !station.admit(event("done")));
      check(dispatches == 2);
      check(station.admit(event("new")));
      ++dispatches;
      station.finish(event("new"), false);
      check(station.paused());
      check(station.control(command("stop", "stop")) && station.stopped());
      check(station.control(command("capacity-resume", "resume")));
      unsigned sequence = 0;
      bool capacity = false;
      while (!capacity) {
        try {
          station.control(command("capacity-" + std::to_string(sequence++), "resume"));
        } catch (const Error &e) {
          check(e.code == ErrorCode::capacity);
          capacity = true;
        }
      }
      station.pause("command table exhausted");
      check(station.paused() && !station.admit(event("after-capacity")));
    }
    std::filesystem::remove_all(path);
    std::cout << "station admission, uncertain reopen, pause, steering: passed\n";
    return 0;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
}
