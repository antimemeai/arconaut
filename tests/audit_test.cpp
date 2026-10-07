#include "blackbird/context.hpp"
#include <filesystem>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
Json num(std::size_t n) { return Json{JsonNumber{std::to_string(n)}}; }
void check(bool b) {
  if (!b)
    throw Error{ErrorCode::corrupt};
}
int main() {
  char name[] = "/tmp/arco-audit-XXXXXX";
  const auto *p = mkdtemp(name);
  if (!p)
    return 2;
  try {
    JournalHeader h{id<EnvironmentId>(1),
                    id<AuditStreamId>(2),
                    3,
                    {1024 * 1024, 4 * 1024 * 1024},
                    std::nullopt};
    JournalCapacity cap{32 * 1024 * 1024, 4096};
    std::size_t original_index = 0, end = 0;
    const std::string raw{"A\0\xffZ", 4};
    {
      auto root =
          unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                           unwrap(NativeJournalDirectory::open(p))),
                                       "audit", h, cap));
      AuditLog log{*root};
      log.original(
          {"provider.request", raw, Json::object({{"attempt", Json{"attempt-X"}}})});
      log.original(
          {"provider.request", raw, Json::object({{"attempt", Json{"attempt-Y"}}})});
      end = root->committed_facts().size();
      auto index = log.inspect(Json::object({{"end", num(end)}, {"count", num(64)}}));
      std::size_t found = 0;
      for (const auto &row : field(index, "records").array())
        if (row.find("label")) {
          check(string_field(row, "label") == "provider.request");
          if (found++ == 0)
            original_index = std::stoull(field(row, "record").number().text);
        }
      check(found == 2);
      auto page = log.inspect(Json::object({{"record", num(original_index)},
                                            {"source", num(0)},
                                            {"offset", num(1)},
                                            {"limit", num(2)}}));
      check(string_field(page, "hex") == "00ff");
      check(field(page, "total_bytes").number().text == "4");
      log.record(ApplicationChannel::program, Json::object({{"new", Json{true}}}));
      check(
          field(log.inspect(Json::object({{"end", num(end)}})), "end").number().text ==
          std::to_string(end));
      bool invalid = false;
      try {
        (void)log.inspect(Json::object({{"limit", num(65537)}}));
      } catch (const Error &e) {
        invalid = e.code == ErrorCode::invalid_range;
      }
      check(invalid);
    }
    {
      auto root =
          unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(
                                         unwrap(NativeJournalDirectory::open(p))),
                                     "audit", h, cap));
      unwrap(root->confirm_recovery());
      AuditLog log{*root};
      auto page = log.inspect(Json::object(
          {{"record", num(original_index)}, {"source", num(0)}, {"limit", num(4)}}));
      check(string_field(page, "hex") == "4100ff5a");
    }
    std::filesystem::remove_all(p);
    return 0;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << "\n";
    std::filesystem::remove_all(p);
    return 1;
  }
}
