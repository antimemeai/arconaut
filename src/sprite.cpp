#include "blackbird/sprite.hpp"
#include <array>
#include <string_view>

namespace blackbird {
namespace {
// Nose right; swept fin, long chines, delta wing and twin-engine exhaust left.
// One bit per pixel, 36x8. No image decoder, texture or terminal graphics protocol.
constexpr std::array<std::string_view, 8> silhouette{
    "....................................", "..........#.........................",
    ".........##.........................", "........###.........................",
    "......######################........", ".....###############################",
    "......########################......", "..........############.............."};
static_assert([] {
  for (const auto row : silhouette)
    if (row.size() != blackbird_sprite_width * 2)
      return false;
  return true;
}());
std::array<std::string, 2> encode(unsigned flame) {
  constexpr unsigned dots[4][2]{{0, 3}, {1, 4}, {2, 5}, {6, 7}};
  std::array<std::string, 2> result;
  for (std::size_t y = 0; y < 2; ++y) {
    result[y].reserve(blackbird_sprite_width * 3);
    for (std::size_t x = 0; x < blackbird_sprite_width; ++x) {
      unsigned mask = 0;
      for (std::size_t dy = 0; dy < 4; ++dy)
        for (std::size_t dx = 0; dx < 2; ++dx) {
          const auto px = x * 2 + dx, py = y * 4 + dy;
          const bool exhaust = flame && py >= 4 && py <= 6 && px < 5 &&
                               px >= 5 - flame && (py == 5 || px % 2 == 0);
          if (silhouette[py][px] == '#' || exhaust)
            mask |= 1U << dots[dy][dx];
        }
      if (!mask)
        result[y] += ' ';
      else {
        const auto scalar = 0x2800U + mask;
        result[y] += static_cast<char>(0xe0U | (scalar >> 12));
        result[y] += static_cast<char>(0x80U | ((scalar >> 6) & 63));
        result[y] += static_cast<char>(0x80U | (scalar & 63));
      }
    }
  }
  return result;
}
} // namespace
const std::array<std::string, 2> &blackbird_sprite(bool active, std::size_t phase) {
  static const std::array frames{encode(0), encode(2), encode(3), encode(4)};
  return frames[active ? 1 + phase % 3 : 0];
}
} // namespace blackbird
