#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
  if (size > 4096) {
    return 0;
  }
  std::vector<std::uint8_t> copy;
  if (size != 0) {
    copy.assign(data, data + size);
  }
  std::reverse(copy.begin(), copy.end());
  std::reverse(copy.begin(), copy.end());
  if (copy.size() != size ||
      (size != 0 && !std::equal(copy.begin(), copy.end(), data))) {
    std::abort();
  }
  return 0;
}
