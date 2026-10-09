#pragma once
#include <atomic>
#include <mutex>
namespace blackbird::detail {
// Cooperative process-local custody only. No ticket survives process death.
// Arbitrary exec can escape its group; without OS containment that observation
// stays unavailable for the rest of this native lifetime.
inline std::atomic_size_t owned_children{0};
inline std::atomic_bool uncontained_exec{false};
// Covers portable non-atomic CLOEXEC creation and every owned spawn.
inline std::mutex process_spawn_mutex;
inline bool locally_quiescent() noexcept {
  return owned_children.load() == 0 && !uncontained_exec.load();
}
} // namespace blackbird::detail
