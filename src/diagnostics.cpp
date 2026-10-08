#include "blackbird/journal_storage.hpp"
#include <cerrno>
#include <charconv>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace blackbird {
namespace {
std::optional<std::uint64_t> expiration(std::string_view name) {
  const auto dot = name.find('.');
  if (dot == std::string_view::npos || !dot || dot > 20 ||
      name.size() - dot - 1 != 32 ||
      name.substr(dot + 1).find_first_not_of("0123456789abcdef") !=
          std::string_view::npos)
    return {};
  std::uint64_t result = 0;
  auto parsed = std::from_chars(name.data(), name.data() + dot, result);
  if (parsed.ec != std::errc{} || parsed.ptr != name.data() + dot ||
      std::to_string(result) != name.substr(0, dot))
    return {};
  return result;
}
bool private_file(const struct stat &s) {
  return S_ISREG(s.st_mode) && s.st_uid == geteuid() && !(s.st_mode & 0077) &&
         s.st_nlink == 1;
}
} // namespace
class DiagnosticStore {
public:
  int fd = -1;
  DIR *scan = nullptr;
  explicit DiagnosticStore(int descriptor) : fd(descriptor) {}
  ~DiagnosticStore() {
    if (scan)
      closedir(scan);
    if (fd >= 0)
      close(fd);
  }
  static Result<std::shared_ptr<DiagnosticStore>> open(int parent) {
    if (mkdirat(parent, "diagnostics", 0700) < 0 && errno != EEXIST)
      return Result<std::shared_ptr<DiagnosticStore>>::failure({ErrorCode::io, errno});
    const int descriptor =
        openat(parent, "diagnostics", O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
    if (descriptor < 0)
      return Result<std::shared_ptr<DiagnosticStore>>::failure({ErrorCode::io, errno});
    struct stat s{};
    if (fstat(descriptor, &s) < 0 || !S_ISDIR(s.st_mode) || s.st_uid != geteuid() ||
        (s.st_mode & 0077)) {
      close(descriptor);
      return Result<std::shared_ptr<DiagnosticStore>>::failure({ErrorCode::io, EPERM});
    }
    try {
      return Result<std::shared_ptr<DiagnosticStore>>::success(
          std::make_shared<DiagnosticStore>(descriptor));
    } catch (const std::bad_alloc &) {
      close(descriptor);
      return Result<std::shared_ptr<DiagnosticStore>>::failure({ErrorCode::allocation});
    }
  }
  Result<ScratchCleanup> purge(std::uint64_t now) {
    if (!scan) {
      const int separate = openat(fd, ".", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
      if (separate < 0)
        return Result<ScratchCleanup>::failure({ErrorCode::io, errno});
      scan = fdopendir(separate);
      if (!scan) {
        close(separate);
        return Result<ScratchCleanup>::failure({ErrorCode::io, errno});
      }
    }
    ScratchCleanup result;
    while (result.scanned < 128 && result.removed < 16) {
      errno = 0;
      auto *entry = readdir(scan);
      if (!entry) {
        const int error = errno;
        closedir(scan);
        scan = nullptr;
        if (error)
          return Result<ScratchCleanup>::failure({ErrorCode::io, error});
        result.complete = true;
        break;
      }
      ++result.scanned;
      const auto expires = expiration(entry->d_name);
      if (!expires || *expires > now)
        continue;
      struct stat s{};
      if (fstatat(fd, entry->d_name, &s, AT_SYMLINK_NOFOLLOW) < 0) {
        if (errno == ENOENT)
          continue;
        return Result<ScratchCleanup>::failure({ErrorCode::io, errno});
      }
      if (!private_file(s))
        continue;
      if (unlinkat(fd, entry->d_name, 0) < 0) {
        if (errno == ENOENT)
          continue;
        return Result<ScratchCleanup>::failure({ErrorCode::io, errno});
      }
      ++result.removed;
    }
    return Result<ScratchCleanup>::success(result);
  }
};
Result<ScratchCleanup> NativeJournalDirectory::expire_diagnostics(std::uint64_t now) {
  if (!diagnostics_) {
    auto opened = DiagnosticStore::open(descriptor_);
    if (!opened.has_value())
      return Result<ScratchCleanup>::failure(opened.error());
    diagnostics_ = std::move(opened).value();
  }
  return diagnostics_->purge(now);
}
Result<void> NativeJournalDirectory::store_diagnostic(std::string_view name,
                                                      ByteView bytes,
                                                      std::uint64_t now) {
  const auto expires = expiration(name);
  if (!expires || *expires <= now || *expires - now != 30ULL * 24 * 60 * 60 ||
      bytes.size() > 65536)
    return Result<void>::failure({ErrorCode::invalid_range});
  auto purged = expire_diagnostics(now);
  if (!purged.has_value())
    return Result<void>::failure(purged.error());
  const std::string filename{name};
  const int file = openat(diagnostics_->fd, filename.c_str(),
                          O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW, 0600);
  if (file < 0)
    return Result<void>::failure({ErrorCode::io, errno});
  std::size_t offset = 0;
  Error error{ErrorCode::io};
  while (offset < bytes.size()) {
    const auto n = write(file, bytes.data() + offset, bytes.size() - offset);
    if (n < 0) {
      if (errno == EINTR)
        continue;
      error.detail = errno;
      break;
    }
    if (!n)
      break;
    offset += static_cast<std::size_t>(n);
  }
  close(file); // Optional diagnostics: no durability sync or authoritative dependency.
  if (offset != bytes.size()) {
    unlinkat(diagnostics_->fd, filename.c_str(), 0);
    return Result<void>::failure(error);
  }
  return Result<void>::success();
}
Result<std::vector<std::byte>>
NativeJournalDirectory::read_diagnostic(std::string_view name, std::uint64_t now) {
  const auto expires = expiration(name);
  if (!expires)
    return Result<std::vector<std::byte>>::failure({ErrorCode::invalid_range});
  if (*expires <= now)
    return Result<std::vector<std::byte>>::failure({ErrorCode::stale_handle});
  if (!diagnostics_) {
    auto opened = DiagnosticStore::open(descriptor_);
    if (!opened.has_value())
      return Result<std::vector<std::byte>>::failure(opened.error());
    diagnostics_ = std::move(opened).value();
  }
  const std::string filename{name};
  const int file =
      openat(diagnostics_->fd, filename.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
  if (file < 0)
    return Result<std::vector<std::byte>>::failure(
        {errno == ENOENT ? ErrorCode::stale_handle : ErrorCode::io, errno});
  struct stat s{};
  if (fstat(file, &s) < 0 || !private_file(s) || s.st_size < 0 || s.st_size > 65536) {
    close(file);
    return Result<std::vector<std::byte>>::failure({ErrorCode::io, EPERM});
  }
  try {
    std::vector<std::byte> result(static_cast<std::size_t>(s.st_size));
    std::size_t offset = 0;
    while (offset < result.size()) {
      auto n = read(file, result.data() + offset, result.size() - offset);
      if (n < 0 && errno == EINTR)
        continue;
      if (n <= 0) {
        const int e = errno;
        close(file);
        return Result<std::vector<std::byte>>::failure({ErrorCode::io, e});
      }
      offset += static_cast<std::size_t>(n);
    }
    close(file);
    return Result<std::vector<std::byte>>::success(std::move(result));
  } catch (const std::bad_alloc &) {
    close(file);
    return Result<std::vector<std::byte>>::failure({ErrorCode::allocation});
  }
}
} // namespace blackbird
