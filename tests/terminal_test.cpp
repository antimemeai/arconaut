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
    Composer completion;
    feed(completion, "/hel\t");
    check(completion.text() == "/help" && completion.cursor() == 5);
    feed(completion, "\x15/mod\t");
    check(completion.text() == "/model ");
    feed(completion, "gpt-example\t");
    check(completion.text() == "/model gpt-example");
    feed(completion, "\x15/dra\t");
    check(completion.text() == "/draft");
    feed(completion, "\t");
    check(completion.text() == "/draft ");
    feed(completion, "\x15/co\t");
    check(completion.text() == "/co"); // Ambiguous: never guess a command.
    feed(completion, "\x15/unknown\t");
    check(completion.text() == "/unknown");
    feed(completion, "\x15/hel\x1b[D\t");
    check(completion.text() == "/hel" && completion.cursor() == 3);
    feed(completion, "\x15\x1b[200~/hel\t\x1b[201~");
    check(completion.text() == "/hel\t"); // Paste is literal, never completion.
    check(terminal_command_hint("/co").find("/compact") != std::string::npos);
    check(terminal_command_hint("/co").find("/context") != std::string::npos);
    check(terminal_command_hint("/model foo").find("NAME") != std::string::npos);
    check(terminal_command_hint("/unknown").starts_with("Unknown command"));
    check(terminal_command_hint("ordinary draft").empty());
    check(terminal_command_hint("/lua\ncode").empty());
    check(terminal_help().find("/sessions") != std::string::npos);
    check(terminal_help().find("not model context") != std::string::npos);
    check(terminal_key_help().find("keep unsent draft") != std::string::npos);
    Composer multiline;
    feed(multiline, "history\r");
    multiline.draft("abcd\nx\n12345");
    feed(multiline, "\x1b[A");
    check(multiline.text() == "abcd\nx\n12345" && multiline.cursor() == 6);
    feed(multiline, "\x1b[A");
    check(multiline.cursor() == 4); // Sticky column survives the short middle line.
    feed(multiline, "\x1b[B\x1b[B");
    check(multiline.cursor() == 12);
    feed(multiline, "\x1b[1;5H");
    check(multiline.cursor() == 0);
    feed(multiline, "\x10"); // Explicit history, even for multiline drafts.
    check(multiline.text() == "history");
    feed(multiline, "\x0e");
    check(multiline.text() == "abcd\nx\n12345" && multiline.cursor() == 0);
    feed(multiline, "\x1b[B\x05");
    check(multiline.cursor() == 6); // Ctrl-E ends this line, not the whole draft.
    feed(multiline, "\x01");
    check(multiline.cursor() == 5);
    Composer words;
    words.draft("one é界 two");
    feed(words, "\x1b"
                "b");
    check(words.cursor() == 10);
    feed(words, "\x17");
    check(words.text() == "one two" && words.cursor() == 4);
    feed(words, "\x19");
    check(words.text() == "one é界 two" && words.cursor() == 10);
    feed(words, "\x1b[1;5H\x1b[1;5C");
    check(words.cursor() == 3);
    feed(words, "\x1b"
                "d");
    check(words.text() == "one two");
    words.draft("first\nsecond");
    feed(words, "\x1b[1;5H\x05\x0b"); // At line end, kill the newline.
    check(words.text() == "firstsecond");
    feed(words, "\x19");
    check(words.text() == "first\nsecond");
    Composer columns;
    columns.draft("a界z\nx\n1234");
    feed(columns, "\x1b[A\x1b[A");
    check(columns.cursor() == 5); // Four terminal cells, not four UTF-8 bytes.
    feed(columns, "\x1b[D\x1b[B\x1b[B");
    check(columns.cursor() ==
          columns.text().size() - 1); // Horizontal motion resets goal.
    columns.draft("a\n\nb");
    feed(columns, "\x1b[A");
    check(columns.cursor() == 2);
    feed(columns, "\x1b[A");
    check(columns.cursor() == 1);
    columns.draft("abc\ndef");
    feed(columns, "\x01\x1b[A");
    check(columns.cursor() == 0);
    feed(columns, "\x0b");
    check(columns.text() == "\ndef");
    feed(columns, "\x19");
    check(columns.text() == "abc\ndef" && columns.cursor() == 3);
    feed(columns, "\x15\x19");
    check(columns.text() == "abc\ndef");
    columns.draft("界\nx");
    feed(columns, "\x1b[A");
    check(columns.cursor() == 0); // Can't place the cursor inside a wide glyph.
    feed(columns, "\x1b[B");
    check(columns.cursor() == columns.text().size());
    columns.draft("a\tb\n123");
    feed(columns, "\x1b[A");
    check(columns.cursor() == 2); // Pasted tab has the renderer's two-cell width.
    columns.draft("éx\nab");
    feed(columns, "\x1b[A");
    check(columns.cursor() == 4); // Combining mark contributes no extra cell.
    columns.draft(std::string{"\xff\nabcd", 6});
    feed(columns, "\x1b[A");
    check(columns.cursor() == 1); // Invalid byte is displayed as four escaped cells.
    columns.restore({"", 0, {}, {}});
    feed(columns, "\x19");
    check(columns.text().empty()); // Restore does not inherit a prior kill buffer.
    Composer full;
    full.draft(std::string(1024 * 1024, 'a'));
    feed(full, "\x17");
    check(full.text().empty());
    full.draft("b");
    feed(full, "\x19");
    check(full.text() == "b" && full.cursor() == 1); // Oversized yank is atomic/no-op.
    Composer literal;
    feed(literal, "\x1b[200~a\x17\x0b\x19\x10\x0e\x1b[201~");
    check(literal.text() == "a\x17\x0b\x19\x10\x0e");
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
