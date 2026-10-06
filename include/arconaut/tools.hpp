#pragma once
#include "arconaut/context.hpp"
#include <filesystem>
#include <functional>
namespace arconaut {
struct CapturedOutput {
  std::string label;
  std::string bytes;
};
class LocalTools {
public:
  using Observer = std::function<void(std::string_view, std::string_view)>;
  explicit LocalTools(Observer observer = {}) : observer_(std::move(observer)) {}
  std::function<bool()> cancelled;
  Json run(std::string_view name, const Json &arguments);
  const std::vector<CapturedOutput> &captured() const noexcept { return captured_; }

private:
  std::vector<CapturedOutput> captured_;
  Observer observer_;
  void capture(std::string label, std::string raw);
};
std::string read_file(const std::filesystem::path &path,
                      std::size_t limit = 16 * 1024 * 1024);
void write_file(const std::filesystem::path &path, std::string_view bytes);
Json process_output_presentation(const std::string &raw, const Json &args);
Json tool_definitions();
} // namespace arconaut
