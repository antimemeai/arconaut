#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
namespace arconaut::detail {
// Exact 10px grid from the operator's arconaut.png: transparent, sea-green, white.
inline constexpr std::array<std::string_view, 24> arconaut_pixels{
    "................", "................", "................", ".....sssss......",
    "......sssss.....", "...*..s.........", "....*.s.........", "...***.s..s.....",
    "..***........s..", ".*.*.......s.ss.", "............sss.", "..*.*........s..",
    ".***......*.....", "..*.ss.***......", "...ss.....s.....", ".....sssssss....",
    "....sss..ssss...", "...sss....ss....", "....ss....ss....", "...ss....sss....",
    "..sss.....sss...", "..sss...........", "................", "................"};
inline bool mascot_visible(unsigned short rows, unsigned short columns) {
  return rows >= 20 && columns >= 90;
}
inline std::vector<std::string> arconaut_sprite(bool busy, std::uint64_t phase) {
  const bool bob = busy && phase % 2 != 0;
  const auto pixel = [bob](std::size_t x, std::size_t y) {
    if (bob) {
      if (y == 0)
        return '.';
      --y;
    }
    return arconaut_pixels[y][x];
  };
  const auto color = [bob](char value, bool background) {
    std::string out = background ? "\x1b[48;2;" : "\x1b[38;2;";
    out += value == 's' ? "177;201;195m" : bob ? "215;235;230m" : "255;255;255m";
    return out;
  };
  std::vector<std::string> rows;
  for (std::size_t y = 0; y < arconaut_pixels.size(); y += 2) {
    std::string row;
    for (std::size_t x = 0; x < 16; ++x) {
      const auto top = pixel(x, y), bottom = pixel(x, y + 1);
      row += "\x1b[0m";
      if (top == '.' && bottom == '.')
        row += ' ';
      else if (top == '.')
        row += color(bottom, false) + "▄";
      else {
        row += color(top, false);
        if (bottom != '.')
          row += color(bottom, true);
        row += "▀";
      }
    }
    rows.push_back(row + "\x1b[0m");
  }
  return rows;
}
} // namespace arconaut::detail
