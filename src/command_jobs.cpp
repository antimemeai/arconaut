#include "blackbird/command_jobs.hpp"
#include "blackbird/foundation.hpp"
#include "blackbird/json.hpp"
#include "blackbird/process_lifetime.hpp"
#include "blackbird/tools.hpp"
#include <algorithm>
#include <array>
#include <cerrno>
#include <charconv>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <fcntl.h>
#include <map>
#include <mutex>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <spawn.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <utility>
extern char **environ;
namespace blackbird {
namespace {
using Clock = std::chrono::steady_clock;
constexpr std::size_t pending_limit = 1024 * 1024;
constexpr std::size_t output_limit = 16 * 1024 * 1024;
constexpr std::size_t input_limit = 65536;
std::uint64_t integer(const Value &v, std::string_view key, std::uint64_t fallback) {
  const auto *p = v.find(key);
  if (!p)
    return fallback;
  if (!std::holds_alternative<Number>(p->value()))
    throw Error{ErrorCode::invalid_range};
  const auto &s = p->number().text();
  std::uint64_t n = 0;
  const auto r = std::from_chars(s.data(), s.data() + s.size(), n);
  if (r.ec != std::errc{} || r.ptr != s.data() + s.size())
    throw Error{ErrorCode::invalid_range};
  return n;
}
void validate_control(const Value &value, bool mailbox) {
  if (!std::holds_alternative<Value::Object>(value.value()))
    throw Error{ErrorCode::invalid_range};
  const auto *operation = value.find("op");
  if (!operation || !std::holds_alternative<std::string>(operation->value()))
    throw Error{ErrorCode::invalid_range};
  const auto &op = operation->string();
  if (op != "read" && op != "foreground" && op != "background" && op != "input" &&
      op != "resize" && op != "signal" && op != "stop" && op != "archive" &&
      op != "configure")
    throw Error{ErrorCode::unsupported};
  if (mailbox && op == "foreground")
    throw Error{ErrorCode::busy};
  for (const auto &[key, item] : value.object()) {
    const bool allowed =
        key == "op" || key == "job_id" || key == "request_id" ||
        (op == "read" && (key == "count" || key == "offset")) ||
        (op == "foreground" && (key == "yield_ms" || key == "output_max_bytes")) ||
        (op == "background" && key == "wait_epoch") ||
        (op == "input" && key == "bytes") ||
        (op == "resize" && (key == "rows" || key == "columns")) ||
        (op == "signal" && key == "signal") ||
        (op == "configure" && (key == "max_live" || key == "max_records"));
    if (!allowed)
      throw Error{ErrorCode::invalid_range};
    if (key == "op" || key == "job_id" || key == "request_id" || key == "bytes" ||
        key == "signal") {
      if (!std::holds_alternative<std::string>(item.value()))
        throw Error{ErrorCode::invalid_range};
      const auto bound = key == "bytes"        ? input_limit
                         : key == "request_id" ? 128U
                         : key == "job_id"     ? 32U
                                               : 16U;
      if (item.string().size() > bound)
        throw Error{ErrorCode::invalid_range};
    } else {
      const auto bound = key == "count"                      ? 65536ULL
                         : key == "yield_ms"                 ? 3600000ULL
                         : key == "output_max_bytes"         ? output_limit
                         : key == "rows" || key == "columns" ? 512ULL
                         : key == "max_live"                 ? 64ULL
                         : key == "max_records"              ? 256ULL
                                                             : UINT64_MAX;
      if (integer(value, key, 0) > bound)
        throw Error{ErrorCode::invalid_range};
    }
  }
  if (op == "input" && !value.find("bytes"))
    throw Error{ErrorCode::invalid_range};
  if (op == "signal" && !value.find("signal"))
    throw Error{ErrorCode::invalid_range};
  if (!mailbox && (op == "input" || op == "resize" || op == "signal") &&
      (!value.find("request_id") || string_field(value, "request_id").empty()))
    throw Error{ErrorCode::invalid_range};
}
bool same_request(const Value &left, const Value &right) {
  if (left.value().index() != right.value().index())
    return false;
  if (std::holds_alternative<Value::Object>(left.value())) {
    if (left.object().size() != right.object().size())
      return false;
    for (const auto &[key, value] : left.object()) {
      const auto *other = right.find(key);
      if (!other || !same_request(value, *other))
        return false;
    }
    return true;
  }
  if (std::holds_alternative<Value::Array>(left.value())) {
    if (left.array().size() != right.array().size())
      return false;
    for (std::size_t i = 0; i < left.array().size(); ++i)
      if (!same_request(left.array()[i], right.array()[i]))
        return false;
    return true;
  }
  return left == right;
}
bool boolean(const Value &v, std::string_view key, bool fallback = false) {
  const auto *p = v.find(key);
  if (!p)
    return fallback;
  if (!std::holds_alternative<bool>(p->value()))
    throw Error{ErrorCode::invalid_range};
  return std::get<bool>(p->value());
}
void put(Value &v, std::string key, Value item) {
  for (auto &[old, value] : v.object())
    if (old == key) {
      value = std::move(item);
      return;
    }
  v.object().emplace_back(std::move(key), std::move(item));
}
struct Fd {
  int value = -1;
  Fd() = default;
  explicit Fd(int descriptor) : value(descriptor) {}
  ~Fd() { close(); }
  Fd(const Fd &) = delete;
  Fd &operator=(const Fd &) = delete;
  void close() noexcept {
    if (value >= 0)
      (void)::close(std::exchange(value, -1));
  }
};
void pipe_fds(Fd &reader, Fd &writer) {
  int fds[2];
  if (::pipe(fds) != 0)
    throw Error{ErrorCode::io, errno};
  reader.value = fds[0];
  writer.value = fds[1];
  for (auto *fd : {&reader, &writer}) {
    const int normalized = ::fcntl(fd->value, F_DUPFD_CLOEXEC, 4);
    if (normalized < 0)
      throw Error{ErrorCode::io, errno};
    fd->close();
    fd->value = normalized;
  }
}
void nonblocking(int descriptor) {
  const int flags = ::fcntl(descriptor, F_GETFL);
  if (flags < 0 || ::fcntl(descriptor, F_SETFL, flags | O_NONBLOCK) < 0)
    throw Error{ErrorCode::io, errno};
}
struct Launch {
  std::vector<std::string> argv;
  bool pty = false;
  std::chrono::seconds lifetime{120};
};
Launch parse_launch(const Value &arguments) {
  if (!std::holds_alternative<Value::Object>(arguments.value()))
    throw Error{ErrorCode::invalid_range};
  Launch out;
  out.pty = boolean(arguments, "pty");
  const auto seconds = integer(arguments, "timeout_seconds", 120);
  if (seconds < 1 || seconds > 3600)
    throw Error{ErrorCode::invalid_range};
  out.lifetime = std::chrono::seconds{seconds};
  if (const auto *argv = arguments.find("argv")) {
    for (const auto &arg : argv->array()) {
      if (arg.string().find('\0') != std::string::npos)
        throw Error{ErrorCode::invalid_range};
      out.argv.push_back(arg.string());
    }
  }
  if (out.argv.empty()) {
    const auto command = string_field(arguments, "command");
    if (command.find('\0') != std::string::npos)
      throw Error{ErrorCode::invalid_range};
    out.argv = {"/bin/sh", "-c", command};
  }
  (void)integer(arguments, "output_max_bytes", output_limit);
  if (integer(arguments, "yield_ms", 0) > 3600000)
    throw Error{ErrorCode::invalid_range};
  return out;
}
} // namespace
struct CommandJobs::Impl {
  struct Delivery {
    Value request;
    const std::string &bytes() const { return string_field(request, "bytes"); }
    std::size_t written = 0;
    bool done = false;
    bool failed = false;
  };
  struct Chunk {
    std::size_t offset;
    std::shared_ptr<const std::string> bytes;
  };
  struct Job {
    std::string id;
    Launch launch;
    std::jthread worker;
    pid_t pid = -1;
    bool ready = false, stopped = false, leader_exited = false;
    bool output_closed = false, finished = false, terminal_sent = false;
    bool terminal_retained = false, stop = false, timed_out = false;
    std::size_t received = 0, retained = 0, pending_bytes = 0, tail_bytes = 0;
    std::size_t input_bytes = 0, input_history_bytes = 0;
    int exit_code = 0, signal = 0, failure = 0;
    std::string termination = "unobserved";
    bool capture_complete = true;
    std::deque<Chunk> pending, tail;
    std::map<std::string, Delivery> deliveries;
    std::deque<std::string> input, signals;
    Clock::time_point deadline;
    Value terminal;
    std::uint64_t release_epoch = 0;
    unsigned short rows = 24, columns = 80;
    bool resize_pending = false;
  };
  mutable std::mutex mutex;
  std::condition_variable condition;
  std::map<std::string, std::shared_ptr<Job>> jobs;
  std::deque<Value> mailbox;
  std::string waiting;
  std::uint64_t epoch = 0, request_counter = 0;
  bool text_input = false;
  std::size_t max_live = 8, max_records = 32;
  Job &find(const std::string &id) const {
    const auto i = jobs.find(id);
    if (i == jobs.end())
      throw Error{ErrorCode::invalid_range};
    return *i->second;
  }
  std::size_t debt_locked() const {
    return static_cast<std::size_t>(
        std::count_if(jobs.begin(), jobs.end(), [](const auto &entry) {
          return !entry.second->terminal_retained;
        }));
  }
  struct View {
    std::size_t max_output = 0, offset = 0;
    bool output = false;
  };
  Value snapshot(const Job &j, View view) const {
    const auto max_output = view.max_output, offset = view.offset;
    Value result =
        Value::object({{"job_id", Value{j.id}},
                       {"output_ref", Value{j.id}},
                       {"state", Value{j.terminal_retained ? "completed"
                                       : j.finished        ? "settling"
                                       : j.leader_exited   ? "draining"
                                       : j.stopped         ? "stopped"
                                       : j.ready           ? "running"
                                                           : "starting"}},
                       {"pty", Value{j.launch.pty}},
                       {"launches", Value{Number{1}}},
                       {"received_bytes", Value{Number{j.received}}},
                       {"retained_bytes", Value{Number{j.retained}}},
                       {"sealed", Value{j.terminal_retained && j.capture_complete}},
                       {"leader_exited", Value{j.leader_exited}},
                       {"output_closed", Value{j.output_closed}}});
    if (j.pid > 0)
      put(result, "pid", Value{Number{j.pid}});
    if (j.terminal_retained)
      for (const auto &[key, value] : j.terminal.object())
        put(result, key, value);
    if (view.output) {
      const auto first = j.tail.empty() ? j.retained : j.tail.front().offset;
      const auto begin = std::min(j.retained, std::max(offset, first));
      std::string bytes;
      for (const auto &chunk : j.tail) {
        const auto end = std::min(chunk.offset + chunk.bytes->size(), j.retained);
        if (end <= begin || bytes.size() >= max_output)
          continue;
        const auto at = std::max(chunk.offset, begin);
        bytes.append(*chunk.bytes, at - chunk.offset,
                     std::min(end - at, max_output - bytes.size()));
      }
      const auto shown = process_output_presentation(bytes, Value::object({}));
      put(result, "output", field(shown, "output"));
      put(result, "returned_bytes", Value{Number{bytes.size()}});
      put(result, "omitted_bytes", Value{Number{j.retained - bytes.size()}});
      put(result, "byte_start", Value{Number{begin}});
      put(result, "next_offset", Value{Number{begin + bytes.size()}});
      put(result, "omitted_prefix_bytes",
          Value{Number{begin - std::min(offset, begin)}});
    }
    return result;
  }
  void run(const std::shared_ptr<Job> &job) noexcept;
};
void CommandJobs::Impl::run(const std::shared_ptr<Job> &job) noexcept {
  sigset_t blocked;
  sigemptyset(&blocked);
  sigaddset(&blocked, SIGWINCH);
  (void)::pthread_sigmask(SIG_BLOCK, &blocked, nullptr);
  Fd input_read, input_write, output_read, output_write, boot_read, boot_write;
  pid_t pid = -1;
  bool group_ready = false, counted = false, reaped = false;
  bool channel_closed = false;
  const auto deadline = job->deadline;
  std::optional<Clock::time_point> stopping;
  bool killed = false;
  std::array<unsigned char, sizeof(int) + 2> bootstrap{};
  std::size_t bootstrap_size = 0;
  auto signal_owned = [&](int number) {
    if (pid <= 0 || reaped)
      return;
    if (!group_ready || ::kill(-pid, number) != 0 || ::getpgid(pid) != pid)
      (void)::kill(pid, number);
  };
  try {
    std::vector<std::string> arguments = job->launch.argv;
    {
      const std::lock_guard spawn_lock{detail::process_spawn_mutex};
      std::string slave;
      if (job->launch.pty) {
        Fd master{::posix_openpt(O_RDWR | O_NOCTTY)};
        if (master.value < 0 || ::grantpt(master.value) != 0 ||
            ::unlockpt(master.value) != 0)
          throw Error{ErrorCode::io, errno};
        const char *name = ::ptsname(master.value);
        if (!name)
          throw Error{ErrorCode::io, errno};
        slave = name;
        output_read.value = ::fcntl(master.value, F_DUPFD_CLOEXEC, 4);
        if (output_read.value < 0)
          throw Error{ErrorCode::io, errno};
        input_write.value = ::fcntl(output_read.value, F_DUPFD_CLOEXEC, 4);
        if (input_write.value < 0)
          throw Error{ErrorCode::io, errno};
        pipe_fds(boot_read, boot_write);
        arguments.insert(arguments.begin(), slave);
        arguments.insert(arguments.begin(), BLACKBIRD_JOB_RUNNER);
      } else {
        pipe_fds(input_read, input_write);
        pipe_fds(output_read, output_write);
      }
      nonblocking(output_read.value);
      nonblocking(input_write.value);
      if (boot_read.value >= 0)
        nonblocking(boot_read.value);
#ifdef F_SETNOSIGPIPE
      if (::fcntl(input_write.value, F_SETNOSIGPIPE, 1) < 0)
        throw Error{ErrorCode::io, errno};
#endif
      posix_spawn_file_actions_t actions;
      posix_spawnattr_t attributes;
      int rc = ::posix_spawn_file_actions_init(&actions);
      if (rc != 0)
        throw Error{ErrorCode::io, rc};
      struct Actions {
        posix_spawn_file_actions_t &value;
        ~Actions() { (void)::posix_spawn_file_actions_destroy(&value); }
      } cleanup_actions{actions};
      rc = ::posix_spawnattr_init(&attributes);
      if (rc != 0)
        throw Error{ErrorCode::io, rc};
      struct Attributes {
        posix_spawnattr_t &value;
        ~Attributes() { (void)::posix_spawnattr_destroy(&value); }
      } cleanup_attributes{attributes};
      auto check = [](int error) {
        if (error != 0)
          throw Error{ErrorCode::io, error};
      };
      sigset_t empty, defaults;
      sigemptyset(&empty);
      sigemptyset(&defaults);
      for (int number : {SIGINT, SIGQUIT, SIGTERM, SIGHUP, SIGPIPE, SIGCHLD, SIGTSTP,
                         SIGTTIN, SIGTTOU, SIGUSR1, SIGUSR2, SIGWINCH})
        sigaddset(&defaults, number);
      check(::posix_spawnattr_setsigmask(&attributes, &empty));
      check(::posix_spawnattr_setsigdefault(&attributes, &defaults));
      short flags = POSIX_SPAWN_SETSIGMASK | POSIX_SPAWN_SETSIGDEF;
      if (!job->launch.pty) {
        flags = static_cast<short>(flags | POSIX_SPAWN_SETPGROUP);
        check(::posix_spawnattr_setpgroup(&attributes, 0));
        check(::posix_spawn_file_actions_adddup2(&actions, input_read.value, 0));
        check(::posix_spawn_file_actions_adddup2(&actions, output_write.value, 1));
        check(::posix_spawn_file_actions_adddup2(&actions, output_write.value, 2));
      } else {
        check(::posix_spawn_file_actions_adddup2(&actions, boot_write.value, 3));
        for (int destination = 0; destination < 3; ++destination)
          check(::posix_spawn_file_actions_addopen(&actions, destination, "/dev/null",
                                                   destination ? O_WRONLY : O_RDONLY,
                                                   0));
      }
      check(::posix_spawnattr_setflags(&attributes, flags));
      std::vector<char *> argv;
      for (auto &argument : arguments)
        argv.push_back(argument.data());
      argv.push_back(nullptr);
      check(::posix_spawnp(&pid, argv.front(), &actions, &attributes, argv.data(),
                           environ));
      group_ready = !job->launch.pty;
      ++detail::owned_children;
      counted = true;
      detail::uncontained_exec.store(true);
      input_read.close();
      output_write.close();
      boot_write.close();
      if (!job->launch.pty)
        input_write.close();
    }
    {
      const std::lock_guard lock{mutex};
      job->pid = pid;
      job->ready = group_ready;
      condition.notify_all();
    }
    while (true) {
      bool stop = false;
      bool exited = false;
      std::size_t available = 0;
      {
        const std::lock_guard lock{mutex};
        if (Clock::now() >= deadline) {
          job->stop = true;
          job->timed_out = true;
        }
        stop = job->stop;
        exited = job->leader_exited;
        available = pending_limit - job->pending_bytes;
      }
      if (stop && !stopping) {
        stopping = Clock::now();
        signal_owned(SIGTERM);
      }
      if (stopping && !killed &&
          Clock::now() - *stopping >= std::chrono::milliseconds{200}) {
        signal_owned(SIGKILL);
        killed = true;
      }
      if (boot_read.value >= 0) {
        const auto n = ::read(boot_read.value, bootstrap.data() + bootstrap_size,
                              bootstrap.size() - bootstrap_size);
        if (n > 0)
          bootstrap_size += static_cast<std::size_t>(n);
        else if (n == 0) {
          if (!group_ready || bootstrap_size != 0)
            throw Error{ErrorCode::io, EPROTO};
          boot_read.close();
        } else if (errno != EAGAIN && errno != EINTR)
          throw Error{ErrorCode::io, errno};
        if (bootstrap_size && bootstrap[0] == 1) {
          if (group_ready)
            throw Error{ErrorCode::io, EPROTO};
          group_ready = true;
          --bootstrap_size;
          std::memmove(bootstrap.data(), bootstrap.data() + 1, bootstrap_size);
          const std::lock_guard lock{mutex};
          job->ready = true;
          job->resize_pending = true;
        }
        if (bootstrap_size) {
          if (bootstrap[0] != 2)
            throw Error{ErrorCode::io, EPROTO};
          if (bootstrap_size >= sizeof(int) + 1) {
            int failure = EIO;
            std::memcpy(&failure, bootstrap.data() + 1, sizeof(failure));
            const std::lock_guard lock{mutex};
            job->failure = failure > 0 ? failure : EPROTO;
            job->stop = true;
            boot_read.close();
          }
        }
      }

      {
        const std::lock_guard lock{mutex};
        if (group_ready && job->launch.pty && job->resize_pending) {
          winsize size{job->rows, job->columns, 0, 0};
          if (::ioctl(output_read.value, TIOCSWINSZ, &size) != 0)
            job->failure = errno;
          job->resize_pending = false;
        }
        if (!job->signals.empty()) {
          auto &delivery = job->deliveries.at(job->signals.front());
          const auto name = string_field(delivery.request, "signal");
          const int number = name == "stop"        ? SIGSTOP
                             : name == "continue"  ? SIGCONT
                             : name == "interrupt" ? SIGINT
                             : name == "terminate" ? SIGTERM
                                                   : SIGKILL;
          delivery.failed = job->leader_exited;
          if (!delivery.failed)
            signal_owned(number);
          delivery.done = true;
          job->signals.pop_front();
        }
        if (!job->input.empty()) {
          auto &delivery = job->deliveries.at(job->input.front());
          const auto op = string_field(delivery.request, "op");
          if (op == "input" && group_ready) {
            const auto remaining = delivery.bytes().size() - delivery.written;
            const auto n =
                ::write(input_write.value, delivery.bytes().data() + delivery.written,
                        remaining);
            if (n > 0) {
              const auto size = static_cast<std::size_t>(n);
              delivery.written += size;
              job->input_bytes -= size;
            } else if (n < 0 && errno != EAGAIN && errno != EINTR) {
              delivery.failed = true;
              job->input_bytes -= remaining;
            }
            delivery.done =
                delivery.failed || delivery.written == delivery.bytes().size();
          } else if (op == "resize" && group_ready) {
            winsize size{
                static_cast<unsigned short>(integer(delivery.request, "rows", 24)),
                static_cast<unsigned short>(integer(delivery.request, "columns", 80)),
                0, 0};
            delivery.failed = ::ioctl(output_read.value, TIOCSWINSZ, &size) != 0;
            delivery.done = true;
          }
          if (delivery.done)
            job->input.pop_front();
        }
      }

      if (!channel_closed && available >= 8192) {
        char bytes[8192];
        const auto n = ::read(output_read.value, bytes, sizeof(bytes));
        if (n > 0) {
          auto chunk =
              std::make_shared<const std::string>(bytes, static_cast<std::size_t>(n));
          const std::lock_guard lock{mutex};
          if (chunk->size() > output_limit - std::min(output_limit, job->received)) {
            job->capture_complete = false;
            job->failure = EFBIG;
            job->stop = true;
          } else {
            const Chunk entry{job->received, std::move(chunk)};
            job->received += entry.bytes->size();
            job->pending_bytes += entry.bytes->size();
            job->tail_bytes += entry.bytes->size();
            job->pending.push_back(entry);
            job->tail.push_back(entry);
            while (job->tail_bytes > pending_limit && job->tail.size() > 1) {
              job->tail_bytes -= job->tail.front().bytes->size();
              job->tail.pop_front();
            }
            condition.notify_all();
          }
        } else if (n == 0 ||
                   (n < 0 && errno == EIO && job->launch.pty && group_ready)) {
          channel_closed = true;
          output_read.close();
          const std::lock_guard lock{mutex};
          job->output_closed = true;
        } else if (n < 0 && errno != EAGAIN && errno != EINTR &&
                   !(errno == EIO && job->launch.pty && !group_ready)) {
          throw Error{ErrorCode::io, errno};
        }
      }
      if (!exited) {
        siginfo_t observed{};
        const int rc = ::waitid(P_PID, static_cast<id_t>(pid), &observed,
                                WEXITED | WNOHANG | WNOWAIT);
        if (rc < 0 && errno != EINTR)
          throw Error{ErrorCode::io, errno};
        if (rc == 0 && observed.si_pid == pid &&
            (observed.si_code == CLD_EXITED || observed.si_code == CLD_KILLED ||
             observed.si_code == CLD_DUMPED)) {
          const std::lock_guard lock{mutex};
          job->leader_exited = true;
          job->termination = observed.si_code == CLD_EXITED ? "exit" : "signal";
          job->signal = observed.si_code == CLD_EXITED ? 0 : observed.si_status;
          job->exit_code = observed.si_code == CLD_EXITED ? observed.si_status
                                                          : 128 + observed.si_status;
          exited = true;
        } else {
          siginfo_t state{};
          if (::waitid(P_PID, static_cast<id_t>(pid), &state,
                       WSTOPPED | WCONTINUED | WNOHANG) == 0 &&
              state.si_pid != 0 &&
              (state.si_code == CLD_STOPPED || state.si_code == CLD_CONTINUED)) {
            const std::lock_guard lock{mutex};
            job->stopped = state.si_code == CLD_STOPPED;
          }
        }
      }
      if (stopping && Clock::now() - *stopping >= std::chrono::milliseconds{500}) {
        if (!channel_closed) {
          channel_closed = true;
          output_read.close();
          const std::lock_guard lock{mutex};
          job->capture_complete = false;
          job->output_closed = true;
        }
      }
      if (exited && channel_closed && boot_read.value < 0)
        break;
      pollfd descriptors[2]{{available >= 8192 ? output_read.value : -1, POLLIN, 0},
                            {boot_read.value, POLLIN, 0}};
      (void)::poll(descriptors, 2, 10);
    }
    signal_owned(SIGKILL);
    while (::waitpid(pid, nullptr, 0) < 0 && errno == EINTR) {
    }
    reaped = true;
  } catch (const Error &error) {
    const std::lock_guard lock{mutex};
    job->failure = static_cast<int>(error.detail ? error.detail : EIO);
    job->capture_complete = false;
  } catch (...) {
    const std::lock_guard lock{mutex};
    job->failure = ENOMEM;
    job->capture_complete = false;
  }
  if (pid > 0 && !reaped) {
    signal_owned(SIGKILL);
    int status = 0;
    while (::waitpid(pid, &status, 0) < 0 && errno == EINTR) {
    }
  }
  if (counted)
    --detail::owned_children;
  const std::lock_guard lock{mutex};
  for (auto &[id, delivery] : job->deliveries) {
    (void)id;
    if (!delivery.done) {
      delivery.failed = true;
      delivery.done = true;
    }
  }
  job->finished = true;
  condition.notify_all();
}
CommandJobs::CommandJobs() : impl_(std::make_unique<Impl>()) {}
CommandJobs::~CommandJobs() { shutdown(); }
void CommandJobs::start(const std::string &id, const Value &arguments) {
  auto launch = parse_launch(arguments);
  if (id.size() != 32 || id.find_first_not_of("0123456789abcdef") != std::string::npos)
    throw Error{ErrorCode::invalid_range};
  auto job = std::make_shared<Impl::Job>();
  job->id = id;
  job->launch = std::move(launch);
  job->deadline = Clock::now() + job->launch.lifetime;
  sigset_t previous, blocked;
  sigemptyset(&blocked);
  sigaddset(&blocked, SIGWINCH);
  const int mask_error = ::pthread_sigmask(SIG_BLOCK, &blocked, &previous);
  if (mask_error)
    throw Error{ErrorCode::io, mask_error};
  struct RestoreMask {
    sigset_t previous;
    ~RestoreMask() { (void)::pthread_sigmask(SIG_SETMASK, &previous, nullptr); }
  } restore{previous};
  const std::lock_guard lock{impl_->mutex};
  if (impl_->jobs.contains(id))
    throw Error{ErrorCode::conflict};
  if (impl_->jobs.size() >= impl_->max_records ||
      impl_->debt_locked() >= impl_->max_live)
    throw Error{ErrorCode::capacity};
  impl_->jobs.emplace(id, job);
  try {
    job->worker = std::jthread([state = impl_.get(), job] { state->run(job); });
  } catch (...) {
    impl_->jobs.erase(id);
    throw;
  }
}
Value CommandJobs::wait(const std::string &id, JobWaitOptions options,
                        const std::function<void()> &tick,
                        const std::function<bool()> &cancelled) {
  std::uint64_t epoch = 0;
  {
    const std::lock_guard lock{impl_->mutex};
    auto &job = impl_->find(id);
    if (!impl_->waiting.empty())
      throw Error{ErrorCode::busy};
    impl_->waiting = id;
    epoch = ++impl_->epoch;
    impl_->text_input = job.launch.pty;
  }
  struct Release {
    Impl &state;
    ~Release() {
      const std::lock_guard lock{state.mutex};
      state.waiting.clear();
      state.text_input = false;
    }
  } release{*impl_};
  const auto deadline =
      options.yield_ms ? Clock::now() + std::chrono::milliseconds{*options.yield_ms}
                       : Clock::time_point::max();
  while (true) {
    tick();
    if (cancelled()) {
      if (options.stop_on_cancel) {
        std::shared_ptr<Impl::Job> stopped;
        {
          const std::lock_guard lock{impl_->mutex};
          stopped = impl_->jobs.at(id);
          stopped->stop = true;
        }
        if (stopped->worker.joinable())
          stopped->worker.join();
        // Cleanup progresses independently of capture backpressure; now retain
        // that command's actual terminal before propagating turn cancellation.
        for (unsigned pass = 0; pass < 4; ++pass)
          tick();
      }
      throw Error{ErrorCode::interrupted};
    }
    std::unique_lock lock{impl_->mutex};
    auto &job = impl_->find(id);
    if (job.terminal_retained || job.release_epoch == epoch || Clock::now() >= deadline)
      return impl_->snapshot(job, {options.max_output, 0, true});
    impl_->condition.wait_for(lock, std::chrono::milliseconds{10});
  }
}
Value CommandJobs::read(const Value &arguments) const {
  const std::lock_guard lock{impl_->mutex};
  const auto id =
      arguments.find("job_id") ? string_field(arguments, "job_id") : std::string{};
  if (!id.empty()) {
    if (integer(arguments, "count", 65536) > 65536 ||
        integer(arguments, "offset", 0) > impl_->find(id).retained)
      throw Error{ErrorCode::invalid_range};
    return impl_->snapshot(
        impl_->find(id),
        {static_cast<std::size_t>(
             std::min<std::uint64_t>(65536, integer(arguments, "count", 65536))),
         static_cast<std::size_t>(integer(arguments, "offset", 0)), true});
  }
  Value::Array rows;
  for (const auto &[key, job] : impl_->jobs) {
    (void)key;
    rows.push_back(impl_->snapshot(*job, {}));
  }
  return Value::object({{"jobs", Value{std::move(rows)}},
                        {"live", Value{Number{impl_->debt_locked()}}},
                        {"max_live", Value{Number{impl_->max_live}}},
                        {"max_records", Value{Number{impl_->max_records}}}});
}
Value CommandJobs::control(const Value &arguments) {
  validate_control(arguments, false);
  const auto op = string_field(arguments, "op");
  if (op == "read")
    return read(arguments);
  std::unique_lock lock{impl_->mutex};
  if (op == "configure") {
    if (impl_->debt_locked())
      throw Error{ErrorCode::busy};
    const auto live = integer(arguments, "max_live", impl_->max_live);
    const auto records = integer(arguments, "max_records", impl_->max_records);
    if (live < 1 || live > 64 || records < live || records > 256 ||
        records < impl_->jobs.size())
      throw Error{ErrorCode::invalid_range};
    impl_->max_live = static_cast<std::size_t>(live);
    impl_->max_records = static_cast<std::size_t>(records);
    return Value::object({{"configured", Value{true}}});
  }
  const auto id = string_field(arguments, "job_id");
  auto &job = impl_->find(id);
  if (op == "archive") {
    if (!job.terminal_retained || impl_->waiting == id ||
        std::any_of(
            impl_->mailbox.begin(), impl_->mailbox.end(),
            [&](const Value &entry) { return string_field(entry, "job_id") == id; }))
      throw Error{ErrorCode::busy};
    auto retained = impl_->jobs.at(id);
    impl_->jobs.erase(id);
    lock.unlock();
    if (retained->worker.joinable())
      retained->worker.join();
    return Value::object({{"archived", Value{true}}, {"job_id", Value{id}}});
  }
  if (op == "background") {
    const auto requested = integer(arguments, "wait_epoch", impl_->epoch);
    const bool released = impl_->waiting == id && requested == impl_->epoch;
    if (released)
      job.release_epoch = requested;
    impl_->condition.notify_all();
    return Value::object({{"job_id", Value{id}}, {"released", Value{released}}});
  }
  if (op == "stop") {
    job.stop = true;
    impl_->condition.notify_all();
    return Value::object({{"job_id", Value{id}}, {"stop_requested", Value{true}}});
  }
  const auto request = string_field(arguments, "request_id");
  if (request.empty() || request.size() > 128)
    throw Error{ErrorCode::invalid_range};
  if (const auto existing = job.deliveries.find(request);
      existing != job.deliveries.end()) {
    if (!same_request(existing->second.request, arguments))
      throw Error{ErrorCode::conflict};
    const auto &delivery = existing->second;
    return Value::object({{"job_id", Value{id}},
                          {"request_id", Value{request}},
                          {"written_bytes", Value{Number{delivery.written}}},
                          {"status", Value{delivery.failed ? "partial_or_unknown"
                                           : delivery.done ? "delivered"
                                                           : "queued"}},
                          {"duplicate", Value{true}}});
  }
  if (job.finished || job.deliveries.size() >= 4096)
    throw Error{job.finished ? ErrorCode::busy : ErrorCode::capacity};
  Impl::Delivery delivery{arguments, 0, false, false};
  if (op == "input") {
    if (!job.launch.pty)
      throw Error{ErrorCode::unsupported};
    if (delivery.bytes().size() > input_limit - job.input_bytes ||
        delivery.bytes().size() > output_limit - job.input_history_bytes)
      throw Error{ErrorCode::capacity};
    delivery.done = delivery.bytes().empty();
  } else if (op == "resize") {
    const auto rows = integer(arguments, "rows", 24);
    const auto columns = integer(arguments, "columns", 80);
    if (!job.launch.pty || rows < 1 || columns < 1 || rows > 512 || columns > 512)
      throw Error{ErrorCode::invalid_range};

  } else if (op == "signal") {
    const auto name = string_field(arguments, "signal");
    const int number = name == "stop"        ? SIGSTOP
                       : name == "continue"  ? SIGCONT
                       : name == "interrupt" ? SIGINT
                       : name == "terminate" ? SIGTERM
                       : name == "kill"      ? SIGKILL
                                             : 0;
    if (!number || job.pid <= 0 || job.leader_exited)
      throw Error{ErrorCode::busy};

  } else {
    throw Error{ErrorCode::unsupported};
  }
  const bool done = delivery.done;
  auto [inserted, fresh] = job.deliveries.emplace(request, std::move(delivery));
  (void)fresh;
  try {
    if (!done) {
      if (op == "signal")
        job.signals.push_back(request);
      else
        job.input.push_back(request);
    }
  } catch (...) {
    job.deliveries.erase(inserted);
    throw;
  }
  if (op == "input") {
    job.input_bytes += inserted->second.bytes().size();
    job.input_history_bytes += inserted->second.bytes().size();
  }
  impl_->condition.notify_all();
  return Value::object({{"job_id", Value{id}},
                        {"request_id", Value{request}},
                        {"status", Value{done ? "delivered" : "queued"}},
                        {"written_bytes", Value{Number{0}}},
                        {"duplicate", Value{false}}});
}
void CommandJobs::request(Value arguments) {
  validate_control(arguments, true);
  const std::lock_guard lock{impl_->mutex};
  const auto op = string_field(arguments, "op");
  const bool urgent = op == "background" || op == "stop";
  if (impl_->mailbox.size() >= (urgent ? 64U : 60U))
    throw Error{ErrorCode::capacity};
  auto id =
      arguments.find("job_id") ? string_field(arguments, "job_id") : std::string{};
  if (id.empty()) {
    id = impl_->waiting;
    if (id.empty())
      throw Error{ErrorCode::busy};
    put(arguments, "job_id", Value{id});
  }
  const auto &job = impl_->find(id);
  if (string_field(arguments, "op") == "input" &&
      (!job.launch.pty || string_field(arguments, "bytes").size() > input_limit))
    throw Error{ErrorCode::invalid_range};
  if (!arguments.find("request_id"))
    put(arguments, "request_id",
        Value{id + ":ui:" + std::to_string(++impl_->request_counter)});
  if (string_field(arguments, "op") == "background" && !arguments.find("wait_epoch"))
    put(arguments, "wait_epoch", Value{Number{impl_->epoch}});
  impl_->mailbox.push_back(std::move(arguments));
  impl_->condition.notify_all();
}
std::vector<Value> CommandJobs::requests() {
  const std::lock_guard lock{impl_->mutex};
  std::vector<Value> out;
  std::map<std::string, std::size_t> selected_bytes;
  std::map<std::string, std::size_t> selected_controls;
  for (auto entry = impl_->mailbox.begin(); entry != impl_->mailbox.end();) {
    const auto id = string_field(*entry, "job_id");
    auto &job = impl_->find(id);
    const auto op = string_field(*entry, "op");
    const bool control = op == "input" || op == "resize" || op == "signal";
    const auto bytes = op == "input" ? string_field(*entry, "bytes").size() : 0;
    const bool duplicate =
        control && job.deliveries.contains(string_field(*entry, "request_id"));
    if (!job.finished && control && !duplicate &&
        (job.deliveries.size() + selected_controls[id] >= 4096 ||
         bytes > input_limit - job.input_bytes - selected_bytes[id] ||
         bytes > output_limit - job.input_history_bytes - selected_bytes[id])) {
      ++entry;
      continue;
    }
    if (control && !duplicate) {
      selected_bytes[id] += bytes;
      ++selected_controls[id];
    }
    out.push_back(std::move(*entry));
    entry = impl_->mailbox.erase(entry);
  }
  return out;
}
std::vector<JobEvent> CommandJobs::drain() {
  const std::lock_guard lock{impl_->mutex};
  std::vector<JobEvent> out;
  for (const auto &[id, job] : impl_->jobs) {
    std::size_t count = 0;
    while (!job->pending.empty() && count++ < 64) {
      auto entry = std::move(job->pending.front());
      job->pending.pop_front();
      job->pending_bytes -= entry.bytes->size();
      out.push_back({id, std::move(entry.bytes), entry.offset, Value{}});
    }
    if (job->finished && job->pending.empty() && !job->terminal_sent) {
      job->terminal_sent = true;
      auto result = Value::object(
          {{"job_id", Value{id}},
           {"output_ref", Value{id}},
           {"termination", Value{job->termination}},
           {"signal", Value{Number{job->signal}}},
           {"output_bytes", Value{Number{job->received}}},
           {"capture_complete", Value{job->capture_complete}},
           {"timed_out", Value{job->timed_out}},
           {"stop_requested", Value{job->stop}},
           {"effect_outcome",
            Value{job->stop || job->failure || !job->capture_complete ? "unknown"
                                                                      : "observed"}}});
      if (job->leader_exited)
        put(result, "exit_code", Value{Number{job->exit_code}});
      if (job->failure)
        put(result, "error", Value{"io"});
      if (job->failure || job->timed_out) {
        put(result, "error", Value{"io"});
        put(result, "error_detail",
            Value{Number{job->timed_out ? ETIMEDOUT : job->failure}});
      }
      out.push_back({id, {}, job->received, std::move(result)});
    }
  }
  impl_->condition.notify_all();
  return out;
}
void CommandJobs::retained(const std::string &id, std::size_t end) {
  const std::lock_guard lock{impl_->mutex};
  auto &job = impl_->find(id);
  if (end < job.retained || end > job.received)
    throw Error{ErrorCode::conflict};
  job.retained = end;
}
void CommandJobs::capture_failed(const std::string &id) {
  const std::lock_guard lock{impl_->mutex};
  auto &job = impl_->find(id);
  job.capture_complete = false;
  job.stop = true;
  impl_->condition.notify_all();
}
void CommandJobs::settled(const std::string &id, const Value &result) {
  const std::lock_guard lock{impl_->mutex};
  auto &job = impl_->find(id);
  job.terminal = result;
  job.terminal_retained = true;
  impl_->condition.notify_all();
}
bool CommandJobs::contains(const std::string &id) const {
  const std::lock_guard lock{impl_->mutex};
  return impl_->jobs.contains(id);
}
bool CommandJobs::active() const { return debt() != 0; }
std::size_t CommandJobs::debt() const {
  const std::lock_guard lock{impl_->mutex};
  return impl_->debt_locked();
}
Value CommandJobs::foreground() const {
  const std::lock_guard lock{impl_->mutex};
  return Value::object({{"job_id", Value{impl_->waiting}},
                        {"wait_epoch", Value{Number{impl_->epoch}}},
                        {"pty", Value{impl_->text_input}}});
}
void CommandJobs::stop_all() noexcept {
  const std::lock_guard lock{impl_->mutex};
  for (auto &[id, job] : impl_->jobs) {
    (void)id;
    if (!job->finished)
      job->stop = true;
  }
  impl_->condition.notify_all();
}
void CommandJobs::shutdown() noexcept {
  stop_all();
  // Only the owning thread starts/archives/shuts down; workers never alter registry.
  for (const auto &[id, job] : impl_->jobs) {
    (void)id;
    if (job->worker.joinable())
      job->worker.join();
  }
}
} // namespace blackbird
