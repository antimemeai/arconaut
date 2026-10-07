#pragma once
#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace blackbird {
inline constexpr std::size_t blackbird_sprite_width = 12;
inline constexpr std::size_t blackbird_sprite_height = 6;
// Cached square SR-71 avatar, facing down-left; live state supplies its ink.
const std::array<std::string, blackbird_sprite_height> &
blackbird_sprite(bool active, std::size_t phase);
// Local startup art only; never added to provider context or the audit.
std::vector<std::string> blackbird_startup(std::size_t columns, std::size_t rows,
                                         bool mascot = true);
} // namespace blackbird
