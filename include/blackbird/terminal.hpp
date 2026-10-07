#pragma once
#include "blackbird/chat_view.hpp"
#include <atomic>
#include <filesystem>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
namespace blackbird {
enum class InputAction {
  none,
  submit,
  cancel,
  quit,
  page_up,
  page_down,
  edit,
  choose_command
};
struct InputResult {
  InputAction action = InputAction::none;
  std::string text;
};
struct TerminalState {
  std::string draft;
  std::size_t cursor = 0;
  std::vector<std::string> history, queued;
};
struct EditorResult {
  bool accepted = false;
  std::string text, message;
};
// Synchronous operator-selected foreground editor; caller owns terminal handoff.
EditorResult edit_terminal_draft(std::string_view draft);
// Shared command discovery; local-only commands are labelled in help.
std::string terminal_help();
std::string terminal_key_help();
std::string terminal_command_hint(std::string_view draft);
TerminalState load_terminal_state(const std::filesystem::path &path);
void save_terminal_state(const std::filesystem::path &path, const TerminalState &state);
class Composer {
public:
  InputResult feed(char byte);
  void restore(const TerminalState &state);
  TerminalState state() const;
  void draft(std::string text);
  bool palette_open() const noexcept { return palette_open_; }
  bool slash_open() const noexcept { return slash_open_; }
  bool escape_pending() const noexcept { return !escape_.empty(); }
  bool flush_escape();
  std::vector<std::string> palette_lines(std::size_t rows) const;
  const std::string &text() const noexcept { return text_; }
  std::size_t cursor() const noexcept { return cursor_; }

private:
  std::string text_, escape_;
  std::size_t cursor_ = 0, history_index_ = 0;
  std::vector<std::string> history_;
  std::string draft_, killed_;
  std::size_t draft_cursor_ = 0;
  std::optional<std::size_t> goal_column_;
  bool pasted_ = false, palette_open_ = false;
  bool slash_open_ = false;
  std::string palette_query_;
  std::size_t palette_selected_ = 0;
  void palette_move(bool up);
  void insert(char byte);
  void history(bool older);
  void vertical(bool up);
  void kill(std::size_t begin, std::size_t end);
};
struct TerminalScrollUpdate {
  std::size_t scroll, previous, before_trim, after_trim, height;
};
std::size_t terminal_scroll_after_output(const TerminalScrollUpdate &update);
void trim_terminal_transcript(std::string &text);
std::vector<ChatRow> terminal_powerword_lines(std::string_view text,
                                              std::size_t columns);
std::vector<std::string> terminal_lines(std::string_view text, std::size_t columns);
class TerminalUI {
public:
  explicit TerminalUI(std::string title) : title_(std::move(title)) {}
  void text(std::string_view text);
  void notice(std::string_view text);
  void process_output(std::string_view text);
  void restore_message(ChatKind kind, std::string_view text);
  void usage(std::string_view text);
  void operation_started(bool provider);
  void status(std::string_view text);
  void operation_completed(std::string_view text, Ink outcome = Ink::muted);
  void failed();
  void title(std::string_view text);
  void run(const std::function<void(std::string_view)> &perform,
           std::atomic_bool &cancelled,
           const std::function<bool()> &exit_requested = {}, std::string initial = {},
           std::filesystem::path state_path = {});

private:
  enum class Kind {
    text,
    notice,
    process,
    user,
    assistant,
    usage,
    provider_start,
    tool_start,
    status,
    title,
    complete,
    operation_complete,
    failure
  };
  struct Message {
    Kind kind;
    std::string text;
    Ink outcome = Ink::muted;
  };
  std::mutex mutex_;
  int notification_ = -1;
  std::vector<Message> messages_;
  std::string title_;
  void post(Kind kind, std::string_view text);
};
} // namespace blackbird
