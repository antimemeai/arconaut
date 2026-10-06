#include "arconaut/context.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace arconaut;
void check(bool x) {
  if (!x)
    throw std::runtime_error("retained context contract");
}
template <class T> T take(Result<T> r) {
  if (!r.has_value())
    throw r.error();
  return std::move(r).value();
}
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return take(T::from_bytes(b));
}
int main() {
  char name[] = "/tmp/arco-context-XXXXXX";
  const char *path = mkdtemp(name);
  if (!path)
    return 2;
  try {
    JournalHeader h{id<EnvironmentId>(1),
                    id<AuditStreamId>(2),
                    3,
                    {1024 * 1024, 4 * 1024 * 1024},
                    std::nullopt};
    JournalCapacity cap{32 * 1024 * 1024, 4096};
    auto root =
        take(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                       take(NativeJournalDirectory::open(path))),
                                   "audit", h, cap));
    AuditLog log{*root};
    const auto app = log.issue();
    const auto packet = Json::object({{"fixture", Json{true}}});
    log.record(app, ApplicationChannel::log, packet);
    const auto before_duplicate = root->cursor();
    log.record(app, ApplicationChannel::log, packet);
    check(root->cursor().sequence == before_duplicate.sequence);
    bool conflict = false;
    try {
      log.record(app, ApplicationChannel::log,
                 Json::object({{"fixture", Json{false}}}));
    } catch (const Error &e) {
      conflict = e.code == ErrorCode::conflict;
    }
    check(conflict && root->state() == JournalWriterState::live);
    RetainedEvent literal{{},
                          ApplicationRecordEvent{id<ApplicationRecordId>(7),
                                                 ApplicationChannel::context,
                                                 {std::byte{0x41}}}};
    const auto encoded = take(encode_retained_event(literal, 64));
    check(encoded.size() == 31 && encoded[0] == std::byte{1} &&
          encoded[2] == std::byte{14} && encoded[8] == std::byte{7} &&
          encoded[24] == std::byte{2} && encoded[26] == std::byte{1} &&
          encoded[30] == std::byte{0x41});
    check(take(decode_retained_event(encoded, 64)) == literal);
    auto malformed = encoded;
    malformed[24] = std::byte{4};
    check(!decode_retained_event(malformed, 64).has_value());
    ContextStore ctx{log};
    auto sizes = ctx.stats();
    check(field(sizes, "entry_count").number().text == "0" &&
          field(sizes, "input_bytes").number().text == "2");
    const auto literal_item = unwrap(parse_json(R"({"text":"é\n"})"));
    ctx.append({literal_item}, "size-fixture");
    sizes = ctx.stats();
    // Serializer uses six-byte Unicode escape for LF: item19 + brackets2.
    check(field(sizes, "input_bytes").number().text == "21" &&
          field(field(sizes, "entries").array()[0], "item_bytes").number().text ==
              "19");
    auto clear_sizes = ctx.view();
    clear_sizes.object()[1].second = Json{Json::Array{}};
    check(std::get<bool>(field(ctx.edit(clear_sizes), "accepted").value()));
    check(field(ctx.stats(), "input_bytes").number().text == "2");
    const auto original =
        parse_json(
            R"({"type":"reasoning","encrypted_content":"OPAQUE==","extension":9007199254740993})")
            .value();
    ctx.append({original}, "fixture");
    auto initial = ctx.view();
    const auto entry = initial.find("entries")->array()[0].find("id")->string();
    auto empty = Json::object(
        {{"base", *initial.find("base")}, {"entries", Json{Json::Array{}}}});
    auto edited = ctx.edit(empty);
    check(std::get<bool>(edited.find("accepted")->value()));
    auto stale = ctx.edit(empty);
    check(!std::get<bool>(stale.find("accepted")->value()));
    check(ctx.view().find("entries")->array().empty());
    auto invalid =
        Json::object({{"base", Json{ctx.head()}}, {"entries", Json{"not an array"}}});
    check(!std::get<bool>(ctx.edit(invalid).find("accepted")->value()));
    ctx.restore(entry);
    check(ctx.items()[0] == original);
    const auto head = ctx.head();
    log.original({"tool-output",
                  R"({"op":"edit","accepted":true,"entries":[],"revision":"evil"})",
                  Json{}});
    root.reset();
    root = take(RetainedState::open(std::make_unique<NativeJournalDirectory>(
                                        take(NativeJournalDirectory::open(path))),
                                    "audit", h, cap));
    check(root->confirm_recovery().has_value());
    struct EmptyCustody final : CustodyVerifier {
      Result<void> verify(std::span<const AttemptState> attempts) override {
        return attempts.empty() ? Result<void>::success()
                                : Result<void>::failure({ErrorCode::conflict});
      }
    } custody;
    check(root->reconcile(custody).has_value());
    AuditLog replay{*root};
    ContextStore restored{replay};
    check(restored.head() == head && restored.items()[0] == original);
    restored.edit(
        Json::object({{"base", Json{head}}, {"entries", Json{Json::Array{}}}}));
    restored.restore(entry);
    check(restored.items()[0] == original);
    auto ok = [](const Json &out) {
      return std::get<bool>(field(out, "accepted").value());
    };
    auto proposal = [&](std::string mode, Json::Array ids) {
      return Json::object({{"base", Json{restored.head()}},
                           {"mode", Json{std::move(mode)}},
                           {"ids", Json{std::move(ids)}},
                           {"reason", Json{"direct oracle"}},
                           {"source", Json{"explicit test input"}}});
    };
    restored.append(
        {Json::object({{"role", Json{"user"}}, {"content", Json{"MISSION"}}}),
         Json::object({{"type", Json{"function_call"}}, {"call_id", Json{"c1"}}}),
         Json::object({{"type", Json{"function_call_output"}},
                       {"call_id", Json{"c1"}},
                       {"output", Json{"opaque é\u0000"}}})},
        "managed-fixture");
    const auto all = field(restored.view(), "entries").array();
    const auto mission = field(all[1], "id"), call = field(all[2], "id"),
               output = field(all[3], "id");
    check(!ok(restored.manage(Json{})));
    check(!ok(restored.manage(proposal("archive", {call}))));
    check(!ok(restored.manage(proposal("archive", {mission}))));
    check(!ok(restored.manage(proposal("archive", {call, call}))));
    check(!ok(restored.manage(proposal("archive", {Json{"unknown"}}))));
    check(!ok(restored.manage(proposal("select", {}))));
    auto summarize = proposal("summarize", {call, output});
    summarize.object().emplace_back(
        "summary", Json::object({{"role", Json{"developer"}},
                                 {"content", Json{"tool completed"}}}));
    check(ok(restored.manage(summarize)));
    check(!ok(restored.manage(summarize))); // stale
    const auto summarized = restored.view();
    check(!ok(restored.manage(proposal(
        "archive", {field(field(summarized, "entries").array().back(), "id")}))));
    check(field(field(summarized, "entries").array().back(), "ancestry") ==
          Json{Json::Array{call, output}});
    check(ok(restored.manage(proposal("restore", {call, output}))));
    check(restored.items()[2] == field(all[2], "item") &&
          restored.items()[3] == field(all[3], "item"));
    auto reordered = restored.view();
    std::swap(reordered.object()[1].second.array()[2],
              reordered.object()[1].second.array()[3]);
    check(ok(restored.edit(reordered))); // generic CLM is still available
    check(ok(restored.manage(proposal("restore", {call, output}))));
    check(restored.items()[2] == field(all[2], "item"));
    const auto before_stage = restored.view();
    restored.begin_workflow();
    check(field(restored.manage(proposal("archive", {call, output})), "staged") ==
          Json{true});
    check(restored.view() == before_stage);
    check(!ok(restored.manage(proposal("archive", {call, output})))); // overlap
    restored.append(
        {Json::object({{"type", Json{"function_call"}}, {"call_id", Json{"active"}}})},
        "active");
    restored.append({Json::object({{"type", Json{"function_call_output"}},
                                   {"call_id", Json{"active"}},
                                   {"output", Json{"settled"}}})},
                    "active-result");
    check(ok(restored.finish_workflow(true)));
    check(restored.items().size() == 5 &&
          string_field(restored.items()[3], "call_id") == "active");
    restored.begin_workflow();
    check(field(restored.manage(proposal("restore", {call, output})), "staged") ==
          Json{true});
    const auto cancelled_head = restored.head();
    check(!ok(restored.finish_workflow(false)) && restored.head() == cancelled_head);
    restored.begin_workflow();
    (void)restored.manage(proposal("restore", {call, output}));
    (void)restored.edit(restored.view());
    check(!ok(restored.finish_workflow(true)));
    check(ok(restored.manage(proposal("restore", {call, output}))));
    const auto inspection =
        restored.inspect(Json::object({{"kind", Json{"originals"}},
                                       {"entry", call},
                                       {"offset", Json{JsonNumber{"1"}}},
                                       {"limit", Json{JsonNumber{"7"}}}}));
    check(string_field(inspection, "bytes").size() == 14 &&
          field(inspection, "next").number().text == "8");
    // A page cursor is only valid for the snapshot returned with its first page.
    const auto history_page =
        restored.inspect(Json::object({{"kind", Json{"history"}}}));
    const auto history_before_reject = field(history_page, "revision");
    const auto view_before_reject = restored.view();
    auto stale_read_proposal = proposal("archive", {call, output});
    for (auto &[key, value] : stale_read_proposal.object())
      if (key == "base")
        value = Json{"obsolete"};
    check(!ok(restored.manage(stale_read_proposal)));
    check(restored.view() == view_before_reject);
    const auto rejection_cursor = root->cursor().sequence;
    bool rejected_history_stale = false;
    try {
      (void)restored.inspect(Json::object(
          {{"kind", Json{"history"}}, {"revision", history_before_reject}}));
    } catch (const Error &e) {
      rejected_history_stale = e.code == ErrorCode::conflict;
    }
    check(rejected_history_stale && root->cursor().sequence == rejection_cursor);
    const auto inspection_head = restored.head();
    for (const auto *kind : {"originals", "history", "index"}) {
      check(restored.inspect(Json::object(
                {{"kind", Json{kind}},
                 {"revision",
                  field(restored.inspect(Json::object({{"kind", Json{kind}}})),
                        "revision")}})) ==
            restored.inspect(Json::object({{"kind", Json{kind}}})));
    }
    const auto inspect_cursor = root->cursor().sequence;
    bool bad_inspection_revision = false;
    try {
      (void)restored.inspect(
          Json::object({{"kind", Json{"originals"}}, {"revision", Json{true}}}));
    } catch (const Error &e) {
      bad_inspection_revision = e.code == ErrorCode::corrupt;
    }
    check(bad_inspection_revision && root->cursor().sequence == inspect_cursor);
    restored.append({Json::object({{"role", Json{"assistant"}},
                                   {"content", Json{"concurrent append"}}})},
                    "inspection-concurrent-append");
    const auto changed_cursor = root->cursor().sequence;
    const auto changed_view = restored.view();
    for (const auto *kind : {"originals", "history", "index"}) {
      bool stale_inspection = false;
      try {
        (void)restored.inspect(
            Json::object({{"kind", Json{kind}}, {"revision", Json{inspection_head}}}));
      } catch (const Error &e) {
        stale_inspection = e.code == ErrorCode::conflict;
      }
      check(stale_inspection && root->cursor().sequence == changed_cursor &&
            restored.view() == changed_view);
    }
    restored.append(
        {Json::object({{"role", Json{"user"}}, {"content", Json{"follow-up"}}})},
        "mission-followup");
    check(!ok(restored.manage(proposal("archive", {mission}))));
    restored.begin_workflow();
    restored.append({Json::object({{"type", Json{"function_call"}},
                                   {"call_id", Json{"select-active"}}})},
                    "select-current");
    Json::Array keep;
    const auto select_view = restored.view();
    for (const auto &kept : field(select_view, "entries").array()) {
      const auto *role = field(kept, "item").find("role");
      if (role && (*role == Json{"user"} || *role == Json{"developer"}))
        keep.push_back(field(kept, "id"));
    }
    check(field(restored.manage(proposal("select", keep)), "staged") == Json{true});
    restored.append({Json::object({{"type", Json{"function_call_output"}},
                                   {"call_id", Json{"select-active"}},
                                   {"output", Json{"done"}}})},
                    "select-result");
    check(ok(restored.finish_workflow(true)));
    check(string_field(restored.items().back(), "call_id") == "select-active");
    restored.begin_workflow();
    (void)restored.manage(proposal("archive", {}));
    restored.append({Json::object({{"type", Json{"function_call_output"}},
                                   {"call_id", Json{"DANGLING-SUFFIX"}},
                                   {"output", Json{"bad"}}})},
                    "malformed suffix");
    const auto malformed_suffix_view = restored.view();
    const auto suffix_outcome = restored.finish_workflow(true);
    check(!ok(suffix_outcome) &&
          string_field(suffix_outcome, "reason") == "invalid-tool-protocol");
    check(restored.view() == malformed_suffix_view);
    auto suffix_repair = restored.view();
    suffix_repair.object()[1].second.array().pop_back();
    check(ok(restored.edit(suffix_repair)));
    const auto final_head = restored.head();

    ContextStore replay_managed{replay};
    check(replay_managed.head() == final_head &&
          replay_managed.view() == restored.view());
    check(replay_managed.inspect(Json::object({{"kind", Json{"index"}}})) ==
          restored.inspect(Json::object({{"kind", Json{"index"}}})));
    // Native close/reopen after accepted transforms and an orphan staged proposal.
    const auto expected_view = restored.view();
    const auto expected_index =
        restored.inspect(Json::object({{"kind", Json{"index"}}}));
    restored.begin_workflow();
    (void)restored.manage(proposal("restore", {call, output}));
    root.reset();
    root = take(RetainedState::open(std::make_unique<NativeJournalDirectory>(
                                        take(NativeJournalDirectory::open(path))),
                                    "audit", h, cap));
    check(root->confirm_recovery().has_value());
    check(root->reconcile(custody).has_value());
    AuditLog native_replay{*root};
    ContextStore native_restored{native_replay};
    check(native_restored.view() == expected_view);
    check(native_restored.inspect(Json::object({{"kind", Json{"index"}}})) ==
          expected_index);
    check(native_restored.finish_workflow(true) == Json{}); // orphan never executes
    check(native_restored.items().back() == restored.items().back());
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
    std::filesystem::remove_all(path);
    return 1;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
  std::filesystem::remove_all(path);
}
