#include "arconaut/terminal.hpp"
#include "arconaut/tools.hpp"
#include "arconaut_sprite.hpp"
#include <algorithm>
#include <cerrno>
#include <charconv>
#include <chrono>
#include <clocale>
#include <cwchar>
#include <iostream>
#include <poll.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
namespace arconaut {
namespace {
bool continuation(char byte) {
  return (static_cast<unsigned char>(byte) & 0xc0U) == 0x80U;
}
std::size_t previous(std::string_view s, std::size_t cursor) {
  if (cursor == 0)
    return 0;
  --cursor;
  while (cursor > 0 && continuation(s[cursor]))
    --cursor;
  return cursor;
}
std::size_t next(std::string_view s, std::size_t cursor) {
  if (cursor < s.size())
    ++cursor;
  while (cursor < s.size() && continuation(s[cursor]))
    ++cursor;
  return cursor;
}
class TerminalMode {
public:
  TerminalMode() {
    if (tcgetattr(STDIN_FILENO, &before_) != 0)
      throw std::runtime_error("Cannot read terminal mode");
    auto raw = before_;
    raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO | IEXTEN | ISIG));
    raw.c_iflag &= static_cast<tcflag_t>(~(IXON | ICRNL));
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0)
      throw std::runtime_error("Cannot set terminal mode");
    active_ = true;
    std::cout << "\x1b[?1049h\x1b[?2004h" << std::flush;
  }
  ~TerminalMode() {
    if (active_) {
      std::cout << "\x1b[0m\x1b[?2004l\x1b[?1049l\x1b[?25h" << std::flush;
      (void)tcsetattr(STDIN_FILENO, TCSANOW, &before_);
    }
  }

