#include "blackbird/chat_view.hpp"
#include "blackbird/sprite.hpp"
#include <chrono>
#include <clocale>
#include <csignal>
#include <iostream>
#include <string_view>

namespace {
volatile std::sig_atomic_t stopping = 0;
void stop(int) { stopping = 1; }
} // namespace
int main(int argc, char **argv) {
  using namespace blackbird;
  try {
    const int seconds = argc > 1 ? std::stoi(argv[1]) : 10;
    const std::string_view scenario = argc > 2 ? argv[2] : "stream";
    if (seconds < 1 || seconds > 300 || (scenario != "stream" && scenario != "motion"))
      throw std::runtime_error("renderer_profile SECONDS(1..300) [stream|motion]");
    (void)std::setlocale(LC_CTYPE, "");
    std::signal(SIGTERM, stop);
    std::signal(SIGINT, stop);
    ChatView view;
    view.begin(ChatKind::assistant);
    view.stream("# Native rendering workload\n\n```cpp\nconst auto frame = "
                "compose();\nreturn paint(frame);\n```\n");
    ChatGrid grid;
    ChatPainter painter;
    std::size_t frames = 0, bytes = 0;
    const auto started = std::chrono::steady_clock::now();
    while (!stopping &&
           std::chrono::steady_clock::now() - started < std::chrono::seconds{seconds}) {
      if (scenario == "stream" && frames % 8 == 0)
        view.stream("Retained context · 界 · é · 👩‍💻 · " +
                    std::to_string(frames) + "\n");
      const auto &rows = view.rows(120);
      grid.reset(120, 40);
      grid.line(0, {{"Blackbird · renderer workload", Ink::assistant}});
      grid.line(1, {{"Frame " + std::to_string(frames), Ink::muted}});
      const auto &sprite = blackbird_sprite(true, frames);
      for (std::size_t y = 0; y < sprite.size(); ++y)
        grid.line(y, {{sprite[y], Ink::assistant}}, 100);
      const auto begin = rows.size() > 34 ? rows.size() - 34 : 0;
      for (std::size_t row = begin; row < rows.size(); ++row)
        grid.line(row - begin + 2, rows[row]);
      grid.line(37, {{"╭─ native fixture ─────────────────────╮", Ink::border}});
      grid.line(38, {{"│ >                                   │", Ink::border}});
      grid.line(39, {{"╰─────────────────────────────────────╯", Ink::border}});
      bytes += painter.prepare(grid, 38, 4).size();
      painter.commit(grid);
      ++frames;
    }
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                             std::chrono::steady_clock::now() - started)
                             .count();
    std::cout
        << "{\"scenario\":\"" << scenario << "\",\"frames\":" << frames
        << ",\"packet_bytes\":" << bytes << ",\"layout_reflows\":" << view.reflows()
        << ",\"elapsed_us\":" << elapsed
        << ",\"scope\":\"native layout/composition/paint; no terminal or provider\"}\n";
    return frames ? 0 : 1;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 2;
  }
}
