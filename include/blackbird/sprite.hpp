#pragma once
#include <array>
#include <cstddef>
#include <string>

namespace blackbird {
inline constexpr std::size_t blackbird_sprite_width = 18;
// Original SR-71 side silhouette, two existing header rows. Cached UTF-8 frames.
const std::array<std::string, 2> &blackbird_sprite(bool active, std::size_t phase);
} // namespace blackbird
