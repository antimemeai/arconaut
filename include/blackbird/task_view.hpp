#pragma once
#include "blackbird/chat_view.hpp"
#include "blackbird/json.hpp"
#include <map>

namespace blackbird {
// Pure bounded presentation of a current task page; no task authority.
std::vector<ChatRow> task_pane_rows(const Json &page, std::size_t width,
                                    const std::map<std::string, Json> &activity = {});
std::string task_summary(const Json &page);
} // namespace blackbird
