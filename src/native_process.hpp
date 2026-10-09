#pragma once
#include "blackbird/foundation.hpp"
#include "blackbird/process_lifetime.hpp"
#include <algorithm>
#include <cerrno>
#include <charconv>
#include <chrono>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <poll.h>
#include <signal.h>
#include <spawn.h>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#ifndef F_SETNOSIGPIPE
#include <pthread.h>
#endif

extern char **environ;
namespace blackbird::detail {
[[noreturn]] inline void fail(ErrorCode code, std::int64_t detail = 0) {
  throw Error{code, detail};
}
using Clock = std::chrono::steady_clock;
#ifndef F_SETNOSIGPIPE
// Suppress only this thread's new SIGPIPE while writing an owned child pipe.
class PipeSignalGuard {
public:
  PipeSignalGuard() {
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGPIPE);
    const int rc = pthread_sigmask(SIG_BLOCK, &blocked, &previous);
    if (rc != 0)
      fail(ErrorCode::io, rc);
    sigset_t pending;
    sigpending(&pending);
    was_pending = sigismember(&pending, SIGPIPE) == 1;
  }
  ~PipeSignalGuard() {
    if (!was_pending) {
      timespec zero{};
      while (sigtimedwait(&blocked, nullptr, &zero) < 0 && errno == EINTR) {
      }
    }
    (void)pthread_sigmask(SIG_SETMASK, &previous, nullptr);
  }

private:
  sigset_t blocked{}, previous{};
  bool was_pending = false;
};
#endif
class Child {
public:
  std::function<void(std::string_view)> output_observer;
  std::function<bool()> cancelled;
  std::function<void(std::string_view)> error_observer;
  std::string error_bytes;
  std::filesystem::path cwd;
  std::vector<std::string> child_environment;
  bool separate_error = false;
  bool spawned() const noexcept { return group > 0; }
  Error deadline_error{ErrorCode::io, ETIMEDOUT};
  Child() = default;
  Child(const Child &) = delete;
  Child &operator=(const Child &) = delete;
  ~Child() {
    close_input();
    if (output >= 0)
      (void)::close(output);
    if (error_output >= 0)
      (void)::close(error_output);
    // Keep group custody even after collect() reaps the leader.
    if (group > 0)
      (void)::kill(-group, SIGKILL);
    if (pid > 0) {
      while (::waitpid(pid, nullptr, 0) < 0 && errno == EINTR) {
      }
    }
    if (group > 0) {
      if (::kill(-group, 0) == 0 || errno != ESRCH)
        uncontained_exec.store(true); // descendants not observed gone
      --owned_children;
    }
  }
  void start(std::vector<std::string> args,
             const std::filesystem::path &codex_home = {}, bool merge_error = false) {
    if (cancelled && cancelled())
      fail(ErrorCode::interrupted);
    std::vector<char *> argv;
    for (auto &arg : args)
      argv.push_back(arg.data());
    argv.push_back(nullptr);
    std::vector<std::string> environment;
    std::vector<char *> envp;
    if (!child_environment.empty()) {
      environment = child_environment;
      for (auto &entry : environment)
        envp.push_back(entry.data());
      envp.push_back(nullptr);
    } else if (!codex_home.empty()) {
      for (char **entry = environ; *entry != nullptr; ++entry)
        if (!std::string_view{*entry}.starts_with("CODEX_HOME="))
          environment.emplace_back(*entry);
      environment.push_back("CODEX_HOME=" + codex_home.string());
      for (auto &entry : environment)
        envp.push_back(entry.data());
      envp.push_back(nullptr);
    }
    int in[2]{-1, -1}, out[2]{-1, -1}, err[2]{-1, -1};
    // Normalize descriptors above stdio even if the parent started with closed stdio.
    auto pipe = [](int (&fds)[2]) {
      if (::pipe(fds) != 0)
        fail(ErrorCode::io, errno);
      for (auto &fd : fds) {
        const int owned = ::fcntl(fd, F_DUPFD_CLOEXEC, 3);
        if (owned < 0) {
          const int e = errno;
          for (int old : fds)
            if (old >= 0)
              (void)::close(old);
          fail(ErrorCode::io, e);
        }
        (void)::close(fd);
        fd = owned;
      }
    };
    pipe(in);
    try {
      pipe(out);
      if (separate_error)
        pipe(err);
    } catch (...) {
      for (int fd : in)
        (void)::close(fd);
      for (int fd : out)
        if (fd >= 0)
          (void)::close(fd);
      throw;
    }
    posix_spawn_file_actions_t actions;
    posix_spawnattr_t attributes;
    int rc = ::posix_spawn_file_actions_init(&actions);
    if (rc == 0) {
      rc = ::posix_spawnattr_init(&attributes);
      if (rc == 0) {
        rc = ::posix_spawn_file_actions_adddup2(&actions, in[0], STDIN_FILENO);
        if (rc == 0)
          rc = ::posix_spawn_file_actions_adddup2(&actions, out[1], STDOUT_FILENO);
        if (rc == 0)
          rc = separate_error
                   ? ::posix_spawn_file_actions_adddup2(&actions, err[1], STDERR_FILENO)
               : merge_error
                   ? ::posix_spawn_file_actions_adddup2(&actions, out[1], STDERR_FILENO)
                   : ::posix_spawn_file_actions_addopen(&actions, STDERR_FILENO,
                                                        "/dev/null", O_WRONLY, 0);
        if (rc == 0 && !cwd.empty())
#ifdef __APPLE__
          rc = ::posix_spawn_file_actions_addchdir(&actions, cwd.c_str());
#else
          rc = ::posix_spawn_file_actions_addchdir_np(&actions, cwd.c_str());
#endif
        if (rc == 0)
          rc = ::posix_spawnattr_setflags(&attributes, POSIX_SPAWN_SETPGROUP);
        if (rc == 0)
          rc = ::posix_spawnattr_setpgroup(&attributes, 0);
        if (rc == 0)
          rc = ::posix_spawnp(&pid, argv[0], &actions, &attributes, argv.data(),
                              envp.empty() ? environ : envp.data());
        (void)::posix_spawnattr_destroy(&attributes);
      }
      (void)::posix_spawn_file_actions_destroy(&actions);
    }
    (void)::close(in[0]);
    (void)::close(out[1]);
    if (err[1] >= 0)
      (void)::close(err[1]);
    if (rc != 0) {
      (void)::close(in[1]);
      (void)::close(out[0]);
      if (err[0] >= 0)
        (void)::close(err[0]);
      pid = -1;
      fail(ErrorCode::io, rc);
    }
    group = pid;
    ++owned_children;
    input = in[1];
    output = out[0];
    error_output = err[0];
    if (error_output >= 0 && ::fcntl(error_output, F_SETFL, O_NONBLOCK) < 0)
      fail(ErrorCode::io, errno);
    // Socket-style SIGPIPE suppression exists for pipes on the qualified Mac profile.
#ifdef F_SETNOSIGPIPE
    if (::fcntl(input, F_SETNOSIGPIPE, 1) < 0)
      fail(ErrorCode::io, errno);
#endif
    if (::fcntl(input, F_SETFL, O_NONBLOCK) < 0 ||
        ::fcntl(output, F_SETFL, O_NONBLOCK) < 0)
      fail(ErrorCode::io, errno);
  }
  void close_input() {
    if (input >= 0) {
      (void)::close(input);
      input = -1;
    }
  }
  void send(std::string_view bytes, Clock::time_point deadline) {
#ifndef F_SETNOSIGPIPE
    PipeSignalGuard signals;
#endif
    while (!bytes.empty()) {
      wait(input, POLLOUT, deadline);
      const auto n = ::write(input, bytes.data(), bytes.size());
      if (n > 0)
        bytes.remove_prefix(static_cast<std::size_t>(n));
      else if (n < 0 && errno != EINTR && errno != EAGAIN)
        fail(ErrorCode::io, errno);
    }
  }
  std::string line(Clock::time_point deadline, std::size_t limit) {
    while (true) {
      const auto end = pending.find('\n');
      if (end != std::string::npos) {
        auto s = pending.substr(0, end);
        pending.erase(0, end + 1);
        return s;
      }
      if (!read(deadline, limit))
        fail(ErrorCode::incomplete);
    }
  }
  std::string collect(Clock::time_point deadline, std::size_t limit,
                      int *exit_code = nullptr) {
    while (read(deadline, limit)) {
    }
    siginfo_t observed{};
    while (true) {
      if (cancelled && cancelled())
        fail(ErrorCode::interrupted);
      // Observe exit without reaping: the zombie leader pins its PID/group name
      // until owned group cleanup. Never kill a possibly recycled PGID.
      const int rc = ::waitid(P_PID, static_cast<id_t>(pid), &observed,
                              WEXITED | WNOHANG | WNOWAIT);
      if (rc == 0 && observed.si_pid == pid)
        break;
      if (rc < 0 && errno != EINTR)
        fail(ErrorCode::io, errno);
      if (Clock::now() >= deadline)
        throw deadline_error;
      (void)::poll(nullptr, 0, 10);
    }
    const int code =
        observed.si_code == CLD_EXITED ? observed.si_status : 128 + observed.si_status;
    if (exit_code != nullptr) {
      *exit_code = code;
      return std::move(pending);
    }
    if (observed.si_code != CLD_EXITED || code != 0)
      fail(ErrorCode::io, observed.si_code == CLD_EXITED ? code : -1);
    return std::move(pending);
  }

