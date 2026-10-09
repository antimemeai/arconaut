#pragma once
#include "blackbird/chat_view.hpp"
#include "blackbird/value.hpp"
#include <map>

namespace blackbird {
// Pure bounded presentation of a current task page; no task authority.
std::vector<ChatRow> task_pane_rows(const Value &page, std::size_t width,
                                    const std::map<std::string, Value> &activity = {});
std::string task_summary(const Value &page);
} // namespace blackbird
