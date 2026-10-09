#include "blackbird/participants.hpp"
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
void set(Json &value, std::string_view key, Json replacement) {
  for (auto &[name, item] : value.object())
    if (name == key) {
      item = std::move(replacement);
      return;
    }
  value.object().emplace_back(key, std::move(replacement));
}
void erase(Json &value, std::string_view key) {
  std::erase_if(value.object(), [&](const auto &entry) { return entry.first == key; });
}
void bounded(std::string_view text, std::size_t limit) {
  if (text.empty() || text.size() > limit || text.find('\0') != std::string_view::npos)
    throw Error{ErrorCode::invalid_range};
}
unsigned integer(const Json &value) {
  const auto &text = value.number().text;
  unsigned out = 0;
  const auto parsed = std::from_chars(text.data(), text.data() + text.size(), out);
  if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size())
    throw Error{ErrorCode::invalid_range};
  return out;
}
Json projection(const Json &result) {
  if (unwrap(dump_json(result)).size() <= 65536)
    return result;
  return Json::object(
      {{"status", field(result, "status")},
       {"remote_disposition", field(result, "remote_disposition")},
       {"text", result.find("text")
                    ? Json{field(result, "text").string().substr(0, 8192)}
                    : Json{}},
       {"details",
        Json{"Read retained participant result original; projection truncated"}}});
}
} // namespace
struct Participants::Impl {
  struct Run {
    Json row;
    std::atomic_bool stop{false};
    bool sealed = false, finished = false;
    std::deque<Json> inbox;
    std::map<std::string, Json> receipts;
    std::jthread worker;
  };
  struct Capture {
    std::string label, raw;
    Json metadata;
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
  Run &find(const Json &args) {
    const auto found = runs.find(string_field(args, "run_id"));
    if (found == runs.end())
      throw Error{ErrorCode::invalid_range};
    return *found->second;
  }
  Json snapshot() const {
    Json::Array rows;
    for (const auto &[id, run] : runs)
      rows.push_back(run->row);
    return Json::object({{"label", Json{"participants-state-v1"}},
                         {"concurrency", Json{JsonNumber{std::to_string(concurrency)}}},
                         {"max_runs", Json{JsonNumber{std::to_string(max_runs)}}},
                         {"runs", Json{std::move(rows)}}});
  }
  void notify(const Json &row) noexcept {
    if (publish)
      try {
        publish(Json::object({{"attempt", field(row, "run_id")},
                              {"run_id", field(row, "run_id")},
                              {"task_id", field(row, "task_id")},
                              {"phase", field(row, "state")},
                              {"operation", Json{"participant"}},
                              {"cancel_requested", field(row, "cancel_requested")}}));
      } catch (...) { /* Presentation cannot change effects. */
      }
  }
  void work(Run &run, Json request, const ParticipantTransport &transport) noexcept {
    try {
      std::string run_id;
      {
        std::lock_guard lock{mutex};
        run_id = string_field(run.row, "run_id");
      }
      for (;;) {
        Json result;
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
                              Json::object({{"run_id", Json{run_id}},
                                            {"request_id", Json{request_id}}})});
          capture_bytes += raw.size();
        };
        if (run.stop.load()) {
          result = Json::object({{"status", Json{"cancelled"}},
                                 {"remote_disposition", Json{"not_dispatched"}}});
        } else {
          result = call_colleague(
              request, capture,
              [&](const Json &prepared, const ColleagueCapture &retain_bytes) {
                if (run.stop.load())
                  throw Error{ErrorCode::interrupted};
                return transport(prepared, retain_bytes,
                                 [&] { return run.stop.load(); });
              });
        }
        Json publication;
        {
          std::unique_lock lock{mutex};
          set(run.row, "result", projection(result));
          set(run.row, "last_delivery", Json{request_id});
          const auto status = string_field(result, "status");
          const auto remote = string_field(result, "remote_disposition");
          if (run.stop.load() || status != "completed") {
            set(run.row, "state",
                Json{remote == "unknown" ? "unknown"
                     : run.stop.load()   ? "cancelled"
                                         : "failed"});
            run.finished = true;
          } else
            set(run.row, "state", Json{"waiting"});
          if (run.finished) {
            erase(run.row, "request");
            set(run.row, "dropped_directions",
                Json{JsonNumber{std::to_string(run.inbox.size())}});
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
            set(run.row, "state", Json{"cancelled"});
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
            set(run.row, "state", Json{"completed"});
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
          set(run.row, "state", Json{"running"});
          dirty = true;
          publication = run.row;
        }
        notify(publication);
      }
    } catch (...) {
      Json publication;
      {
        std::lock_guard lock{mutex};
        set(run.row, "state", Json{"unknown"});
        set(run.row, "result",
            Json::object({{"status", Json{"unknown"}},
                          {"remote_disposition", Json{"unknown"}},
                          {"error", Json{"local_capture_or_transport_failed"}}}));
        set(run.row, "last_delivery", field(request, "request_id"));
        run.finished = true;
        erase(run.row, "request");
        set(run.row, "dropped_directions",
            Json{JsonNumber{std::to_string(run.inbox.size())}});
        run.inbox.clear();
        dirty = true;
        publication = run.row;
      }
      notify(publication);
      changed.notify_all();
    }
  }
};
Participants::Participants(const Json &saved, Retain retain, Save save, Publish publish)
    : impl_(std::make_unique<Impl>(std::move(retain), std::move(save),
                                   std::move(publish))) {
  if (saved == Json{})
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
      set(run->row, "state", Json{"unknown"});
      set(run->row, "reopened", Json{true});
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
            set(run->row, "cancel_requested", Json{true});
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
  Json snapshot;
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
        set(run->row, "cancel_requested", Json{true});
      }
    impl_->changed.notify_all();
    throw;
  }
}
Json Participants::configure(const Json &args) {
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
Json Participants::start(std::string id, const Json &args,
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
  impl_->retain("participant.admission", unwrap(dump_json(prepared)),
                Json::object({{"run_id", Json{id}},
                              {"request_id", field(request, "request_id")}}));
  auto run = std::make_unique<Impl::Run>();
  run->row = Json::object({{"run_id", Json{id}},
                           {"state", Json{"running"}},
                           {"cancel_requested", Json{false}},
                           {"task_id", Json{task}},
                           {"provider", field(request, "provider")},
                           {"model", field(request, "model")},
                           {"from", field(request, "from")},
                           {"to", field(request, "to")},
                           {"request", request},
                           {"result", Json{}},
                           {"last_delivery", Json{}}});
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
    set(pointer->row, "state", Json{"failed"});
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
    set(pointer->row, "state", Json{"failed"});
    pointer->finished = true;
    impl_->dirty = true;
    throw;
  }
  return Json::object({{"run_id", Json{id}}, {"state", Json{"running"}}});
}
Json Participants::read(const Json &args) {
  drain();
  std::lock_guard lock{impl_->mutex};
  if (args.find("run_id")) {
    auto &run = impl_->find(args);
    auto row = run.row;
    set(row, "queued_directions", Json{JsonNumber{std::to_string(run.inbox.size())}});
    set(row, "accepted_directions",
        Json{JsonNumber{std::to_string(run.receipts.size())}});
    set(row, "sealed", Json{run.sealed});
    for (auto it = row.object().begin(); it != row.object().end(); ++it)
      if (it->first == "request") {
        row.object().erase(it);
        break;
      }
    return row;
  }
  // Return concise discovery; selected request bytes remain in retained originals.
  Json::Array rows;
  for (const auto &[id, run] : impl_->runs) {
    auto row = run->row;
    set(row, "queued_directions", Json{JsonNumber{std::to_string(run->inbox.size())}});
    set(row, "sealed", Json{run->sealed});
    auto &fields = row.object();
    std::erase_if(fields, [](const auto &entry) {
      return entry.first == "request" || entry.first == "result";
    });
    rows.push_back(std::move(row));
  }
  return Json::object(
      {{"concurrency", Json{JsonNumber{std::to_string(impl_->concurrency)}}},
       {"max_runs", Json{JsonNumber{std::to_string(max_runs)}}},
       {"runs", Json{std::move(rows)}}});
}
Json Participants::send(const Json &args) {
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
  Json request;
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
      return Json::object({{"message_id", Json{message}},
                           {"accepted", Json{true}},
                           {"duplicate", Json{true}}});
    }
    if (run.finished || run.sealed || run.stop.load())
      throw Error{ErrorCode::busy};
    if (run.inbox.size() >= max_inbox || run.receipts.size() >= max_messages)
      throw Error{ErrorCode::capacity};
    request = field(run.row, "request");
    set(request, "request_id",
        Json{string_field(request, "request_id") + ":" + message});
    set(request, "task",
        Json{string_field(request, "task") + "\nAddressed direction from " +
             string_field(args, "from") + ":\n" + string_field(args, "text")});
    if (const auto *result = run.row.find("result"); result && result->find("text")) {
      auto context = field(request, "context");
      context.array().push_back(
          Json::object({{"id", Json{"participant-prior-answer:" + id}},
                        {"text", field(*result, "text")}}));
      set(request, "context", std::move(context));
    }
  }
  const auto prepared = prepare_colleague(request);
  impl_->retain("participant.admission", unwrap(dump_json(prepared)),
                Json::object({{"run_id", Json{id}},
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
  return Json::object({{"message_id", Json{message}},
                       {"run_id", Json{id}},
                       {"accepted", Json{true}},
                       {"boundary", Json{"after current request"}}});
}
Json Participants::cancel(const Json &args) {
  drain();
  Json row;
  {
    std::lock_guard lock{impl_->mutex};
    auto &run = impl_->find(args);
    if (!run.finished) {
      run.stop.store(true);
      set(run.row, "cancel_requested", Json{true});
      impl_->dirty = true;
    }
    row = run.row;
  }
  impl_->changed.notify_all();
  drain();
  impl_->notify(row);
  return read(args);
}
Json Participants::await(const Json &args, const std::function<bool()> &cancelled) {
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
  set(row, "await_timed_out", Json{timed_out});
  return row;
}
Json Participants::join(const Json &args, const std::function<bool()> &cancelled) {
  std::vector<Impl::Run *> selected;
  {
    std::lock_guard lock{impl_->mutex};
    const auto &ids = field(args, "run_ids").array();
    if (ids.empty() || ids.size() > max_runs)
      throw Error{ErrorCode::invalid_range};
    for (const auto &id : ids) {
      auto &run = impl_->find(Json::object({{"run_id", id}}));
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
  Json::Array rows;
  for (const auto *run : selected)
    rows.push_back(read(Json::object({{"run_id", field(run->row, "run_id")}})));
  return Json::object({{"runs", Json{std::move(rows)}}});
}
Json Participants::archive(const Json &args) {
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
  return Json::object({{"archived", field(args, "run_id")}});
}
void Participants::shutdown() {
  {
    std::lock_guard lock{impl_->mutex};
    for (auto &[id, run] : impl_->runs)
      if (!run->finished) {
        run->stop.store(true);
        set(run->row, "cancel_requested", Json{true});
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
