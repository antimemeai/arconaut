#include "blackbird/colleague.hpp"
#include "blackbird/json.hpp"
#include "blackbird/tools.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace blackbird;
int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "usage: blackbird-colleague REQUEST.json NEW_CAPTURE_DIRECTORY\n";
    return 2;
  }
  try {
    const auto request = unwrap(parse_json(read_file(argv[1], 65536)));
    // Refuse existing capture directory; reading it is recovery, never retry.
    if (!std::filesystem::create_directory(argv[2])) {
      std::cerr << "capture directory exists; no dispatch\n";
      return 2;
    }
    const std::filesystem::path directory{argv[2]};
    std::filesystem::permissions(directory, std::filesystem::perms::owner_all);

    std::ofstream raw;
    raw.exceptions(std::ios::failbit | std::ios::badbit);
    raw.open(directory / "raw.bin", std::ios::binary);
    const auto capture = [&](std::string_view kind, std::string_view bytes) {
      if (kind == "raw") {
        raw.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        raw.flush();
      } else {
        write_file(directory / (std::string{kind} + ".bbm"), bytes);
      }
    };
    const auto result = call_colleague(
        request, capture, [](const Value &prepared, const ColleagueCapture &retain) {
          return native_colleague_transport(prepared, retain);
        });
    std::cout << unwrap(dump_json(result)) << '\n';
    return result.find("status")->string() == "completed" ? 0 : 1;
  } catch (const Error &e) {
    std::cerr << "colleague stopped: " << error_name(e.code)
              << "; inspect retained capture, do not replay\n";
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "colleague stopped: " << e.what()
              << "; inspect retained capture, do not replay\n";
    return 1;
  }
}
