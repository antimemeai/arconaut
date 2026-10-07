#include "blackbird/sprite.hpp"
#include <array>
#include <string_view>

namespace blackbird {
namespace {
// Front three-quarter view traced from the operator's SR-71 reference.
// 24x24 bits occupy 12x6 Braille cells: square in a typical 1:2 terminal font.
constexpr std::array<std::string_view, 24> silhouette{
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........##..............",
    "........##..............",
    "........##..............",
    "......##..#......#......",
    "......#####.##...#......",
    ".....##########.##......",
    ".......######..##.#.....",
    ".....##.#.#....####.....",
    "....#.##.#....#####.....",
    "...######.#####.#####...",
    "..##.##..#..#####.......",
    "..#.##.#####............",
    ".##...#.................",
    "######..................",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................"};
static_assert([] {
  for (const auto row : silhouette)
    if (row.size() != blackbird_sprite_width * 2)
      return false;
  return true;
}());
std::array<std::string, blackbird_sprite_height> encode(unsigned flame) {
  constexpr unsigned dots[4][2]{{0, 3}, {1, 4}, {2, 5}, {6, 7}};
  std::array<std::string, blackbird_sprite_height> result;
  for (std::size_t y = 0; y < blackbird_sprite_height; ++y) {
    result[y].reserve(blackbird_sprite_width * 3);
    for (std::size_t x = 0; x < blackbird_sprite_width; ++x) {
      unsigned mask = 0;
      for (std::size_t dy = 0; dy < 4; ++dy)
        for (std::size_t dx = 0; dx < 2; ++dx) {
          const auto px = x * 2 + dx, py = y * 4 + dy;
          // Both jets trail up/right. No exhaust pixels or timer at idle.
          const bool exhaust = flame &&
                               ((px >= 10 && px < 10 + flame && py == 17 - px) ||
                                (px >= 18 && px < 18 + flame && py == 30 - px));
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
const std::array<std::string, blackbird_sprite_height> &
blackbird_sprite(bool active, std::size_t phase) {
  static const std::array frames{encode(0), encode(2), encode(3), encode(4)};
  return frames[active ? 1 + phase % 3 : 0];
}
} // namespace blackbird

namespace blackbird {
namespace {
constexpr std::array<std::string_view, 5> lettering{
    "####  #      ###   #### #   # ####  ##### ####  ####",
    "#   # #     #   # #     #  #  #   #   #   #   # #   #",
    "####  #     ##### #     ###   ####    #   ####  #   #",
    "#   # #     #   # #     #  #  #   #   #   #  #  #   #",
    "####  ##### #   #  #### #   # ####  ##### #   # ####"};
constexpr std::array<std::string_view, 18> aircraft{
    "                                      .",
    "                    /|            . .",
    "                   / |         . : .",
    "                  /  |      . :+*",
    "             ____/   |__ .:+*",
    "            / ___\\___/  \\                  .",
    "           / /   /   \\   |          /|  . : .",
    "          <_/___/_____/_.'          / |.+*",
    "             |   _.-'  \\          /  |'",
    "             |.-'       \\________/___|__",
    "          _.-'  ______.-'      / ___\\   \\",
    "       .-'  _.-'     /        / /   /\\   |",
    "     .'  .-'  __    /     ___<_/___/  \\_.'\\",
    "    /  .'    /_/   /  _.-'     \\___________\\",
    "   / .'  /\\      .'--'       _.-'",
    "  /.'    \\/   .-'      __.--'",
    " /'________.-'__..----'",
    "<___..-----'"};
} // namespace
std::vector<std::string> blackbird_startup(std::size_t columns, std::size_t rows,
                                          bool mascot) {
  std::vector<std::string> result;
  if (!columns || !rows)
    return result;
  if (columns < 53 || rows < 5) {
    result.emplace_back(std::string_view{"BLACKBIRD"}.substr(0, columns));
    return result;
  }
  // On wide screens the lettering sits alongside the larger aircraft.
  if (mascot && columns >= 101 && rows >= aircraft.size()) {
    result.reserve(aircraft.size());
    for (std::size_t y = 0; y < aircraft.size(); ++y) {
      std::string line{aircraft[y]};
      if (y >= 5 && y < 5 + lettering.size()) {
        line.resize(48, ' ');
        line += lettering[y - 5];
      }
      result.push_back(std::move(line));
    }
  } else {
    for (const auto line : lettering)
      result.emplace_back(line);
    if (mascot && rows >= lettering.size() + 1 + aircraft.size()) {
      result.emplace_back();
      for (const auto line : aircraft)
        result.emplace_back(line);
    }
  }
  return result;
}
} // namespace blackbird
