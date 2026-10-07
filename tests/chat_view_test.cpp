#include "arconaut/chat_view.hpp"
#include <cerrno>
#include <chrono>
#include <clocale>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace arconaut;
void require(bool yes, const char *why) {
  if (!yes)
    throw std::runtime_error(why);
}
std::string plain(const std::vector<ChatRow> &rows) {
  std::string s;
  for (const auto &r : rows)
    s += chat_plain(r) + "\n";
  return s;
}
std::string hex(std::string_view bytes) {
  constexpr char digits[] = "0123456789abcdef";
  std::string result;
  for (const char byte : bytes) {
    const auto c = static_cast<unsigned char>(byte);
    result += digits[c >> 4];
    result += digits[c & 15];
  }
  return result;
}
int main(int argc, char **) {
  try {
    (void)std::setlocale(LC_CTYPE, "en_US.UTF-8");
    if (argc > 1) {
      ChatPainter delta;
      ChatGrid frame;
      for (int i = 0; i < 6; ++i) {
        frame.reset(i == 4 ? 12 : 20, 4);
        frame.line(0, {{i == 0   ? "hello long"
                        : i == 1 ? "界éx"
                        : i == 2 ? "h"
                                 : "👩‍💻🇺🇸",
                        Ink::assistant}});
        frame.line(1, {{"keyword", Ink::keyword}, {" string", Ink::string}});
        ChatPainter full;
        const auto packet = delta.prepare(frame, 3, 2);
        std::cout << frame.width << " " << frame.height << "\n"
                  << hex(packet) << "\n"
                  << hex(full.prepare(frame, 3, 2)) << "\n";
        delta.commit(frame);
      }
      return 0;
    }
    ChatView view;
    view.append(ChatKind::user, "hola");
    view.begin(ChatKind::assistant);
    view.stream("¡Hola! ¿En qué te ayudo?");
    auto text = plain(view.rows(80));
    require(text.find("YOU") != std::string::npos &&
                text.find("ARCO") != std::string::npos,
            "typed roles missing");
    require(text.find("JSON bytes") == std::string::npos, "diagnostics leak");
    const auto count = view.reflows();
    for (int i = 0; i < 100; ++i)
      (void)view.rows(80);
    require(view.reflows() == count, "idle/cache causes reflow");
    ChatView stream, cold;
    std::string markdown =
        "# Heading\n\n**bold** and `code`\n```cpp\nauto value = \"yes\";\n```\nDone.\n";
    stream.begin(ChatKind::assistant);
    for (char c : markdown) {
      stream.stream(std::string_view{&c, 1});
      cold.clear();
      cold.append(ChatKind::assistant, markdown.substr(0, stream.size()));
      const auto &a = stream.rows(40), &b = cold.rows(40);
      require(plain(a) == plain(b), "stream/cold diverge");
      require(a.size() == b.size(), "styled row count differs");
      for (std::size_t row = 0; row < a.size(); ++row) {
        require(a[row].size() == b[row].size(), "styled span count differs");
        for (std::size_t span = 0; span < a[row].size(); ++span)
          require(a[row][span].text == b[row][span].text &&
                      a[row][span].ink == b[row][span].ink,
                  "stream/cold styles diverge");
      }
    }
    ChatGrid grid;
    grid.reset(20, 4);
    grid.line(0, {{"hello", Ink::assistant}});
    ChatPainter painter;
    auto initial = painter.prepare(grid, 3, 2);
    require(!initial.empty(), "first paint missing");
    painter.commit(grid);
    require(painter.prepare(grid, 3, 2).empty(), "unchanged frame emitted");
    grid.line(0, {{"j", Ink::assistant}});
    auto delta = painter.prepare(grid, 3, 2);
    require(delta.find("hello") == std::string::npos && painter.touched() == 1,
            "damage not bounded");
    painter.commit(grid);
    require(painter.prepare(grid, 3, 2).empty(), "committed frame repeats");
    grid.reset(20, 4);
    grid.line(0, {{"界x", Ink::normal}});
    require(grid.cells[0].width == 2 && grid.cells[1].width == 0 &&
                grid.cells[2].scalar == U'x',
            "CJK cells");
    grid.reset(20, 4);
    grid.line(0, {{"éx", Ink::normal}});
    require(grid.cells[0].length == 2 && grid.cells[1].scalar == U'x',
            "combining cluster");
    grid.reset(20, 4);
    grid.line(0, {{"👩‍💻x", Ink::normal}});
    require(grid.cells[0].width == 2 && grid.cells[2].scalar == U'x', "ZWJ cluster");
    grid.reset(20, 4);
    grid.line(0, {{"🇺🇸x", Ink::normal}});
    require(grid.cells[0].width == 2 && grid.cells[2].scalar == U'x', "flag cluster");
    painter.prepare(grid, 3, 2);
    painter.commit(grid);
    grid.reset(10, 3);
    require(!painter.prepare(grid, 2, 0).empty(), "resize did not repaint");
    ChatView control;
    control.append(ChatKind::assistant, "evil\x1b[2J");
    require(plain(control.rows(80)).find('\x1b') == std::string::npos,
            "untrusted control escaped renderer");
    ChatView large;
    large.append(ChatKind::tool, std::string(2 * 1024 * 1024, 'a'));
    require(large.size() <= 1024 * 1024, "display source unbounded");
    ChatView narrow;
    narrow.append(ChatKind::assistant, "aa👩‍💻x\n```cpp\n// 界\n```");
    const auto narrow_text = plain(narrow.rows(8));
    require(narrow_text.find("👩‍💻") != std::string::npos &&
                narrow_text.find("界") != std::string::npos,
            "wrapping splits Unicode clusters");
    grid.reset(20, 4);
    grid.line(0, {{"é", Ink::normal}});
    grid.line(0, {{"h", Ink::normal}});
    require(grid.cells[0].length == 0, "overwrite preserves old cluster tail");
    std::string cluster = "e";
    for (int i = 0; i < 3000; ++i)
      cluster += "́";
    grid.line(0, {{cluster, Ink::normal}});
    require(grid.cells[0].scalar == U'�', "oversized cluster has no bounded fallback");
    ChatView newlines;
    newlines.append(ChatKind::tool, std::string(1024 * 1024, '\n'));
    require(newlines.rows(8).size() < 4096, "derived rows unbounded");
    ChatView unfinished;
    unfinished.begin(ChatKind::tool);
    for (int i = 0; i < 256; ++i) {
      unfinished.stream(std::string(1024, 'a'));
      require(unfinished.rows(80).size() < 64, "unfinished line work unbounded");
    }
    int descriptors[2];
    require(::pipe(descriptors) == 0, "pipe setup");
    (void)::fcntl(descriptors[0], F_SETFL, O_NONBLOCK);
    std::string payload(256 * 1024, 'q'), received;
    {
      ChatOutput output;
      output.attach(descriptors[1]);
      output.start(payload);
      require(!output.flush() && output.pending(),
              "backpressure did not remain pending");
      for (unsigned round = 0;
           round < 1000 && (output.pending() || received.size() < payload.size());
           ++round) {
        char data[16384];
        const auto n = ::read(descriptors[0], data, sizeof(data));
        if (n > 0)
          received.append(data, static_cast<std::size_t>(n));
        if (output.pending())
          (void)output.flush();
      }
      require(!output.pending() && received == payload,
              "partial writes corrupted bytes");
    }
    require((::fcntl(descriptors[1], F_GETFL) & O_NONBLOCK) == 0,
            "output flags not restored");
    ::close(descriptors[0]);
    ::close(descriptors[1]);
    const auto start = std::chrono::steady_clock::now();
    std::size_t bytes = 0;
    ChatGrid motion;
    motion.reset(120, 40);
    ChatPainter paint;
    motion.line(0, {{"activity 0", Ink::assistant}});
    paint.prepare(motion, 38, 3);
    paint.commit(motion);
    for (int i = 0; i < 1000; ++i) {
      motion.line(0, {{"activity " + std::to_string(i % 10), Ink::assistant}});
      bytes += paint.prepare(motion, 38, 3).size();
      paint.commit(motion);
    }
    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();
    require(bytes < 100000, "motion writes excessive bytes");
    std::cout << "1000 120x40 motion updates: " << us << "us, " << bytes
              << " bytes; sizeof(cell)=" << sizeof(ChatCell) << "\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
