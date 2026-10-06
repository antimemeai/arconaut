// Isolated, intentional defects for actual tool qualification; never ship this target.
#include <climits>
#include <cstddef>
#include <cstring>
#include <memory>
#include <thread>

int main(int argc, char **argv) {
  if (argc != 2) {
    return 2;
  }
  if (std::strcmp(argv[1], "memory") == 0) {
    auto bytes = std::make_unique<int[]>(1);
    volatile std::size_t index = static_cast<std::size_t>(argc - 1);
    return bytes[index];
  }
  if (std::strcmp(argv[1], "undefined") == 0) {
    volatile int limit = INT_MAX;
    return limit + argc;
  }
  if (std::strcmp(argv[1], "race") == 0) {
    int value = 0;
    auto write = [&]() {
      for (int i = 0; i < 100000; ++i) {
        ++value;
      }
    };
    {
      const std::jthread first{write};
      const std::jthread second{write};
    }
    return value == 200000 ? 0 : 1;
  }
  return 2;
}
