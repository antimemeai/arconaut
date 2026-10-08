#include "blackbird/task_view.hpp"
#include "blackbird/context.hpp"
#include "blackbird/terminal.hpp"
#include <algorithm>

namespace blackbird {
std::string task_summary(const Json &page) {
  const auto &counts = field(page, "counts");
  return field(counts, "done").number().text + " done · " +
         field(counts, "active").number().text + " active · " +
         field(counts, "blocked").number().text + " blocked · " +
         field(counts, "queued").number().text + " queued";
}
std::vector<ChatRow> task_pane_rows(const Json &page, std::size_t width,
                                    const std::map<std::string, Json> &activity) {
  std::vector<ChatRow> out;
  if (width < 8)
    return out;
  auto append = [&](std::string_view text, Ink ink, std::size_t maximum) {
    const auto lines = terminal_lines(text, width);
    for (std::size_t i = 0; i < std::min(maximum, lines.size()); ++i)
      out.push_back({{lines[i], ink}});
  };
  append("TASKS  " + string_field(page, "title"), Ink::heading, 1);
  append(task_summary(page), Ink::muted, 2);
  if (!string_field(page, "bead").empty())
    append("Bead " + string_field(page, "bead"), Ink::muted, 1);
  if (field(page, "items").array().empty())
    append("No tasks", Ink::muted, 1);
  for (const auto &row : field(page, "items").array()) {
    const auto &status = string_field(row, "status");
    const auto &id = string_field(row, "id");
    const bool child = !string_field(row, "parent").empty();
    const auto marker = status == "done"      ? "✓ "
                        : status == "active"  ? "▶ "
                        : status == "blocked" ? "! "
                        : status == "dropped" ? "– "
                                              : "○ ";
    const auto ink = status == "done"      ? Ink::success
                     : status == "blocked" ? Ink::failure
                     : status == "active"  ? Ink::assistant
                                           : Ink::muted;
    append(std::string{child ? "  " : ""} + marker + id + " " +
               string_field(row, "title"),
           ink, 2);
    std::string detail = "   ";
    if (child)
      detail += "  ";
    detail += status;
    if (const auto found = activity.find(id); found != activity.end())
      detail += " · " + string_field(found->second, "phase");
    if (const auto *count = row.find("subtasks"); count && count->number().text != "0")
      detail += " · " + field(row, "subtasks_done").number().text + "/" +
                count->number().text;
    if (!string_field(row, "owner").empty())
      detail += " · " + string_field(row, "owner");
    append(detail, Ink::muted, 1);
    if (status == "blocked" && !string_field(row, "blocker").empty())
      append("   " + string_field(row, "blocker"), Ink::failure, 2);
  }
  if (field(page, "next") != Json{})
    append("More tasks ↓", Ink::muted, 1);
  append("/tasks up · down · fold ID", Ink::muted, 1);
  return out;
}
} // namespace blackbird