  std::string take_partial() noexcept { return std::move(pending); }

private:
  pid_t pid = -1, group = -1;
  int input = -1, output = -1, error_output = -1;
  bool stdout_done = false;
  std::string pending;
  void wait(int fd, short events, Clock::time_point deadline) {
    if (cancelled && cancelled())
      fail(ErrorCode::interrupted);
    const auto remaining =
        std::chrono::duration_cast<std::chrono::milliseconds>(deadline - Clock::now())
            .count();
    if (remaining <= 0)
      throw deadline_error;
    pollfd descriptor{fd, events, 0};
    const int rc = ::poll(&descriptor, 1,
                          static_cast<int>(std::min<std::int64_t>(remaining, 100)));
    if (rc < 0 && errno != EINTR)
      fail(ErrorCode::io, errno);
    if (rc > 0 && (descriptor.revents & POLLNVAL) != 0)
      fail(ErrorCode::io);
  }
  bool read(Clock::time_point deadline, std::size_t limit) {
    if (error_output >= 0) {
      wait(error_output, POLLIN, deadline);
      char b[8192];
      const auto n = ::read(error_output, b, sizeof(b));
      if (n == 0) {
        (void)::close(error_output);
        error_output = -1;
      } else if (n > 0) {
        if (error_observer)
          error_observer(std::string_view{b, static_cast<std::size_t>(n)});
        if (error_bytes.size() + static_cast<std::size_t>(n) > limit)
          fail(ErrorCode::capacity);
        error_bytes.append(b, static_cast<std::size_t>(n));
      } else if (errno != EAGAIN && errno != EINTR)
        fail(ErrorCode::io, errno);
    }
    if (stdout_done)
      return error_output >= 0;
    wait(output, POLLIN, deadline);
    char bytes[8192];
    const auto n = ::read(output, bytes, sizeof(bytes));
    if (n == 0) {
      stdout_done = true;
      return error_output >= 0;
    }
    if (n < 0) {
      if (errno != EAGAIN && errno != EINTR)
        fail(ErrorCode::io, errno);
      return true;
    }
    const auto count = static_cast<std::size_t>(n);
    if (output_observer)
      output_observer(std::string_view{bytes, count});
    if (pending.size() > limit || count > limit - pending.size())
      fail(ErrorCode::capacity);
    pending.append(bytes, count);
    return true;
  }
};
} // namespace blackbird::detail
