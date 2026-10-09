#include "blackbird/participants.hpp"
#include "blackbird/tools.hpp"
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
using namespace blackbird;
void check(bool good) {
  if (!good)
    throw Error{ErrorCode::corrupt};
}
Json request(std::string who) {
  auto value = unwrap(parse_json(
      R"({"request_id":"initial","from":"main","to":"alpha","provider":"openai","model":"fake","task":"source review","context":[{"id":"s1","text":"selected"}],"profile":{"name":"context-only","provenance":"test","timeout_seconds":3,"tools":"none","requests":1}})"));
  for (auto &[name, item] : value.object())
    if (name == "to")
      item = Json{who};
  return value;
}
Json args(std::string id) { return Json::object({{"run_id", Json{id}}}); }
Json send(std::string run, std::string message, std::string text = "direction") {
  return Json::object({{"run_id", Json{run}},
                       {"message_id", Json{message}},
                       {"from", Json{"operator"}},
                       {"text", Json{text}}});
}
int main() {
  try {
    const auto owner = std::this_thread::get_id();
    Json saved;
    std::vector<std::string> admissions;
    std::mutex gate;
    std::condition_variable cv;
    int first_calls = 0, total_calls = 0;
    bool alpha_release = false, beta_release = false;
    Participants pool{Json{},
                      [&](std::string_view label, std::string_view raw, const Json &) {
                        check(std::this_thread::get_id() == owner && !raw.empty());
                        if (label == "participant.admission")
                          admissions.emplace_back(raw);
                      },
                      [&](const Json &packet) {
                        check(std::this_thread::get_id() == owner);
                        saved = packet;
                      }};
    const ParticipantTransport transport = [&](const Json &prepared,
                                               const ColleagueCapture &capture,
                                               const std::function<bool()> &cancelled) {
      const auto &r = field(prepared, "request");
      const auto who = string_field(r, "to");
      const auto delivery = string_field(r, "request_id");
      {
        std::unique_lock lock{gate};
        ++total_calls;
        if (delivery == "initial") {
          ++first_calls;
          cv.notify_all();
          const auto deadline =
              std::chrono::steady_clock::now() + std::chrono::seconds{4};
          check(cv.wait_until(lock, deadline, [&] {
            return who == "alpha" ? alpha_release : beta_release;
          }));
        }
      }
      if (cancelled())
        throw Error{ErrorCode::interrupted};
      if (delivery != "initial")
        check(string_field(r, "task").find("direction") != std::string::npos);
      capture("raw", "captured provider bytes");
      const auto text = Json::object(
          {{"type", Json{"output_text"}}, {"text", Json{"answer:" + delivery}}});
      const auto message = Json::object({{"type", Json{"message"}},
                                         {"role", Json{"assistant"}},
                                         {"content", Json{Json::Array{text}}}});
      return Json::object({{"status", Json{"completed"}},
                           {"model", Json{"actual"}},
                           {"output", Json{Json::Array{message}}}});
    };
    pool.start("r1", Json::object({{"request", request("alpha")}}), transport);
    pool.start("r2", Json::object({{"request", request("beta")}}), transport);
    {
      std::unique_lock lock{gate};
      check(
          cv.wait_for(lock, std::chrono::seconds{2}, [&] { return first_calls == 2; }));
    }
    check(admissions.size() == 2 && pool.active());
    bool cap = false;
    try {
      pool.start("r3", Json::object({{"request", request("third")}}), transport);
    } catch (const Error &e) {
      cap = e.code == ErrorCode::capacity;
    }
    check(cap && admissions.size() == 2);
    auto timed_args = args("r2");
    timed_args.object().emplace_back("timeout_ms", Json{JsonNumber{"1"}});
    check(field(pool.await(timed_args), "await_timed_out") == Json{true});
    const auto crash_snapshot = saved;
    check(field(pool.send(send("r1", "d1")), "accepted") == Json{true});
    check(field(pool.send(send("r1", "d1")), "duplicate") == Json{true});
    auto reordered = send("r1", "d1");
    std::reverse(reordered.object().begin(), reordered.object().end());
    check(field(pool.send(reordered), "duplicate") == Json{true});
    bool conflict = false;
    try {
      pool.send(send("r1", "d1", "different"));
    } catch (const Error &e) {
      conflict = e.code == ErrorCode::conflict;
    }
    check(conflict);
    for (int i = 0; i < 16; ++i)
      pool.send(send("r2", "queued" + std::to_string(i)));
    cap = false;
    try {
      pool.send(send("r2", "overflow"));
    } catch (const Error &e) {
      cap = e.code == ErrorCode::capacity;
    }
    check(cap);
    pool.cancel(args("r2"));
    const auto requested = pool.read(args("r2"));
    check(field(requested, "cancel_requested") == Json{true} &&
          string_field(requested, "state") == "running");
    {
      std::lock_guard lock{gate};
      alpha_release = true;
      beta_release = true;
    }
    cv.notify_all();
    const auto joined = pool.join(
        Json::object({{"run_ids", Json{Json::Array{Json{"r2"}, Json{"r1"}}}}}));
    const auto &rows = field(joined, "runs").array();
    check(string_field(rows[0], "run_id") == "r2" &&
          string_field(rows[0], "state") == "unknown");
    check(string_field(rows[1], "run_id") == "r1" &&
          string_field(rows[1], "state") == "completed");
    check(string_field(field(rows[1], "result"), "text") == "answer:initial:d1" &&
          !pool.active());
    check(total_calls == 3); // Accepted beta directions were cancelled before dispatch.
    Participants reopened{crash_snapshot,
                          [](std::string_view, std::string_view, const Json &) {},
                          [](const Json &) {}};
    check(!reopened.active() &&
          string_field(reopened.read(args("r1")), "state") == "unknown");
    check(string_field(reopened.read(args("r2")), "state") == "unknown" &&
          total_calls == 3);
    pool.archive(args("r1"));
    pool.configure(Json::object({{"concurrency", Json{JsonNumber{"3"}}}}));
    check(field(pool.read(), "runs").array().size() == 1);
    Participants bounded_capture{
        Json{}, [](std::string_view, std::string_view, const Json &) {},
        [](const Json &) {}};
    bounded_capture.start(
        "large", Json::object({{"request", request("large")}}),
        [](const Json &, const ColleagueCapture &capture,
           const std::function<bool()> &) -> Json {
          capture("raw", std::string(9 * 1024 * 1024, 'x'));
          throw Error{ErrorCode::corrupt}; // oversized capture must reject first
        });
    const auto large = bounded_capture.join(
        Json::object({{"run_ids", Json{Json::Array{Json{"large"}}}}}));
    check(string_field(field(large, "runs").array()[0], "state") == "unknown");
    Participants failed_snapshot{
        Json{}, [](std::string_view, std::string_view, const Json &) {},
        [](const Json &) { throw Error{ErrorCode::io}; }};
    bool failed_before_launch = false;
    int launches = 0;
    try {
      failed_snapshot.start(
          "no-save", Json::object({{"request", request("no-save")}}),
          [&](const Json &, const ColleagueCapture &, const std::function<bool()> &) {
            ++launches;
            return Json{};
          });
    } catch (const Error &error) {
      failed_before_launch = error.code == ErrorCode::io;
    }
    check(failed_before_launch && !failed_snapshot.active() && launches == 0);
    int blocked_dispatches = 0;
    Participants failed{Json{},
                        [](std::string_view, std::string_view, const Json &) {
                          throw Error{ErrorCode::io};
                        },
                        [](const Json &) {}};
    bool refused = false;
    try {
      failed.start(
          "blocked", Json::object({{"request", request("blocked")}}),
          [&](const Json &, const ColleagueCapture &, const std::function<bool()> &) {
            ++blocked_dispatches;
            return Json{};
          });
    } catch (const Error &e) {
      refused = e.code == ErrorCode::io;
    }
    check(refused && blocked_dispatches == 0 && !failed.active());
  } catch (const Error &error) {
    std::cerr << error_name(error.code) << '\n';
    return 1;
  }
}
