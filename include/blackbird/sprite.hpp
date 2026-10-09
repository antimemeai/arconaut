#pragma once
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace blackbird {
inline constexpr std::size_t blackbird_sprite_width = 12;
inline constexpr std::size_t blackbird_sprite_height = 6;
// Cached square SR-71 avatar, facing down-left; live state supplies its ink.
const std::array<std::string, blackbird_sprite_height> &
blackbird_sprite(bool active, std::size_t phase);
inline constexpr std::size_t blackbird_lettering_width = 67;
inline constexpr std::size_t blackbird_lettering_height = 6;
// Local startup art only; never added to provider context or the audit.
std::vector<std::string> blackbird_startup(std::size_t columns, std::size_t rows,
                                           bool mascot = true);
} // namespace blackbird

namespace blackbird {
// Local-file Kitty graphics; the terminal owns decoding and cached pixel storage.
bool blackbird_graphics_available();
std::string blackbird_graphics_load();
std::string blackbird_graphics_place(std::size_t row, std::size_t column,
                                     std::size_t columns, std::size_t rows);
std::string_view blackbird_graphics_erase(bool release = false);
} // namespace blackbird
