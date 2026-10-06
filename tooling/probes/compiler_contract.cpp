#include <array>
#include <cstddef>
#include <mutex>
#include <span>
#include <thread>

static_assert(__cplusplus >= 202002L);

int main() {
  const std::array<unsigned char, 4> bytes{0, 255, 13, 10};
  const std::span<const unsigned char> view{bytes};
  if (view.size() != 4 || view[0] != 0 || view[1] != 255) {
    return 1;
  }
  std::mutex mutex;
  std::size_t count = 0;
  auto increment = [&]() {
    for (int i = 0; i < 1000; ++i) {
      const std::lock_guard lock{mutex};
      ++count;
    }
  };
  {
    const std::jthread first{increment};
    const std::jthread second{increment};
  }
  return count == 2000 ? 0 : 2;
}
