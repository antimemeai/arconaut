#pragma once
#include "blackbird/tools.hpp"
namespace blackbird {
// Lazy, process-local binding and cache; no constructor effects.
class BeadsAdapter {
public:
  std::function<bool()> cancelled;
  LocalTools::Observer observer;
  bool mutation_may_have_started() const noexcept { return mutation_possible_; }
  Json run(const Json &args);
private:
  std::string project_, executable_, actor_;
  bool bound_ = false, mutation_possible_ = false;
  Json cache_;
  Json invoke(const std::vector<std::string> &argv, bool write);
};
} // namespace blackbird