private:
  termios before_{};
  bool active_ = false;
};
} // namespace
void Composer::insert(char byte) {
  if (text_.size() >= 1024 * 1024)
    return;
  text_.insert(cursor_, 1, byte);
  ++cursor_;
}
namespace {
constexpr std::size_t ui_limit = 1024 * 1024;
void validate_state(const TerminalState &state) {
  if (state.draft.size() > ui_limit || state.cursor > state.draft.size())
    throw Error{ErrorCode::capacity};
  for (const auto *list : {&state.history, &state.queued}) {
    if (list->size() > 128)
      throw Error{ErrorCode::capacity};
    std::size_t size = 0;
    for (const auto &value : *list) {
      if (value.size() > ui_limit - size)
        throw Error{ErrorCode::capacity};
      size += value.size();
    }
  }
}
std::string ui_hex(std::string_view value, bool decode = false) {
  constexpr char digits[] = "0123456789abcdef";
  std::string out;
  if (!decode) {
    for (const char byte : value) {
      const auto c = static_cast<unsigned char>(byte);
      out += digits[c >> 4U];
      out += digits[c & 15U];
    }
  } else {
    if (value.size() % 2 != 0)
      throw Error{ErrorCode::corrupt};
    for (std::size_t i = 0; i < value.size(); i += 2) {
      unsigned n = 0;
      auto r = std::from_chars(value.data() + i, value.data() + i + 2, n, 16);
      if (r.ec != std::errc{} || r.ptr != value.data() + i + 2)
        throw Error{ErrorCode::corrupt};
      out += static_cast<char>(n);
    }
  }
  return out;
}
void trim_history(std::vector<std::string> &values) {
  std::size_t size = 0;
  for (const auto &value : values)
    size += value.size();
  while (values.size() > 128 || size > ui_limit) {
    size -= values.front().size();
    values.erase(values.begin());
  }
}
} // namespace
std::size_t terminal_scroll_after_output(const TerminalScrollUpdate &update) {
  if (update.scroll == 0)
    return 0;
  const auto growth =
      update.before_trim > update.previous ? update.before_trim - update.previous : 0;
  const auto maximum =
      update.after_trim > update.height ? update.after_trim - update.height : 0;
  if (update.scroll >= maximum || growth >= maximum - update.scroll)
    return maximum;
  return update.scroll + growth;
}
void trim_terminal_transcript(std::string &text) {
  if (text.size() > ui_limit) {
    constexpr std::string_view notice =
        "[Older display trimmed; full originals remain audited]\n";
    const auto minimum = text.size() - ui_limit + notice.size();
    const auto lf = text.find('\n', minimum);
    auto cut =
        lf == std::string::npos || lf - minimum > ui_limit / 8 ? minimum : lf + 1;
    while (cut < text.size() && continuation(text[cut]))
      ++cut;
    text.erase(0, cut);
    text.insert(0, notice);
  }
}
TerminalState load_terminal_state(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path))
    return {};
  const auto packet = unwrap(parse_json(read_file(path, 7 * ui_limit)));
  if (string_field(packet, "encoding") != "hex-v1")
    throw Error{ErrorCode::corrupt};
  TerminalState state;
  state.draft = ui_hex(string_field(packet, "draft"), true);
  const auto &cursor = field(packet, "cursor").number().text;
  auto r = std::from_chars(cursor.data(), cursor.data() + cursor.size(), state.cursor);
  if (r.ec != std::errc{} || r.ptr != cursor.data() + cursor.size())
    throw Error{ErrorCode::corrupt};
  for (const auto &[key, list] :
       {std::pair{"history", &state.history}, std::pair{"queued", &state.queued}})
    for (const auto &value : field(packet, key).array())
      list->push_back(ui_hex(value.string(), true));
  validate_state(state);
  while (state.cursor > 0 && state.cursor < state.draft.size() &&
         continuation(state.draft[state.cursor]))
    --state.cursor;
  return state;
}
void save_terminal_state(const std::filesystem::path &path,
                         const TerminalState &state) {
  validate_state(state);
  auto encode = [](const auto &values) {
    Json::Array out;
    for (const auto &v : values)
      out.emplace_back(ui_hex(v));
    return Json{std::move(out)};
  };
  write_file(path, unwrap(dump_json(Json::object(
                       {{"encoding", Json{"hex-v1"}},
                        {"draft", Json{ui_hex(state.draft)}},
                        {"cursor", Json{JsonNumber{std::to_string(state.cursor)}}},
                        {"history", encode(state.history)},
                        {"queued", encode(state.queued)}}))));
}
void Composer::restore(const TerminalState &state) {
  validate_state(state);
  text_ = state.draft;
  cursor_ = state.cursor;
  history_ = state.history;
  history_index_ = history_.size();
}
TerminalState Composer::state() const { return {text_, cursor_, history_, {}}; }
void Composer::draft(std::string text) {
  if (text.size() > ui_limit)
    throw Error{ErrorCode::capacity};
  text_ = std::move(text);
  cursor_ = text_.size();
  history_index_ = history_.size();
  draft_.clear();
}
InputResult Composer::feed(char byte) {
  const auto c = static_cast<unsigned char>(byte);
  if (!escape_.empty()) {
    escape_ += byte;
    if (escape_ == "\x1b[200~") {
      pasted_ = true;
      escape_.clear();
      return {};
    }
    if (escape_ == "\x1b[201~") {
      pasted_ = false;
      escape_.clear();
      return {};
    }
    if (pasted_) {
      if (std::string_view{"\x1b[201~"}.starts_with(escape_))
        return {};
      for (const char v : escape_)
        insert(v);
      escape_.clear();
      return {};
    }
    if (escape_ == "\x1b\r" || escape_ == "\x1b\n") {
      insert('\n');
      escape_.clear();
      return {};
    }
    if (escape_.size() == 2 && (byte == '[' || byte == 'O'))
      return {};
    if (escape_.size() >= 3 && c >= 0x30U && c <= 0x3fU)
      return {};
    if (escape_ == "\x1b[D")
      cursor_ = previous(text_, cursor_);
    if (escape_ == "\x1b[C")
      cursor_ = next(text_, cursor_);
    if (escape_ == "\x1b[H" || escape_ == "\x1b[1~")
      cursor_ = 0;
    if (escape_ == "\x1b[F" || escape_ == "\x1b[4~")
      cursor_ = text_.size();
    if (escape_ == "\x1b[3~" && cursor_ < text_.size())
      text_.erase(cursor_, next(text_, cursor_) - cursor_);
    if (escape_ == "\x1b[A" && history_index_ > 0) {
      if (history_index_ == history_.size())
        draft_ = text_;
      text_ = history_[--history_index_];
      cursor_ = text_.size();
    }
    if (escape_ == "\x1b[B" && history_index_ < history_.size()) {
      ++history_index_;
      text_ = history_index_ == history_.size() ? draft_ : history_[history_index_];
      cursor_ = text_.size();
    }
    const auto action = escape_ == "\x1b[5~"   ? InputAction::page_up
                        : escape_ == "\x1b[6~" ? InputAction::page_down
                                               : InputAction::none;
    escape_.clear();
    return {action, {}};
  }
  if (c == 27) {
    escape_ = byte;
    return {};
  }
  if (pasted_) {
    insert(byte == '\r' ? '\n' : byte);
    return {};
  }
  if (c == 3) {
    return {InputAction::cancel, {}};
  }
  if (c == 17 || (c == 4 && text_.empty()))
    return {InputAction::quit, {}};
  if (c == 13) {
    auto result = text_;
    text_.clear();
    cursor_ = 0;
    if (!result.empty() && result != "/quit" && result != "/exit") {
      history_.push_back(result);
      trim_history(history_);
    }
    history_index_ = history_.size();
    draft_.clear();
    return {InputAction::submit, std::move(result)};
  }
  if (c == 10) {
    insert('\n');
    return {};
  }
  if (c == 1) {
    cursor_ = 0;
    return {};
  }
  if (c == 5) {
    cursor_ = text_.size();
    return {};
  }
  if (c == 21) {
    text_.clear();
    cursor_ = 0;
    return {};
  }
  if (c == 127 || c == 8) {
    const auto p = previous(text_, cursor_);
    text_.erase(p, cursor_ - p);
    cursor_ = p;
    return {};
  }
  if (c == 4 && cursor_ < text_.size()) {
    text_.erase(cursor_, next(text_, cursor_) - cursor_);
    return {};
  }
  if (c >= 32 && text_.size() < 1024 * 1024)
    insert(byte);
  return {};
}
std::vector<std::string> terminal_lines(std::string_view text, std::size_t columns) {
  columns = std::max<std::size_t>(columns, 1);
  std::vector<std::string> lines(1);
  std::size_t width = 0;
  auto add = [&](std::string_view value, std::size_t cells) {
    if (width != 0 && width + cells > columns) {
      lines.emplace_back();
      width = 0;
    }
    lines.back().append(value);
    width += cells;
  };
  while (!text.empty()) {
    const auto c = static_cast<unsigned char>(text.front());
    if (c == '\n') {
      lines.emplace_back();
      width = 0;
      text.remove_prefix(1);
      continue;
    }
    if (c == '\t') {
      add("  ", 2);
      text.remove_prefix(1);
      continue;
    }
    wchar_t wide{};
    std::mbstate_t state{};
    auto count = std::mbrtowc(&wide, text.data(), text.size(), &state);
    const auto cells = count != static_cast<std::size_t>(-1) &&
                               count != static_cast<std::size_t>(-2) && count != 0
                           ? ::wcwidth(wide)
                           : -1;
    if (c < 32 || c == 127 || cells < 0) {
      constexpr char hex[] = "0123456789abcdef";
      const char escaped[] = {'\\', 'x', hex[c >> 4U], hex[c & 15U]};
      for (const auto value : escaped)
        add(std::string_view{&value, 1}, 1);
      count = 1;
    } else
      add(text.substr(0, count), static_cast<std::size_t>(cells));
    text.remove_prefix(count);
  }
  return lines;
}
void TerminalUI::post(Kind kind, std::string_view text) {
  const std::lock_guard lock{mutex_};
  messages_.push_back({kind, std::string{text}});
}
void TerminalUI::text(std::string_view value) { post(Kind::text, value); }
void TerminalUI::status(std::string_view value) { post(Kind::status, value); }
void TerminalUI::operation_completed(std::string_view value) {
  post(Kind::operation_complete, value);
}
void TerminalUI::failed() { post(Kind::failure, {}); }
void TerminalUI::title(std::string_view value) { post(Kind::title, value); }
void TerminalUI::run(const std::function<void(std::string_view)> &perform,
                     std::atomic_bool &cancelled,
                     const std::function<bool()> &exit_requested, std::string initial,
                     std::filesystem::path state_path) {
  (void)std::setlocale(LC_CTYPE, "");
  TerminalMode mode;
  Composer composer;
  std::jthread worker;
  struct StopOnExit {
    std::atomic_bool &flag;
    ~StopOnExit() { flag.store(true); }
  } stop{cancelled};
  std::vector<std::string> queued, recovered;
  if (!state_path.empty()) {
    try {
      const auto saved = load_terminal_state(state_path);
      composer.restore(saved);
      recovered = saved.queued;
    } catch (...) {
      text("UI state damaged/unreadable; not restored. Audit untouched.\n");
    }
  }
  if (!recovered.empty())
    text("Recovered drafts: " + std::to_string(recovered.size()) +
         " · /drafts lists; /draft N loads without submitting.\n");
  bool save_failed = false;
  auto persist = [&] {
    if (state_path.empty())
      return true;
    auto saved = composer.state();
    saved.queued = recovered;
    saved.queued.insert(saved.queued.end(), queued.begin(), queued.end());
    try {
      save_terminal_state(state_path, saved);
      save_failed = false;
      return true;
    } catch (...) {
      if (!save_failed)
        text("UI state save failed; unsaved drafts retained in RAM. New work waits for "
             "a successful save.\n");
      save_failed = true;
      return false;
    }
  };

  std::string transcript, activity = "Ready";
  bool busy = false, quitting = false, redraw = true, tool_active = false,
       turn_failed = false;
  std::size_t scroll = 0, previous_lines = 0;
  unsigned short old_rows = 0, old_columns = 0;
  auto started = std::chrono::steady_clock::now();
  auto second = started, tool_started = started;
  auto animation_tick = started;
  std::uint64_t animation_phase = 0;
  auto start = [&](std::string prompt) {
    if (worker.joinable())
      worker.join();
    busy = true;
    tool_active = false;
    turn_failed = false;
    cancelled.store(false);
    started = std::chrono::steady_clock::now();
    transcript += "\nYou\n" + prompt + "\n\nArco\n";
    scroll = 0;
    activity = "Starting";
    worker = std::jthread([&, prompt = std::move(prompt)] {
      try {
        perform(prompt);
      } catch (const std::exception &e) {
        post(Kind::failure, {});
        text(std::string{"\nError: "} + e.what() + "\n");
      } catch (...) {
        post(Kind::failure, {});
        text("\nTurn failed; inspect the retained audit.\n");
      }
      post(Kind::complete, {});
    });
  };
  if (!initial.empty())
    start(std::move(initial));
  while (true) {
    std::vector<Message> incoming;
    {
      const std::lock_guard lock{mutex_};
      incoming.swap(messages_);
    }
    for (const auto &message : incoming) {
      if (message.kind == Kind::text)
        transcript += message.text;
      if (message.kind == Kind::status) {
        activity = message.text;
        tool_started = std::chrono::steady_clock::now();
        tool_active = true;
      }
      if (message.kind == Kind::operation_complete) {
        activity = message.text;
        tool_active = false;
        transcript += "\n[" + message.text + "]\n";
      }
      if (message.kind == Kind::failure)
        turn_failed = true;
      if (message.kind == Kind::title)
        title_ = message.text;
      if (message.kind == Kind::complete) {
        if (worker.joinable())
          worker.join();
        busy = false;
        tool_active = false;
        activity = cancelled.load() ? "Turn cancelled"
                   : turn_failed    ? "Turn failed"
                                    : "Turn completed";
        activity += " · " +
                    std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                                       std::chrono::steady_clock::now() - started)
                                       .count()) +
                    "s";
        transcript += "\n[" + activity + "]\n";
      }
      redraw = true;
    }
    if (!busy && exit_requested && exit_requested())
      break;
    if (!busy && quitting)
      break;
    if (!busy && !queued.empty()) {
      auto prompt = std::move(queued.front());
      queued.erase(queued.begin());
      if (persist())
        start(std::move(prompt));
      else {
        recovered.push_back(std::move(prompt));
        recovered.insert(recovered.end(), std::make_move_iterator(queued.begin()),
                         std::make_move_iterator(queued.end()));
        queued.clear();
      }
      redraw = true;
    }
    const auto before_trim_lines =
        scroll > 0 && old_columns > 1 && transcript.size() > ui_limit
            ? terminal_lines(
                  transcript,
                  static_cast<std::size_t>(
                      old_columns - 1 -
                      (detail::mascot_visible(old_rows, old_columns) ? 18 : 0)))
                  .size()
            : 0;
    trim_terminal_transcript(transcript);
    winsize dimensions{};
    (void)ioctl(STDOUT_FILENO, TIOCGWINSZ, &dimensions);
    const auto rows = std::max<unsigned short>(dimensions.ws_row, 10);
    const auto columns = std::max<unsigned short>(dimensions.ws_col, 12);
    const auto now = std::chrono::steady_clock::now();
    const bool show_mascot = detail::mascot_visible(rows, columns);
    if (now - animation_tick >= std::chrono::milliseconds{500}) {
      animation_tick = now;
      ++animation_phase;
      if (busy && show_mascot)
        redraw = true;
    }
    if (now - second >= std::chrono::seconds{1}) {
      second = now;
      if (busy)
        redraw = true;
    }
    if (rows != old_rows || columns != old_columns)
      redraw = true;
    if (redraw) {
      const bool same_width =
          old_columns == columns &&
          detail::mascot_visible(old_rows, old_columns) == show_mascot;
      old_rows = rows;
      old_columns = columns;
      redraw = false;
      const auto width = static_cast<std::size_t>(columns - 1 - (show_mascot ? 18 : 0));
      const auto height = static_cast<std::size_t>(rows - 7);
      const auto lines = terminal_lines(transcript, width);
      if (scroll > 0 && same_width)
        scroll = terminal_scroll_after_output(
            {.scroll = scroll,
             .previous = previous_lines,
             .before_trim = before_trim_lines ? before_trim_lines : lines.size(),
             .after_trim = lines.size(),
             .height = height});
      previous_lines = lines.size();
      scroll = std::min(scroll, lines.size() > height ? lines.size() - height : 0);
      const auto end = lines.size() - scroll;
      const auto begin = end > height ? end - height : 0;
      std::string frame = "\x1b[?25l\x1b[H\x1b[2K\x1b[1;36m" +
                          terminal_lines(title_, width)[0] + "\x1b[0m\r\n\x1b[2K";
      std::string status_line;
      if (busy)
        status_line = "Turn " +
                      std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                                         now - started)
                                         .count()) +
                      "s · ";
      if (tool_active)
        status_line += "Tool " +
                       std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                                          now - tool_started)
                                          .count()) +
                       "s · ";
      if (!queued.empty())
        status_line += "queued " + std::to_string(queued.size()) + " · ";
      if (scroll > 0)
        status_line += "Up " + std::to_string(scroll) + " · ";
      status_line += activity;
      frame += terminal_lines(status_line, width)[0] + "\r\n";
      for (std::size_t row = 0; row < height; ++row)
        frame += "\x1b[2K" + (begin + row < end ? lines[begin + row] : "") + "\r\n";
      frame += "\x1b[2K" +
               terminal_lines("Enter send · Alt-Enter newline · Ctrl-C stop · Ctrl-Q "
                              "exit · PgUp/PgDn scroll · /help",
                              width)[0] +
               "\r\n";
      const auto composed = terminal_lines("> " + composer.text(), width);
      const auto prefix =
          terminal_lines("> " + composer.text().substr(0, composer.cursor()), width);
      const auto cursor_row = prefix.size() - 1;
      const auto draft_begin = cursor_row >= 3 ? cursor_row - 2 : 0;
      for (std::size_t row = 0; row < 3; ++row)
        frame +=
            "\x1b[2K" +
            (draft_begin + row < composed.size() ? composed[draft_begin + row] : "") +
            "\r\n";
      const auto cursor_line =
          static_cast<std::size_t>(rows - 3) + cursor_row - draft_begin;
      // Derive cursor cells with the same terminal width rules, including UTF-8.
      std::size_t cells = 0;
      std::mbstate_t state{};
      auto remain = std::string_view{prefix.back()};
      while (!remain.empty()) {
        wchar_t wide{};
        const auto n = std::mbrtowc(&wide, remain.data(), remain.size(), &state);
        if (n == 0 || n > remain.size())
          break;
        cells += static_cast<std::size_t>(std::max(0, ::wcwidth(wide)));
        remain.remove_prefix(n);
      }
      if (show_mascot) {
        const auto sprite = detail::arconaut_sprite(busy, animation_phase);
        for (std::size_t row = 0; row < sprite.size(); ++row)
          frame += "\x1b[" + std::to_string(row + 1) + ";" +
                   std::to_string(columns - 16) + "H" + sprite[row];
      }
      frame += "\x1b[" + std::to_string(cursor_line) + ";" +
               std::to_string(std::min(cells + 1, width)) + "H\x1b[?25h";
      std::cout << frame << std::flush;
    }
    pollfd input{STDIN_FILENO, POLLIN, 0};
    const auto ready = poll(&input, 1, 50);
    if (ready < 0) {
      if (errno == EINTR)
        continue;
      throw std::runtime_error("Terminal polling failed");
    }
    if (ready == 0)
      continue;
    char bytes[512];
    const auto count = read(STDIN_FILENO, bytes, sizeof(bytes));
    if (count <= 0) {
      quitting = true;
      cancelled.store(true);
      persist();
      continue;
    }
    for (ssize_t i = 0; i < count; ++i) {
      auto result = composer.feed(bytes[i]);
      redraw = true;
      if (result.action == InputAction::page_up)
        scroll += static_cast<std::size_t>(rows / 2);
      if (result.action == InputAction::page_down)
        scroll -= std::min(scroll, static_cast<std::size_t>(rows / 2));
      if (result.action == InputAction::cancel) {
        queued.clear();
        if (busy) {
          cancelled.store(true);
          activity = "Stopping";
        }
      }
      if (result.action == InputAction::quit ||
          (result.action == InputAction::submit &&
           (result.text == "/quit" || result.text == "/exit"))) {
        quitting = true;
        cancelled.store(true);
      }
      if (result.action == InputAction::submit && !result.text.empty() && !quitting) {
        if (result.text == "/clear")
          transcript.clear();
        else if (result.text == "/drafts") {
          transcript +=
              "\nRecovered drafts: " + std::to_string(recovered.size()) + "\n";
          for (std::size_t n = 0; n < recovered.size(); ++n)
            transcript += std::to_string(n + 1) + ": " + recovered[n] + "\n";
        } else if (result.text.starts_with("/draft ")) {
          std::size_t n = 0;
          const auto arg = std::string_view{result.text}.substr(7);
          const auto r = std::from_chars(arg.data(), arg.data() + arg.size(), n);
          if (r.ec != std::errc{} || r.ptr != arg.data() + arg.size() || n == 0 ||
              n > recovered.size())
            transcript += "\nInvalid recovered draft number.\n";
          else {
            composer.draft(std::move(recovered[n - 1]));
            recovered.erase(recovered.begin() + static_cast<std::ptrdiff_t>(n - 1));
            transcript += "\nLoaded recovered draft; Enter submits explicitly.\n";
          }
        } else if (busy) {
          auto candidate = composer.state();
          candidate.queued = recovered;
          candidate.queued.insert(candidate.queued.end(), queued.begin(), queued.end());
          candidate.queued.push_back(result.text);
          try {
            validate_state(candidate);
            queued.push_back(std::move(result.text));
          } catch (const Error &) {
            composer.draft(std::move(result.text));
            transcript += "\nQueue full; prompt retained in composer.\n";
          }
        } else {
          if (persist())
            start(std::move(result.text));
          else
            composer.draft(std::move(result.text));
        }
      }
    }
    persist();
  }
  persist();
}
} // namespace arconaut
