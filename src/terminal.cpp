#include "arconaut/terminal.hpp"
#include "arconaut/tools.hpp"
#include "arconaut_sprite.hpp"
#include <algorithm>
#include <array>
#include <cerrno>
#include <charconv>
#include <chrono>
#include <clocale>
#include <cstdlib>
#include <cwchar>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <spawn.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
extern char **environ;
namespace arconaut {
namespace {
struct ChatCommand {
  std::string_view name, arguments, description, group;
};
constexpr std::array commands{
    ChatCommand{"/help", "", "Command guide (immediate in TUI)", "Chat"},
    ChatCommand{"/keys", "", "Keyboard guide (TUI only)", "Chat"},
    ChatCommand{"/commands", "",
                "Search command palette; Ctrl-Space or Ctrl-T (TUI only)", "Chat"},
    ChatCommand{
        "/edit", "",
        "Open a blank draft in $VISUAL/$EDITOR; Ctrl-G edits current draft (idle TUI)",
        "Chat"},
    ChatCommand{"/queue", "", "Show pending prompts (TUI only)", "Chat"},
    ChatCommand{"/cancel", "", "Request stop and clear pending queue (TUI only)",
                "Chat"},
    ChatCommand{"/clear", "", "Clear display, not model context (TUI only)", "Chat"},
    ChatCommand{"/drafts", "", "List recovered drafts (TUI only)", "Chat"},
    ChatCommand{"/draft", "NUMBER",
                "Load a recovered draft; explicit Enter sends (TUI only)", "Chat"},
    ChatCommand{"/quit", "", "Stop work and exit; preserve draft", "Chat"},
    ChatCommand{"/exit", "", "Alias for /quit", "Chat"},
    ChatCommand{"/model", "NAME", "Change model at the next available boundary",
                "Session"},
    ChatCommand{"/effort", "LEVEL", "Set low, medium, high or xhigh", "Session"},
    ChatCommand{"/session", "", "Current session and settings", "Session"},
    ChatCommand{"/sessions", "", "List neighbouring sessions", "Session"},
    ChatCommand{"/stats", "", "Context bytes and observed provider usage", "Session"},
    ChatCommand{"/context", "", "Inspect editable model context", "Context"},
    ChatCommand{"/originals", "", "Retained context originals", "Context"},
    ChatCommand{"/compact", "JSON", "Managed context transformation", "Context"},
    ChatCommand{"/inspect", "JSON", "Bounded retained context inspection", "Context"},
    ChatCommand{"/restore", "ENTRY", "Restore an original context entry", "Context"},
    ChatCommand{"/lua", "CODE", "Run a Lua workflow", "Programs"},
    ChatCommand{"/workflow", "FILE", "Select the turn workflow", "Programs"},
    ChatCommand{"/restart", "NOTE", "Request native restart; build replacement first",
                "Programs"}};

std::string search_text(std::string_view text) {
  std::string out{text};
  for (auto &c : out)
    if (c >= 'A' && c <= 'Z')
      c = static_cast<char>(c + ('a' - 'A'));
  return out;
}
std::vector<std::size_t> palette_matches(std::string_view query) {
  std::vector<std::size_t> matches;
  const auto needle = search_text(query);
  for (std::size_t n = 0; n < commands.size(); ++n) {
    const auto &command = commands[n];
    const auto haystack =
        search_text(std::string{command.name} + " " + std::string{command.description} +
                    " " + std::string{command.group});
    if (haystack.find(needle) != std::string::npos)
      matches.push_back(n);
  }
  auto rank = [&](std::size_t n) {
    auto name = search_text(commands[n].name);
    auto exact = needle;
    if (exact.starts_with("/"))
      exact.erase(0, 1);
    if (name.substr(1) == exact)
      return 0;
    if (name.find(needle) != std::string::npos)
      return 1;
    if (search_text(commands[n].description).find(needle) != std::string::npos)
      return 2;
    return 3;
  };
  std::stable_sort(matches.begin(), matches.end(),
                   [&](auto a, auto b) { return rank(a) < rank(b); });
  return matches;
}
bool command_token(std::string_view text) {
  return text.starts_with("/") &&
         text.find_first_of(" \t\r\n") == std::string_view::npos;
}
std::string complete_command(std::string_view text) {
  if (!command_token(text))
    return std::string{text};
  // An exact command wins over longer names (/draft versus /drafts).
  for (const auto &command : commands)
    if (command.name == text)
      return std::string{text} + (command.arguments.empty() ? "" : " ");
  std::string prefix;
  const ChatCommand *only = nullptr;
  std::size_t count = 0;
  for (const auto &command : commands) {
    if (!command.name.starts_with(text))
      continue;
    if (count++ == 0) {
      prefix = command.name;
      only = &command;
    } else {
      std::size_t n = 0;
      while (n < prefix.size() && n < command.name.size() &&
             prefix[n] == command.name[n])
        ++n;
      prefix.resize(n);
    }
  }
  if (count == 0)
    return std::string{text};
  if (count == 1 && !only->arguments.empty())
    prefix += ' ';
  return prefix;
}
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
std::size_t line_start(std::string_view text, std::size_t cursor) {
  const auto newline =
      cursor == 0 ? std::string_view::npos : text.rfind('\n', cursor - 1);
  return newline == std::string_view::npos ? 0 : newline + 1;
}
std::size_t line_end(std::string_view text, std::size_t cursor) {
  const auto newline = text.find('\n', cursor);
  return newline == std::string_view::npos ? text.size() : newline;
}
std::size_t display_width(std::string_view text) {
  std::size_t cells = 0;
  while (!text.empty()) {
    if (text.front() == '\t') {
      cells += 2;
      text.remove_prefix(1);
      continue;
    }
    wchar_t wide{};
    std::mbstate_t state{};
    const auto n = std::mbrtowc(&wide, text.data(), text.size(), &state);
    const auto width = n != 0 && n <= text.size() ? ::wcwidth(wide) : -1;
    cells += width < 0 ? 4 : static_cast<std::size_t>(width);
    text.remove_prefix(width < 0 ? 1 : n);
  }
  return cells;
}
bool word_space(std::string_view text, std::size_t position) {
  // Whitespace-delimited words, rather than imposing a language's punctuation rules.
  const auto c = static_cast<unsigned char>(text[position]);
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}
std::size_t word_left(std::string_view text, std::size_t cursor) {
  while (cursor > 0 && word_space(text, previous(text, cursor)))
    cursor = previous(text, cursor);
  while (cursor > 0 && !word_space(text, previous(text, cursor)))
    cursor = previous(text, cursor);
  return cursor;
}
std::size_t word_right(std::string_view text, std::size_t cursor) {
  while (cursor < text.size() && word_space(text, cursor))
    cursor = next(text, cursor);
  while (cursor < text.size() && !word_space(text, cursor))
    cursor = next(text, cursor);
  return cursor;
}
class TerminalMode {
public:
  TerminalMode() {
    if (tcgetattr(STDIN_FILENO, &before_) != 0)
      throw std::runtime_error("Cannot read terminal mode");
    resume();
  }
  void resume() {
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
  void suspend() {
    if (!active_)
      return;
    std::cout << "\x1b[0m\x1b[?2004l\x1b[?1049l\x1b[?25h" << std::flush;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &before_) != 0)
      throw std::runtime_error("Cannot restore terminal for editor");
    active_ = false;
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
EditorResult edit_terminal_draft(std::string_view draft) {
  constexpr std::size_t limit = 1024 * 1024;
  if (draft.size() > limit)
    return {false, {}, "Draft exceeds editor limit."};
  std::string path;
  int fd = -1;
  bool owned = false;
  struct Cleanup {
    std::string &path;
    int &fd;
    bool &owned;
    ~Cleanup() {
      if (fd >= 0)
        (void)::close(fd);
      if (owned)
        (void)::unlink(path.c_str());
    }
  } cleanup{path, fd, owned};
  try {
    path = (std::filesystem::temp_directory_path() / "arco-draft-XXXXXX").string();
    fd = ::mkstemp(path.data());
    if (fd < 0)
      throw std::runtime_error("Cannot create private editor draft");
    owned = true;
    if (::fcntl(fd, F_SETFD, FD_CLOEXEC) < 0)
      throw std::runtime_error("Cannot protect editor draft descriptor");
    auto remaining = draft;
    while (!remaining.empty()) {
      const auto n = ::write(fd, remaining.data(), remaining.size());
      if (n < 0 && errno == EINTR)
        continue;
      if (n <= 0)
        throw std::runtime_error("Cannot write editor draft");
      remaining.remove_prefix(static_cast<std::size_t>(n));
    }
    (void)::close(fd);
    fd = -1;
    const char *editor = std::getenv("VISUAL");
    if (!editor || !*editor)
      editor = std::getenv("EDITOR");
    if (!editor || !*editor)
      editor = "vi";
    std::string quoted = "'";
    for (const char c : path)
      quoted += c == '\'' ? "'\\''" : std::string(1, c);
    quoted += '\'';
    std::string command = "exec " + std::string{editor} + " " + quoted;
    std::array<char *, 4> args{const_cast<char *>("sh"), const_cast<char *>("-c"),
                               command.data(), nullptr};
    pid_t child{};
    const auto launched =
        ::posix_spawn(&child, "/bin/sh", nullptr, nullptr, args.data(), environ);
    if (launched != 0)
      return {false, {}, "Editor could not start; draft retained."};
    int status{};
    pid_t waited;
    do {
      waited = ::waitpid(child, &status, 0);
    } while (waited < 0 && errno == EINTR);
    if (waited < 0 || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
      return {false, {}, "Editor cancelled or failed; draft retained."};
    fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK);
    struct stat info{};
    if (fd < 0 || ::fstat(fd, &info) != 0 || !S_ISREG(info.st_mode))
      return {false,
              {},
              "Editor draft unavailable or not a regular file; original retained."};
    std::string edited;
    std::array<char, 4096> bytes{};
    while (true) {
      const auto n = ::read(fd, bytes.data(), bytes.size());
      if (n < 0 && errno == EINTR)
        continue;
      if (n < 0)
        return {false, {}, "Cannot read editor draft; original retained."};
      if (n == 0)
        break;
      const auto count = static_cast<std::size_t>(n);
      if (count > limit - edited.size())
        return {false, {}, "Editor draft exceeds 1 MiB; original retained."};
      edited.append(bytes.data(), count);
    }
    return {true, std::move(edited), "Editor returned; Enter sends explicitly."};
  } catch (const std::exception &e) {
    return {false, {}, std::string{e.what()} + "; original draft retained."};
  }
}
std::string terminal_key_help() {
  return "Keyboard\n"
         "  Enter        Send; while busy, queue for the next turn\n"
         "  Alt-Enter    Insert newline (also Ctrl-J)\n"
         "  Tab          Complete a slash-command name; ambiguity never submits\n"
         "  Ctrl-G       Edit current draft in $VISUAL/$EDITOR (idle only)\n"
         "  Ctrl-Space/T Search commands; arrows select, Enter loads, Esc backs out\n"
         "  Up / Down    Move between draft lines; history at first/last line\n"
         "  Ctrl-P / N   Explicit history; restores your draft and cursor\n"
         "  Left / Right Move within the draft\n"
         "  Ctrl-A / E   Start / end of line (also Home / End)\n"
         "  Ctrl-Home/End Start / end of draft\n"
         "  Alt-B / F    Back / forward word (also Ctrl-Left / Right)\n"
         "  Ctrl-W       Delete previous word; Alt-D deletes next word\n"
         "  Ctrl-K / Y   Kill to line end / yank last deletion\n"
         "  Ctrl-U       Clear draft; Ctrl-Y restores it\n"
         "  Ctrl-C       Request stop and clear pending queue; keep unsent draft\n"
         "  Ctrl-Q       Stop and exit; preserve unsent draft\n"
         "  PgUp / PgDn  Scroll transcript\n"
         "  Paste        Multiline text stays literal; Enter sends explicitly\n";
}
std::string terminal_help() {
  std::string out = "Arco command guide\n";
  std::string_view group;
  for (const auto &command : commands) {
    if (command.group != group) {
      group = command.group;
      out += "\n" + std::string{group} + "\n";
    }
    out += "  " + std::string{command.name};
    if (!command.arguments.empty())
      out += " " + std::string{command.arguments};
    out += " — " + std::string{command.description} + "\n";
  }
  out +=
      "\nLocal TUI controls run immediately, even during work. Other commands queue\n"
      "behind the active turn; they do not mutate an in-flight provider request.\n\n";
  return out +
         "Keyboard: /keys · Tab completes commands · PgUp/PgDn browse this guide\n";
}
std::string terminal_command_hint(std::string_view draft) {
  if (!draft.starts_with("/") || draft.find_first_of("\r\n") != std::string_view::npos)
    return {};
  const auto token = draft.substr(0, draft.find_first_of(" \t"));
  std::string matches;
  for (const auto &command : commands) {
    if (command.name == token) {
      return std::string{command.name} +
             (command.arguments.empty() ? "" : " " + std::string{command.arguments}) +
             " — " + std::string{command.description};
    }
    if (command.name.starts_with(token) && token == draft) {
      if (!matches.empty())
        matches += " · ";
      matches += command.name;
    }
  }
  return matches.empty() ? "Unknown command · /help lists commands"
                         : "Tab complete · " + matches;
}
void Composer::insert(char byte) {
  if (palette_open_) {
    if (static_cast<unsigned char>(byte) >= 32 && byte != 127 &&
        palette_query_.size() < 128) {
      palette_query_ += byte;
      palette_selected_ = 0;
    }
    return;
  }
  goal_column_.reset();
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
  draft_.clear();
  draft_cursor_ = 0;
  killed_.clear();
  goal_column_.reset();
  escape_.clear();
  pasted_ = false;
  palette_open_ = false;
  palette_query_.clear();
  palette_selected_ = 0;
}
TerminalState Composer::state() const { return {text_, cursor_, history_, {}}; }
void Composer::draft(std::string text) {
  if (text.size() > ui_limit)
    throw Error{ErrorCode::capacity};
  text_ = std::move(text);
  cursor_ = text_.size();
  history_index_ = history_.size();
  draft_.clear();
  draft_cursor_ = 0;
  goal_column_.reset();
}
void Composer::palette_move(bool up) {
  const auto matches = palette_matches(palette_query_);
  if (matches.empty()) {
    palette_selected_ = 0;
    return;
  }
  if (up)
    palette_selected_ -= palette_selected_ > 0 ? 1U : 0U;
  else
    palette_selected_ = std::min(palette_selected_ + 1, matches.size() - 1);
}
bool Composer::flush_escape() {
  if (palette_open_ && escape_ == "\x1b") {
    escape_.clear();
    palette_open_ = false;
    return true;
  }
  return false;
}
std::vector<std::string> Composer::palette_lines(std::size_t rows) const {
  if (!palette_open_ || rows == 0)
    return {};
  const auto matches = palette_matches(palette_query_);
  std::vector<std::string> out{"Commands / " + palette_query_ + "_"};
  if (rows == 1)
    return out;
  const auto count = rows > 2 ? rows - 2 : 1;
  if (matches.empty())
    out.emplace_back("  No matching commands");
  else {
    const auto first = palette_selected_ >= count ? palette_selected_ - count + 1 : 0;
    for (std::size_t n = first; n < matches.size() && n - first < count; ++n) {
      const auto &command = commands[matches[n]];
      out.push_back(
          (n == palette_selected_ ? "> " : "  ") + std::string{command.name} +
          (command.arguments.empty() ? "" : " " + std::string{command.arguments}) +
          " — " + std::string{command.description});
    }
  }
  if (rows > 2)
    out.emplace_back("Type search · Up/Down select · Enter loads · Esc back");
  out.resize(rows);
  return out;
}
void Composer::history(bool older) {
  if (older) {
    if (history_index_ == 0)
      return;
    if (history_index_ == history_.size()) {
      draft_ = text_;
      draft_cursor_ = cursor_;
    }
    text_ = history_[--history_index_];
    cursor_ = text_.size();
  } else {
    if (history_index_ == history_.size())
      return;
    ++history_index_;
    text_ = history_index_ == history_.size() ? draft_ : history_[history_index_];
    cursor_ = history_index_ == history_.size() ? draft_cursor_ : text_.size();
  }
  goal_column_.reset();
}
void Composer::vertical(bool up) {
  const auto begin = line_start(text_, cursor_);
  const auto end = line_end(text_, cursor_);
  if ((up && begin == 0) || (!up && end == text_.size())) {
    history(up);
    return;
  }
  if (!goal_column_)
    goal_column_ =
        display_width(std::string_view{text_}.substr(begin, cursor_ - begin));
  const auto target_begin = up ? line_start(text_, begin - 1) : end + 1;
  const auto target_end = line_end(text_, target_begin);
  std::size_t position = target_begin, cells = 0;
  while (position < target_end) {
    const auto after = next(text_, position);
    const auto width =
        display_width(std::string_view{text_}.substr(position, after - position));
    if (cells + width > *goal_column_)
      break;
    cells += width;
    position = after;
  }
  cursor_ = position;
}
void Composer::kill(std::size_t begin, std::size_t end) {
  if (begin == end)
    return;
  killed_ = text_.substr(begin, end - begin);
  text_.erase(begin, end - begin);
  cursor_ = begin;
  goal_column_.reset();
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
      if (!palette_open_)
        insert('\n');
      escape_.clear();
      return {};
    }
    if (escape_.size() == 2 && (byte == '[' || byte == 'O'))
      return {};
    if (escape_.size() >= 3 && c >= 0x30U && c <= 0x3fU)
      return {};
    if (palette_open_) {
      if (escape_ == "\x1b[A" || escape_ == "\x1bOA")
        palette_move(true);
      if (escape_ == "\x1b[B" || escape_ == "\x1bOB")
        palette_move(false);
      escape_.clear();
      return {};
    }
    const bool up = escape_ == "\x1b[A" || escape_ == "\x1bOA";
    const bool down = escape_ == "\x1b[B" || escape_ == "\x1bOB";
    if (up || down)
      vertical(up);
    else {
      goal_column_.reset();
      if (escape_ == "\x1b[D" || escape_ == "\x1bOD")
        cursor_ = previous(text_, cursor_);
      if (escape_ == "\x1b[C" || escape_ == "\x1bOC")
        cursor_ = next(text_, cursor_);
      if (escape_ == "\x1b[H" || escape_ == "\x1bOH" || escape_ == "\x1b[1~" ||
          escape_ == "\x1b[7~")
        cursor_ = line_start(text_, cursor_);
      if (escape_ == "\x1b[F" || escape_ == "\x1bOF" || escape_ == "\x1b[4~" ||
          escape_ == "\x1b[8~")
        cursor_ = line_end(text_, cursor_);
      if (escape_ == "\x1b[1;5H")
        cursor_ = 0;
      if (escape_ == "\x1b[1;5F")
        cursor_ = text_.size();
      if (escape_ == "\x1b"
                     "b" ||
          escape_ == "\x1b[1;5D" || escape_ == "\x1b[1;3D")
        cursor_ = word_left(text_, cursor_);
      if (escape_ == "\x1b"
                     "f" ||
          escape_ == "\x1b[1;5C" || escape_ == "\x1b[1;3C")
        cursor_ = word_right(text_, cursor_);
      if (escape_ == "\x1b"
                     "d")
        kill(cursor_, word_right(text_, cursor_));
      if (escape_ == "\x1b\x7f" || escape_ == "\x1b\x08")
        kill(word_left(text_, cursor_), cursor_);
      if (escape_ == "\x1b[3~" && cursor_ < text_.size())
        text_.erase(cursor_, next(text_, cursor_) - cursor_);
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
  if (c == 0 || c == 20) {
    palette_open_ = !palette_open_;
    palette_query_.clear();
    palette_selected_ = 0;
    return {};
  }
  if (palette_open_) {
    if (c == 3 || c == 17) {
      palette_open_ = false;
      return {c == 17 ? InputAction::quit : InputAction::none, {}};
    }
    if (c == 13) {
      const auto matches = palette_matches(palette_query_);
      if (!matches.empty()) {
        const auto &command = commands[matches[palette_selected_]];
        palette_open_ = false;
        return {InputAction::choose_command,
                std::string{command.name} + (command.arguments.empty() ? "" : " ")};
      }
    } else if (c == 16 || c == 14 || c == 9)
      palette_move(c == 16);
    else if (c == 127 || c == 8) {
      palette_query_.erase(previous(palette_query_, palette_query_.size()));
      palette_selected_ = 0;
    } else
      insert(byte);
    return {};
  }
  if (c == 7)
    return {InputAction::edit, {}};
  if (c == 16 || c == 14) {
    history(c == 16);
    return {};
  }
  if (c == 23) {
    kill(word_left(text_, cursor_), cursor_);
    return {};
  }
  if (c == 11) {
    auto end = line_end(text_, cursor_);
    if (end == cursor_ && end < text_.size())
      ++end;
    kill(cursor_, end);
    return {};
  }
  if (c == 25) {
    if (killed_.size() <= ui_limit - text_.size()) {
      text_.insert(cursor_, killed_);
      cursor_ += killed_.size();
      goal_column_.reset();
    }
    return {};
  }
  if (c == 9) {
    if (cursor_ == text_.size()) {
      text_ = complete_command(text_);
      cursor_ = text_.size();
      goal_column_.reset();
    }
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
    draft_cursor_ = 0;
    goal_column_.reset();
    return {InputAction::submit, std::move(result)};
  }
  if (c == 10) {
    insert('\n');
    return {};
  }
  if (c == 1) {
    goal_column_.reset();
    cursor_ = line_start(text_, cursor_);
    return {};
  }
  if (c == 5) {
    goal_column_.reset();
    cursor_ = line_end(text_, cursor_);
    return {};
  }
  if (c == 21) {
    kill(0, text_.size());
    cursor_ = 0;
    goal_column_.reset();
    return {};
  }
  if (c == 127 || c == 8) {
    goal_column_.reset();
    const auto p = previous(text_, cursor_);
    text_.erase(p, cursor_ - p);
    cursor_ = p;
    return {};
  }
  if (c == 4 && cursor_ < text_.size()) {
    goal_column_.reset();
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
  auto edit = [&] {
    if (busy) {
      transcript += "\nEditor waits for an idle turn; draft retained.\n";
      return false;
    }
    if (!persist())
      return false;
    mode.suspend();
    const auto edited = edit_terminal_draft(composer.text());
    mode.resume();
    (void)tcflush(STDIN_FILENO,
                  TCIFLUSH); // Editor typeahead must never send the returned draft.
    if (edited.accepted && edited.text != composer.text())
      composer.draft(edited.text);
    transcript += "\n" + edited.message + "\n";
    (void)persist();
    scroll = 0;
    redraw = true;
    return true;
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
      const auto palette = composer.palette_lines(std::min<std::size_t>(8, rows - 9));
      const auto height = static_cast<std::size_t>(rows - 7) - palette.size();
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
      for (const auto &line : palette)
        frame += "\x1b[2K\x1b[36m" + terminal_lines(line, width)[0] + "\x1b[0m\r\n";
      auto hint = terminal_command_hint(composer.text());
      if (composer.palette_open())
        hint = "Palette selection loads the composer; it never submits. Esc returns to "
               "draft.";
      else if (hint.empty())
        hint = busy ? "Enter queue · Alt-Enter newline · Ctrl-C stop · /queue · /help"
                    : "Enter send · Alt-Enter newline · Tab commands · /help";
      frame += "\x1b[2K\x1b[2m" + terminal_lines(hint, width)[0] + "\x1b[0m\r\n";
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
      const auto display_cursor_line =
          composer.palette_open() ? height + 3 : cursor_line;
      const auto display_cursor_cells =
          composer.palette_open()
              ? display_width(palette.front().substr(0, palette.front().size() - 1)) + 1
              : cells + 1;
      frame += "\x1b[" + std::to_string(display_cursor_line) + ";" +
               std::to_string(std::min(display_cursor_cells, width)) + "H\x1b[?25h";
      std::cout << frame << std::flush;
    }
    pollfd input{STDIN_FILENO, POLLIN, 0};
    const auto ready = poll(&input, 1, 50);
    if (ready < 0) {
      if (errno == EINTR)
        continue;
      throw std::runtime_error("Terminal polling failed");
    }
    if (ready == 0) {
      if (composer.flush_escape())
        redraw = true;
      continue;
    }
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
      if (result.action == InputAction::choose_command) {
        bool load = true;
        if (!composer.text().empty()) {
          auto candidate = composer.state();
          candidate.queued = recovered;
          candidate.queued.insert(candidate.queued.end(), queued.begin(), queued.end());
          candidate.queued.push_back(composer.text());
          try {
            validate_state(candidate);
            recovered.push_back(composer.text());
          } catch (const Error &) {
            load = false;
            transcript +=
                "\nDraft storage full; original retained, command not loaded.\n";
          }
        }
        if (load) {
          const bool saved_draft = !composer.text().empty();
          composer.draft(std::move(result.text));
          transcript += saved_draft ? "\nCommand loaded; Enter sends explicitly. "
                                      "Previous draft in /drafts.\n"
                                    : "\nCommand loaded; Enter sends explicitly.\n";
        }
      }
      if (result.action == InputAction::edit && edit())
        break;
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
        if (result.text == "/commands") {
          (void)composer.feed(0);
        } else if (result.text == "/edit") {
          if (edit())
            break;
        } else if (result.text == "/help" || result.text == "/keys") {
          transcript +=
              "\n" + (result.text == "/help" ? terminal_help() : terminal_key_help());
          scroll = 0;
        } else if (result.text == "/queue") {
          transcript += "\nPending prompts: " + std::to_string(queued.size()) + "\n";
          for (std::size_t n = 0; n < queued.size(); ++n)
            transcript += std::to_string(n + 1) + ": " + queued[n] + "\n";
          transcript += "Recovered drafts: " + std::to_string(recovered.size()) +
                        " (use /drafts)\n";
          scroll = 0;
        } else if (result.text == "/cancel") {
          queued.clear();
          if (busy) {
            cancelled.store(true);
            activity = "Stopping";
          }
          transcript += busy ? "\nStop requested; pending queue cleared.\n"
                             : "\nNo active turn; pending queue cleared.\n";
          scroll = 0;
        } else if (result.text == "/clear") {
          transcript.clear();
          scroll = 0;
        } else if (result.text == "/drafts") {
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
