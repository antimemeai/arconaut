#include "arconaut/terminal.hpp"
#include "arconaut/tools.hpp"
#include <clocale>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace arconaut;
void check(bool good) {
  if (!good)
    throw std::runtime_error("terminal contract");
}
InputResult feed(Composer &c, std::string_view bytes) {
  InputResult result;
  for (char byte : bytes) {
    auto r = c.feed(byte);
    if (r.action != InputAction::none)
      result = std::move(r);
  }
  return result;
}
int main() {
  try {
    (void)std::setlocale(LC_CTYPE, "en_US.UTF-8");
    Composer c;
    feed(c, "aéz");
    feed(c, "\x1b[D\x7f");
    check(c.text() == "az" && c.cursor() == 1);
    feed(c, "b");
    check(feed(c, "\r").text == "abz");
    feed(c, "draft\x1b[A");
    check(c.text() == "abz");
    feed(c, "\x1b[B");
    check(c.text() == "draft");
    check(feed(c, "\x03").action == InputAction::cancel && c.text() == "draft");
    feed(c, "\x15");
    feed(c, "\x1b[200~one\ntwo\rthree\x1b[201~");
    check(c.text() == "one\ntwo\nthree");
    check(feed(c, "\r").text == "one\ntwo\nthree");
    feed(c, "x\x1b\ry");
    check(c.text() == "x\ny");
    feed(c, "\x15");
    check(feed(c, "\x04").action == InputAction::quit);
    check(terminal_lines("é界x", 3) == std::vector<std::string>{"é界", "x"});
    check(terminal_lines("x\x1b[31m", 20)[0] == "x\\x1b[31m");
    check(terminal_lines("a\nb", 20) == std::vector<std::string>{"a", "b"});
    // Old end=90 (100-10). Append20, trim10: same content end is80,
    // so new110 lines require scroll30, NOT net-growth scroll20.
    check(terminal_scroll_after_output({.scroll = 10,
                                        .previous = 100,
                                        .before_trim = 120,
                                        .after_trim = 110,
                                        .height = 10}) == 30);
    check(terminal_scroll_after_output({.scroll = 0,
                                        .previous = 100,
                                        .before_trim = 120,
                                        .after_trim = 110,
                                        .height = 10}) == 0);
    check(terminal_scroll_after_output({.scroll = 80,
                                        .previous = 100,
                                        .before_trim = 120,
                                        .after_trim = 70,
                                        .height = 10}) == 60);
    check(terminal_scroll_after_output({.scroll = 10,
                                        .previous = 100,
                                        .before_trim = 120,
                                        .after_trim = 120,
                                        .height = 10}) == 30);
    std::string long_line(2 * 1024 * 1024, 'x');
    long_line += "éEND\n";
    trim_terminal_transcript(long_line);
    check(long_line.size() <= 1024 * 1024 && long_line.ends_with("éEND\n") &&
          long_line.starts_with("[Older display trimmed"));
    std::string unicode_line;
    for (std::size_t n = 0; n < 600000; ++n)
      unicode_line += "é";
    trim_terminal_transcript(unicode_line);
    check(unicode_line.size() <= 1024 * 1024 &&
          dump_json(Json{unicode_line}).has_value());
    char path[] = "/tmp/arco-ui-state-XXXXXX";
    const auto dir = mkdtemp(path);
    check(dir != nullptr);
    const auto file = std::filesystem::path{dir} / "ui-state.json";
    TerminalState state{std::string{"x\xff\0", 3}, 1, {"sent"}, {"queued"}};
    save_terminal_state(file, state);
    const auto loaded = load_terminal_state(file);
    check(loaded.draft == state.draft && loaded.cursor == 1 &&
          loaded.history == state.history && loaded.queued == state.queued);
    Composer restored;
    restored.restore(loaded);
    feed(restored, "Z");
    check(restored.text() == std::string{"xZ\xff\0", 4});
    save_terminal_state(file, {"é", 1, {}, {}});
    check(load_terminal_state(file).cursor == 0);
    save_terminal_state(file, state);
    auto too_big = state;
    too_big.queued.resize(129);
    bool rejected = false;
    try {
      save_terminal_state(file, too_big);
    } catch (const Error &e) {
      rejected = e.code == ErrorCode::capacity;
    }
    check(rejected && load_terminal_state(file).queued == state.queued);
    too_big = state;
    too_big.history = {std::string(1024 * 1024, 'a'), "b"};
    rejected = false;
    try {
      save_terminal_state(file, too_big);
    } catch (const Error &e) {
      rejected = e.code == ErrorCode::capacity;
    }
    check(rejected);
    write_file(file, "broken");
    rejected = false;
    try {
      (void)load_terminal_state(file);
    } catch (const Error &) {
      rejected = true;
    }
    check(rejected);
    std::filesystem::remove_all(dir);

  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
