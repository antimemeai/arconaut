#include "blackbird/context.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
#define CHECK(x)                                                                       \
  do {                                                                                 \
    if (!(x))                                                                          \
      throw std::runtime_error("diagnostics line " + std::to_string(__LINE__));        \
  } while (false)
int main() {
  char pattern[] = "/tmp/blackbird-diagnostics-XXXXXX";
  const auto path = mkdtemp(pattern);
  if (!path)
    return 2;
  try {
    auto directory = unwrap(NativeJournalDirectory::open(path));
    constexpr std::uint64_t now = 100000, ttl = 30ULL * 24 * 60 * 60;
    const std::string file = std::to_string(now + ttl) + "." + std::string(32, 'a');
    const std::string text = "optional raw transport";
    const auto bytes = std::as_bytes(std::span{text.data(), text.size()});
    unwrap(directory.store_diagnostic(file, bytes, now));
    CHECK(unwrap(directory.read_diagnostic(file, now + ttl - 1)) ==
          std::vector<std::byte>(bytes.begin(), bytes.end()));
    CHECK(!directory.read_diagnostic(file, now + ttl).has_value());
    CHECK(!directory.store_diagnostic("bad", bytes, now).has_value());
    CHECK(!directory.store_diagnostic(file, bytes, now).has_value());
    const auto folder = std::filesystem::path(path) / "diagnostics";
    std::ofstream(folder / "unrelated") << "keep";
    std::filesystem::create_symlink(
        "unrelated", folder / (std::to_string(now) + "." + std::string(32, 'b')));
    for (unsigned i = 0; i < 400; ++i)
      std::ofstream(folder / ("unrelated" + std::to_string(i))) << "keep";
    bool complete = false;
    std::size_t removed = 0;
    for (int pass = 0; pass < 40 && !complete; ++pass) {
      auto result = unwrap(directory.expire_diagnostics(now + ttl));
      CHECK(result.scanned <= 128 && result.removed <= 16);
      removed += result.removed;
      complete = result.complete;
    }
    CHECK(complete && removed == 1 && !std::filesystem::exists(folder / file));
    CHECK(std::filesystem::exists(folder / "unrelated"));
    const std::string fresh =
        std::to_string(now + ttl + 1) + "." + std::string(32, 'c');
    unwrap(directory.store_diagnostic(fresh, bytes, now + 1));
    auto moved = std::move(directory);
    CHECK(unwrap(moved.read_diagnostic(fresh, now + ttl)) ==
          std::vector<std::byte>(bytes.begin(), bytes.end()));
    std::filesystem::remove_all(path);
    return 0;
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << ':' << e.detail << '\n';
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  std::filesystem::remove_all(path);
  return 1;
}
