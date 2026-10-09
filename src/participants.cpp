#include "blackbird/participants.hpp"
#include "blackbird/packet.hpp"
#include "blackbird/tools.hpp"
#include <algorithm>
#include <atomic>
#include <charconv>
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <thread>
namespace blackbird {
namespace {
constexpr std::size_t max_runs = 32, max_messages = 64, max_inbox = 16;
constexpr std::size_t max_events = 1024, max_capture_bytes = 8 * 1024 * 1024;
void set(Value &value, std::string_view key, Value replacement) {
  for (auto &[name, item] : value.object())
    if (name == key) {
      item = std::move(replacement);
      return;
    }
  value.object().emplace_back(key, std::move(replacement));
}
void erase(Value &value, std::string_view key) {
  std::erase_if(value.object(), [&](const auto &entry) { return entry.first == key; });
}
void bounded(std::string_view text, std::size_t limit) {
  if (text.empty() || text.size() > limit || text.find('\0') != std::string_view::npos)
    throw Error{ErrorCode::invalid_range};
}
unsigned integer(const Value &value) {
  const auto &text = value.number().text();
  unsigned out = 0;
  const auto parsed = std::from_chars(text.data(), text.data() + text.size(), out);
  if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size())
    throw Error{ErrorCode::invalid_range};
  return out;
}
Value projection(const Value &result) {
  if (unwrap(encode_packet_string(result)).size() <= 65536)
    return result;
  return Value::object(
      {{"status", field(result, "status")},
       {"remote_disposition", field(result, "remote_disposition")},
       {"text", result.find("text")
                    ? Value{field(result, "text").string().substr(0, 8192)}
                    : Value{}},
       {"details",
        Value{"Read retained participant result original; projection truncated"}}});
}
} // namespace
struct Participants::Impl {
  struct Run {
    Value row;
    std::atomic_bool stop{false};
    bool sealed = false, finished = false;
    std::deque<Value> inbox;
    std::map<std::string, Value> receipts;
    std::jthread worker;
  };
  struct Capture {
    std::string label, raw;
    Value metadata;
  };
  mutable std::mutex mutex;
  std::condition_variable changed;
  std::map<std::string, std::unique_ptr<Run>> runs;
  std::deque<Capture> captures;
  std::size_t capture_bytes = 0;
  unsigned concurrency = 2;
  bool dirty = false;
  Retain retain;
  Save save;
  Publish publish;
  Impl(Retain r, Save s, Publish p)
      : retain(std::move(r)), save(std::move(s)), publish(std::move(p)) {}
  Run &find(const Value &args) {
    const auto found = runs.find(string_field(args, "run_id"));
    if (found == runs.end())
      throw Error{ErrorCode::invalid_range};
    return *found->second;
  }
  Value snapshot() const {
    Value::Array rows;
    for (const auto &[id, run] : runs)
      rows.push_back(run->row);
    return Value::object({{"label", Value{"participants-state-v1"}},
                          {"concurrency", Value{Number{concurrency}}},
                          {"max_runs", Value{Number{max_runs}}},
                          {"runs", Value{std::move(rows)}}});
  }
  void notify(const Value &row) noexcept {
    if (publish)
      try {
        publish(Value::object({{"attempt", field(row, "run_id")},
                               {"run_id", field(row, "run_id")},
                               {"task_id", field(row, "task_id")},
                               {"phase", field(row, "state")},
                               {"operation", Value{"participant"}},
                               {"cancel_requested", field(row, "cancel_requested")}}));
      } catch (...) { /* Presentation cannot change effects. */
      }
  }
  void work(Run &run, Value request, const ParticipantTransport &transport) noexcept {
    try {
      std::string run_id;
      {
        std::lock_guard lock{mutex};
        run_id = string_field(run.row, "run_id");
      }
      for (;;) {
        Value result;
        const auto request_id = string_field(request, "request_id");
        const auto capture = [&](std::string_view label, std::string_view raw) {
          // Exact admission was synchronously retained by start/send before enqueue.
          if (label == "admission")
            return;
          std::lock_guard lock{mutex};
          if (captures.size() >= max_events ||
              raw.size() > max_capture_bytes - capture_bytes)
            throw Error{ErrorCode::capacity};
          captures.push_back({"participant." + std::string{label}, std::string{raw},
                              Value::object({{"run_id", Value{run_id}},
                                             {"request_id", Value{request_id}}})});
          capture_bytes += raw.size();
        };
        if (run.stop.load()) {
          result = Value::object({{"status", Value{"cancelled"}},
                                  {"remote_disposition", Value{"not_dispatched"}}});
        } else {
          result = call_colleague(
              request, capture,
              [&](const Value &prepared, const ColleagueCapture &retain_bytes) {
                if (run.stop.load())
                  throw Error{ErrorCode::interrupted};
                return transport(prepared, retain_bytes,
                                 [&] { return run.stop.load(); });
              });
        }
        Value publication;
        {
          std::unique_lock lock{mutex};
          set(run.row, "result", projection(result));
          set(run.row, "last_delivery", Value{request_id});
          const auto status = string_field(result, "status");
          const auto remote = string_field(result, "remote_disposition");
          if (run.stop.load() || status != "completed") {
            set(run.row, "state",
                Value{remote == "unknown" ? "unknown"
                      : run.stop.load()   ? "cancelled"
                                          : "failed"});
            run.finished = true;
          } else
            set(run.row, "state", Value{"waiting"});
          if (run.finished) {
            erase(run.row, "request");
            set(run.row, "dropped_directions", Value{Number{run.inbox.size()}});
            run.inbox.clear();
          }
          dirty = true;
          publication = run.row;
          lock.unlock();
          notify(publication);
          changed.notify_all();
          lock.lock();
          if (run.finished)
            break;
          changed.wait(lock, [&] {
            return run.stop.load() || run.sealed || !run.inbox.empty();
          });
          if (run.stop.load()) {
            set(run.row, "state", Value{"cancelled"});
            run.finished = true;
            erase(run.row, "request");
            dirty = true;
            publication = run.row;
            lock.unlock();
            notify(publication);
            changed.notify_all();
            break;
          }
          if (run.inbox.empty()) {
            set(run.row, "state", Value{"completed"});
            run.finished = true;
            erase(run.row, "request");
            dirty = true;
            publication = run.row;
            lock.unlock();
            notify(publication);
            changed.notify_all();
            break;
          }
          request = std::move(run.inbox.front());
          run.inbox.pop_front();
          set(run.row, "state", Value{"running"});
          dirty = true;
          publication = run.row;
        }
        notify(publication);
      }
    } catch (...) {
      Value publication;
      {
        std::lock_guard lock{mutex};
        set(run.row, "state", Value{"unknown"});
        set(run.row, "result",
            Value::object({{"status", Value{"unknown"}},
                           {"remote_disposition", Value{"unknown"}},
                           {"error", Value{"local_capture_or_transport_failed"}}}));
        set(run.row, "last_delivery", field(request, "request_id"));
        run.finished = true;
        erase(run.row, "request");
        set(run.row, "dropped_directions", Value{Number{run.inbox.size()}});
        run.inbox.clear();
        dirty = true;
        publication = run.row;
      }
      notify(publication);
      changed.notify_all();
    }
  }
};
Participants::Participants(const Value &saved, Retain retain, Save save,
                           Publish publish)
    : impl_(std::make_unique<Impl>(std::move(retain), std::move(save),
                                   std::move(publish))) {
  if (saved == Value{})
    return;
  impl_->concurrency = integer(field(saved, "concurrency"));
  if (impl_->concurrency < 1 || impl_->concurrency > 16 ||
      field(saved, "runs").array().size() > max_runs)
    throw Error{ErrorCode::corrupt};
  for (const auto &row : field(saved, "runs").array()) {
    auto run = std::make_unique<Impl::Run>();
    run->row = row;
    run->finished = true;
    run->sealed = true;
    const auto state = string_field(row, "state");
    if (state == "running" || state == "waiting" || state == "cancel_requested") {
      set(run->row, "state", Value{"unknown"});
      set(run->row, "reopened", Value{true});
      impl_->dirty = true;
    }
    erase(run->row, "request");
    if (!impl_->runs.emplace(string_field(row, "run_id"), std::move(run)).second)
      throw Error{ErrorCode::corrupt};
  }
  drain();
}
Participants::~Participants() {
  try {
    shutdown();
  } catch (...) {
  }
}
bool Participants::active() const {
  std::lock_guard lock{impl_->mutex};
  for (const auto &[id, run] : impl_->runs)
    if (!run->finished)
      return true;
  return false;
}
void Participants::drain() {
  // Leave each capture queued if owner retention fails; dispatch never retries.
  for (;;) {
    Impl::Capture capture;
    {
      std::lock_guard lock{impl_->mutex};
      if (impl_->captures.empty())
        break;
      capture = impl_->captures.front();
    }
    try {
      impl_->retain(capture.label, capture.raw, capture.metadata);
    } catch (...) {
      {
        std::lock_guard lock{impl_->mutex};
        for (auto &[id, run] : impl_->runs)
          if (!run->finished) {
            run->stop.store(true);
            set(run->row, "cancel_requested", Value{true});
            impl_->dirty = true;
          }
      }
      impl_->changed.notify_all();
      throw;
    }
    {
      std::lock_guard lock{impl_->mutex};
      impl_->capture_bytes -= impl_->captures.front().raw.size();
      impl_->captures.pop_front();
    }
  }
  Value snapshot;
  {
    std::lock_guard lock{impl_->mutex};
    if (!impl_->dirty)
      return;
    snapshot = impl_->snapshot();
    impl_->dirty = false;
  }
  try {
    impl_->save(snapshot);
  } catch (...) {
    std::lock_guard lock{impl_->mutex};
    impl_->dirty = true;
    for (auto &[id, run] : impl_->runs)
      if (!run->finished) {
        run->stop.store(true);
        set(run->row, "cancel_requested", Value{true});
      }
    impl_->changed.notify_all();
    throw;
  }
}
Value Participants::configure(const Value &args) {
  drain();
  const auto count = integer(field(args, "concurrency"));
  if (count < 1 || count > 16 || active())
    throw Error{ErrorCode::invalid_range};
  {
    std::lock_guard lock{impl_->mutex};
    impl_->concurrency = count;
    impl_->dirty = true;
  }
  drain();
  return read();
}
Value Participants::start(std::string id, const Value &args,
                          const ParticipantTransport &transport) {
  drain();
  const auto &request = field(args, "request");
  const auto prepared = prepare_colleague(request);
  bounded(id, 128);
  const auto task = args.find("task_id") ? string_field(args, "task_id") : "";
  if (task.size() > 128 || !transport)
    throw Error{ErrorCode::invalid_range};
  {
    std::lock_guard lock{impl_->mutex};
    std::size_t live = 0;
    for (const auto &[key, run] : impl_->runs)
      live += run->finished ? 0U : 1U;
    if (impl_->runs.size() >= max_runs || live >= impl_->concurrency)
      throw Error{ErrorCode::capacity};
    if (impl_->runs.contains(id))
      throw Error{ErrorCode::conflict};
  }
  impl_->retain("participant.admission", unwrap(encode_packet_string(prepared)),
                Value::object({{"run_id", Value{id}},
                               {"request_id", field(request, "request_id")}}));
  auto run = std::make_unique<Impl::Run>();
  run->row = Value::object({{"run_id", Value{id}},
                            {"state", Value{"running"}},
                            {"cancel_requested", Value{false}},
                            {"task_id", Value{task}},
                            {"provider", field(request, "provider")},
                            {"model", field(request, "model")},
                            {"from", field(request, "from")},
                            {"to", field(request, "to")},
                            {"request", request},
                            {"result", Value{}},
                            {"last_delivery", Value{}}});
  auto *pointer = run.get();
  {
    std::lock_guard lock{impl_->mutex};
    impl_->runs.emplace(id, std::move(run));
    impl_->dirty = true;
  }
  try {
    drain();
  } // Pending run and exact request precede any worker/provider dispatch.
  catch (...) {
    std::lock_guard lock{impl_->mutex};
    set(pointer->row, "state", Value{"failed"});
    pointer->finished = true;
    impl_->dirty = true;
    throw;
  }
  impl_->notify(pointer->row);
  try {
    pointer->worker = std::jthread([this, pointer, request, transport] {
      impl_->work(*pointer, request, transport);
    });
  } catch (...) {
    std::lock_guard lock{impl_->mutex};
    set(pointer->row, "state", Value{"failed"});
    pointer->finished = true;
    impl_->dirty = true;
    throw;
  }
  return Value::object({{"run_id", Value{id}}, {"state", Value{"running"}}});
}
Value Participants::read(const Value &args) {
  drain();
  std::lock_guard lock{impl_->mutex};
  if (args.find("run_id")) {
    auto &run = impl_->find(args);
    auto row = run.row;
    set(row, "queued_directions", Value{Number{run.inbox.size()}});
    set(row, "accepted_directions", Value{Number{run.receipts.size()}});
    set(row, "sealed", Value{run.sealed});
    for (auto it = row.object().begin(); it != row.object().end(); ++it)
      if (it->first == "request") {
        row.object().erase(it);
        break;
      }
    return row;
  }
  // Return concise discovery; selected request bytes remain in retained originals.
  Value::Array rows;
  for (const auto &[id, run] : impl_->runs) {
    auto row = run->row;
    set(row, "queued_directions", Value{Number{run->inbox.size()}});
    set(row, "sealed", Value{run->sealed});
    auto &fields = row.object();
    std::erase_if(fields, [](const auto &entry) {
      return entry.first == "request" || entry.first == "result";
    });
    rows.push_back(std::move(row));
  }
  return Value::object({{"concurrency", Value{Number{impl_->concurrency}}},
                        {"max_runs", Value{Number{max_runs}}},
                        {"runs", Value{std::move(rows)}}});
}
Value Participants::send(const Value &args) {
  drain();
  if (args.object().size() != 4)
    throw Error{ErrorCode::unsupported};
  for (const auto &[key, value] : args.object()) {
    (void)value;
    if (key != "run_id" && key != "message_id" && key != "from" && key != "text")
      throw Error{ErrorCode::unsupported};
  }
  const auto &message = string_field(args, "message_id");
  bounded(message, 128);
  bounded(string_field(args, "from"), 128);
  bounded(string_field(args, "text"), 2048);
  Value request;
  std::string id;
  {
    std::lock_guard lock{impl_->mutex};
    auto &run = impl_->find(args);
    id = string_field(run.row, "run_id");
    const auto receipt = run.receipts.find(message);
    if (receipt != run.receipts.end()) {
      for (const auto key : {"run_id", "message_id", "from", "text"})
        if (field(receipt->second, key) != field(args, key))
          throw Error{ErrorCode::conflict};
      return Value::object({{"message_id", Value{message}},
                            {"accepted", Value{true}},
                            {"duplicate", Value{true}}});
    }
    if (run.finished || run.sealed || run.stop.load())
      throw Error{ErrorCode::busy};
    if (run.inbox.size() >= max_inbox || run.receipts.size() >= max_messages)
      throw Error{ErrorCode::capacity};
    request = field(run.row, "request");
    set(request, "request_id",
        Value{string_field(request, "request_id") + ":" + message});
    set(request, "task",
        Value{string_field(request, "task") + "\nAddressed direction from " +
              string_field(args, "from") + ":\n" + string_field(args, "text")});
    if (const auto *result = run.row.find("result"); result && result->find("text")) {
      auto context = field(request, "context");
      context.array().push_back(
          Value::object({{"id", Value{"participant-prior-answer:" + id}},
                         {"text", field(*result, "text")}}));
      set(request, "context", std::move(context));
    }
  }
  const auto prepared = prepare_colleague(request);
  impl_->retain("participant.admission", unwrap(encode_packet_string(prepared)),
                Value::object({{"run_id", Value{id}},
                               {"request_id", field(request, "request_id")},
                               {"message", args}}));
  {
    std::lock_guard lock{impl_->mutex};
    auto &run = impl_->find(args);
    if (run.finished || run.stop.load())
      throw Error{ErrorCode::busy};
    run.receipts.emplace(message, args);
    run.inbox.push_back(std::move(request));
  }
  impl_->changed.notify_all();
  return Value::object({{"message_id", Value{message}},
                        {"run_id", Value{id}},
                        {"accepted", Value{true}},
                        {"boundary", Value{"after current request"}}});
}
Value Participants::cancel(const Value &args) {
  drain();
  Value row;
  {
    std::lock_guard lock{impl_->mutex};
    auto &run = impl_->find(args);
    if (!run.finished) {
      run.stop.store(true);
      set(run.row, "cancel_requested", Value{true});
      impl_->dirty = true;
    }
    row = run.row;
  }
  impl_->changed.notify_all();
  drain();
  impl_->notify(row);
  return read(args);
}
Value Participants::await(const Value &args, const std::function<bool()> &cancelled) {
  const auto ms = args.find("timeout_ms") ? integer(field(args, "timeout_ms")) : 30000U;
  if (ms < 1 || ms > 3600000)
    throw Error{ErrorCode::invalid_range};
  const auto deadline =
      std::chrono::steady_clock::now() + std::chrono::milliseconds{ms};
  bool timed_out = false;
  for (;;) {
    drain();
    {
      std::unique_lock lock{impl_->mutex};
      auto &run = impl_->find(args);
      if (run.finished ||
          (string_field(run.row, "state") == "waiting" && run.inbox.empty()))
        break;
      if (std::chrono::steady_clock::now() >= deadline) {
        timed_out = true;
        break;
      }
      impl_->changed.wait_for(lock, std::chrono::milliseconds{20});
    }
    if (cancelled && cancelled())
      throw Error{ErrorCode::interrupted};
  }
  auto row = read(args);
  set(row, "await_timed_out", Value{timed_out});
  return row;
}
Value Participants::join(const Value &args, const std::function<bool()> &cancelled) {
  std::vector<Impl::Run *> selected;
  {
    std::lock_guard lock{impl_->mutex};
    const auto &ids = field(args, "run_ids").array();
    if (ids.empty() || ids.size() > max_runs)
      throw Error{ErrorCode::invalid_range};
    for (const auto &id : ids) {
      auto &run = impl_->find(Value::object({{"run_id", id}}));
      if (std::find(selected.begin(), selected.end(), &run) != selected.end())
        throw Error{ErrorCode::conflict};
      selected.push_back(&run);
    }
    for (auto *run : selected)
      run->sealed = true;
  }
  impl_->changed.notify_all();
  for (;;) {
    drain();
    bool finished = true;
    {
      std::unique_lock lock{impl_->mutex};
      for (const auto *run : selected)
        finished &= run->finished;
      if (finished)
        break;
      impl_->changed.wait_for(lock, std::chrono::milliseconds{20});
    }
    if (cancelled && cancelled())
      throw Error{ErrorCode::interrupted};
  }
  for (auto *run : selected)
    if (run->worker.joinable())
      run->worker.join();
  drain();
  Value::Array rows;
  for (const auto *run : selected)
    rows.push_back(read(Value::object({{"run_id", field(run->row, "run_id")}})));
  return Value::object({{"runs", Value{std::move(rows)}}});
}
Value Participants::archive(const Value &args) {
  drain();
  Impl::Run *run;
  {
    std::lock_guard lock{impl_->mutex};
    run = &impl_->find(args);
    if (!run->finished)
      throw Error{ErrorCode::busy};
  }
  if (run->worker.joinable())
    run->worker.join();
  {
    std::lock_guard lock{impl_->mutex};
    impl_->runs.erase(string_field(args, "run_id"));
    impl_->dirty = true;
  }
  drain();
  return Value::object({{"archived", field(args, "run_id")}});
}
void Participants::shutdown() {
  {
    std::lock_guard lock{impl_->mutex};
    for (auto &[id, run] : impl_->runs)
      if (!run->finished) {
        run->stop.store(true);
        set(run->row, "cancel_requested", Value{true});
        impl_->dirty = true;
      }
  }
  impl_->changed.notify_all();
  for (auto &[id, run] : impl_->runs)
    if (run->worker.joinable())
      run->worker.join();
  drain();
}
} // namespace blackbird
