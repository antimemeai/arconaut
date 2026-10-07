#pragma once
#include <atomic>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>
namespace arconaut {
enum class InputAction { none, submit, cancel, quit, page_up, page_down };
struct InputResult {
  InputAction action = InputAction::none;
  std::string text;
};
struct TerminalState {
  std::string draft;
  std::size_t cursor = 0;
  std::vector<std::string> history, queued;
};
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
  const std::string &text() const noexcept { return text_; }
  std::size_t cursor() const noexcept { return cursor_; }

private:
  std::string text_, escape_;
  std::size_t cursor_ = 0, history_index_ = 0;
  std::vector<std::string> history_;
  std::string draft_;
  bool pasted_ = false;
  void insert(char byte);
};
struct TerminalScrollUpdate {
  std::size_t scroll, previous, before_trim, after_trim, height;
};
std::size_t terminal_scroll_after_output(const TerminalScrollUpdate &update);
void trim_terminal_transcript(std::string &text);
std::vector<std::string> terminal_lines(std::string_view text, std::size_t columns);
class TerminalUI {
public:
  explicit TerminalUI(std::string title) : title_(std::move(title)) {}
  void text(std::string_view text);
  void status(std::string_view text);
  void operation_completed(std::string_view text);
  void failed();
  void title(std::string_view text);
  void run(const std::function<void(std::string_view)> &perform,
           std::atomic_bool &cancelled,
           const std::function<bool()> &exit_requested = {}, std::string initial = {},
           std::filesystem::path state_path = {});

private:
  enum class Kind { text, status, title, complete, operation_complete, failure };
  struct Message {
    Kind kind;
    std::string text;
  };
  std::mutex mutex_;
  std::vector<Message> messages_;
  std::string title_;
  void post(Kind kind, std::string_view text);
};
} // namespace arconaut
