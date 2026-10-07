#pragma once

#include "blackbird/foundation.hpp"

#include <memory>
#include <string_view>

namespace blackbird {

enum class FileAccess : std::uint8_t { read_only, read_write };
class JournalFile : public Storage {
public:
  virtual Result<void> lock_writer() = 0;
  // Memory adapters have no lease; descriptor decorators must forward release.
  // Reader references retain storage ownership, not writer ownership.
  virtual void release_writer() noexcept {}
};
class JournalDirectory {
public:
  virtual ~JournalDirectory() = default;
  virtual Result<std::unique_ptr<JournalFile>>
  create_exclusive(std::string_view name) = 0;
  virtual Result<std::unique_ptr<JournalFile>> open_existing(std::string_view name,
                                                             FileAccess access) = 0;
  virtual Result<void> synchronize_directory(SyncStrength strength) = 0;
  virtual Result<void> replace_file(std::string_view, std::string_view) {
    return Result<void>::failure({ErrorCode::unsupported});
  }
};
class NativeJournalFile final : public JournalFile {
public:
  ~NativeJournalFile() override;
  NativeJournalFile(const NativeJournalFile &) = delete;
  NativeJournalFile &operator=(const NativeJournalFile &) = delete;
  NativeJournalFile(NativeJournalFile &&other) noexcept;
  NativeJournalFile &operator=(NativeJournalFile &&other) noexcept;
  Result<std::size_t> read_at(std::uint64_t offset, MutableByteView output) override;
  Result<std::size_t> write_at(std::uint64_t offset, ByteView input) override;
  Result<std::uint64_t> extent() override;
  Result<void> synchronize(SyncStrength strength) override;
  Result<void> lock_writer() override;
  void release_writer() noexcept override;

private:
  friend class NativeJournalDirectory;
  explicit NativeJournalFile(int descriptor) : descriptor_(descriptor) {}
  int descriptor_;
};
class NativeJournalDirectory final : public JournalDirectory {
public:
  static Result<NativeJournalDirectory> open(std::string_view path);
  ~NativeJournalDirectory() override;
  NativeJournalDirectory(const NativeJournalDirectory &) = delete;
  NativeJournalDirectory &operator=(const NativeJournalDirectory &) = delete;
  NativeJournalDirectory(NativeJournalDirectory &&other) noexcept;
  NativeJournalDirectory &operator=(NativeJournalDirectory &&other) noexcept;
  Result<std::unique_ptr<JournalFile>> create_exclusive(std::string_view name) override;
  Result<std::unique_ptr<JournalFile>> open_existing(std::string_view name,
                                                     FileAccess access) override;
  Result<void> synchronize_directory(SyncStrength strength) override;
  Result<void> replace_file(std::string_view from, std::string_view to) override;

private:
  explicit NativeJournalDirectory(int descriptor) : descriptor_(descriptor) {}
  Result<std::unique_ptr<JournalFile>> open_file(std::string_view name, int flags);
  int descriptor_;
};

} // namespace blackbird
