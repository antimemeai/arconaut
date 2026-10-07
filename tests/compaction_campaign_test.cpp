#include "blackbird/coding.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <unistd.h>
using namespace blackbird;
namespace {
void require(bool yes, std::string_view why) {
  if (!yes)
    throw std::runtime_error(std::string{why});
}
template <class T> T identity(unsigned char n) {
  IdentityBytes bytes{};
  bytes[0] = std::byte{n};
  return unwrap(T::from_bytes(bytes));
}
bool accepted(const Json &result) { return field(result, "accepted") == Json{true}; }
Json numeric(std::size_t n) { return Json{JsonNumber{std::to_string(n)}}; }
std::string decode_hex(const Json &page) {
  const auto &hex = string_field(page, "bytes");
  require(hex.size() % 2 == 0, "hex parity");
  auto digit = [](char c) -> unsigned {
    return c <= '9' ? static_cast<unsigned>(c - '0')
                    : static_cast<unsigned>(c - 'a' + 10);
  };
  std::string out;
  for (std::size_t i = 0; i < hex.size(); i += 2)
    out += static_cast<char>((digit(hex[i]) << 4U) | digit(hex[i + 1]));
  return out;
}
class MockProvider final : public CodingProvider {
public:
  std::size_t calls = 0, bytes = 0;
  Json::Array expected_input;
  Json respond(const Json &request,
               const std::function<void(std::string_view)> &capture) override {
    ++calls;
    require(field(request, "input") == Json{expected_input},
            "actual assembled provider input");
    require(unwrap(dump_json(field(request, "input"))).find("needle-") ==
                std::string::npos,
            "archived scale bytes excluded at provider boundary");
    bytes = unwrap(dump_json(request)).size();
    const auto response = Json::object({{"output", Json{Json::Array{}}}});
    capture(unwrap(dump_json(response)));
    return response;
  }
};
std::uintmax_t disk_bytes(const std::filesystem::path &path) {
  std::uintmax_t total = 0;
  for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
    if (entry.is_regular_file())
      total += entry.file_size();
  return total;
}
struct Custody final : CustodyVerifier {
  Result<void> verify(std::span<const AttemptState> a) override {
    return a.empty() ? Result<void>::success()
                     : Result<void>::failure({ErrorCode::conflict});
  }
};
// Independent logical model: capture ordinals and immutable values, not journal
// packets. Generated opaque identities are bound to their known revision/index;
// contents and transition choices never come from the actual candidate/resulting
// presentation.
struct Model {
  std::vector<std::string> ids;
  std::vector<Json> originals;
  std::vector<std::vector<std::size_t>> ancestors;
  std::vector<std::size_t> visible;
  void capture(std::string id, Json value, std::vector<std::size_t> ancestry = {}) {
    ids.push_back(std::move(id));
    originals.push_back(std::move(value));
    ancestors.push_back(std::move(ancestry));
    visible.push_back(ids.size() - 1);
  }
  Json::Array identifiers(const std::vector<std::size_t> &selected) const {
    Json::Array out;
    for (const auto i : selected)
      out.push_back(Json{ids[i]});
    return out;
  }
  Json::Array items() const {
    Json::Array out;
    for (const auto i : visible)
      out.push_back(originals[i]);
    return out;
  }
  void filter(const std::vector<std::size_t> &selected, bool keep) {
    std::erase_if(visible, [&](std::size_t i) {
      return (std::find(selected.begin(), selected.end(), i) != selected.end()) != keep;
    });
  }
  void restore(std::vector<std::size_t> selected) {
    filter(selected, false);
    std::sort(selected.begin(), selected.end());
    for (const auto i : selected) {
      const auto before = std::find_if(visible.begin(), visible.end(),
                                       [&](std::size_t j) { return j > i; });
      visible.insert(before, i);
    }
  }
  void check(const ContextStore &context) const {
    require(context.items() == items(), "independent exact item model");
    const auto view = context.view();
    const auto &entries = field(view, "entries").array();
    require(entries.size() == visible.size(), "model size");
    for (std::size_t p = 0; p < visible.size(); ++p) {
      const auto i = visible[p];
      require(string_field(entries[p], "id") == ids[i],
              "independent selected identities/order");
      if (!ancestors[i].empty())
        require(field(entries[p], "ancestry") == Json{identifiers(ancestors[i])},
                "summary ancestry model");
    }
    const auto actual = context.originals().array();
    require(actual.size() == originals.size(), "immutable source count");
    for (std::size_t i = 0; i < ids.size(); ++i) {
      const auto found = std::find_if(actual.begin(), actual.end(), [&](const Json &e) {
        return string_field(e, "id") == ids[i];
      });
      require(found != actual.end() && field(*found, "item") == originals[i],
              "independent immutable source bytes");
    }
  }
};
} // namespace
int main() {
  char name[] = "/tmp/arco-compaction-campaign-XXXXXX";
  const auto *path = mkdtemp(name);
  if (!path)
    return 2;
  std::string phase = "initial-create";
  try {
    const JournalHeader header{identity<EnvironmentId>(1),
                               identity<AuditStreamId>(2),
                               3,
                               {4 * 1024 * 1024, 8 * 1024 * 1024},
                               std::nullopt};
    const JournalCapacity capacity{128 * 1024 * 1024, 16384};
    auto root =
        unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                         unwrap(NativeJournalDirectory::open(path))),
                                     "audit", header, capacity));
    auto log = std::make_unique<AuditLog>(*root);
    auto context = std::make_unique<ContextStore>(*log);
    auto proposal = [&](std::string mode, Json::Array ids) {
      return Json::object({{"base", Json{context->head()}},
                           {"mode", Json{std::move(mode)}},
                           {"ids", Json{std::move(ids)}},
                           {"reason", Json{"source-inspired independent model"}},
                           {"source", Json{"explicit deterministic input"}}});
    };
    // Direct red: managed publication must not admit malformed supported
    // representation.
    context->append(
        {Json::object({{"type", numeric(7)}, {"opaque", Json{"untouched"}}})},
        "malformed-type");
    require(!accepted(context->manage(proposal("archive", {}))),
            "RED malformed type accepted by managed publication");
    const auto malformed_id = context->head() + ".0";
    MockProvider malformed_provider;
    CodingEngine malformed_engine{*log, *context, malformed_provider, "test"};
    bool request_failed = false;
    try {
      malformed_engine.turn({"", "arco.request()"});
    } catch (const Error &) {
      request_failed = true;
    }
    require(request_failed && malformed_provider.calls == 0,
            "malformed request rejected before provider dispatch");
    malformed_engine.turn(
        {"", "local v=arco.context(); arco.manage({base=v.base,mode='archive',ids={'" +
                 malformed_id +
                 "'},reason='malformed repair',source='explicit Lua-only repair'})"});
    require(context->items().empty(),
            "actual Lua workflow repairs malformed item by explicit archive");
    malformed_engine.turn({"", "arco.request()"});
    require(malformed_provider.calls == 1,
            "request continues after malformed repair without effect replay");
    auto clear = context->view();
    clear.object()[1].second.array().clear();
    require(accepted(context->edit(clear)), "generic fixture reset");
    // Start a separate model domain; prior malformed source remains retained, so
    // recreate.
    context.reset();
    log.reset();
    root.reset();
    phase = "recreate";
    std::filesystem::remove_all(path);
    std::filesystem::create_directory(path);
    std::filesystem::permissions(path, std::filesystem::perms::owner_all);
    root = unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                            unwrap(NativeJournalDirectory::open(path))),
                                        "audit", header, capacity));
    log = std::make_unique<AuditLog>(*root);
    context = std::make_unique<ContextStore>(*log);
    phase = "seed-model";
    Model expected;
    auto append = [&](Json item) {
      context->append({item}, "modeled input");
      expected.capture(context->head() + ".0", std::move(item));
    };
    append(Json::object(
        {{"role", Json{"user"}},
         {"content",
          Json{"MISSION: no effects replay, exact repair, no automatic threshold"}}}));
    for (unsigned i = 0; i < 12; ++i)
      append(Json::object({{"role", Json{"assistant"}},
                           {"content", Json{"fact-é-" + std::to_string(i)}},
                           {"extension", Json{JsonNumber{"9007199254740993"}}}}));
    Json rejected_candidate;
    std::size_t transitions = 0;
    std::string stale = context->head();
    for (std::size_t step = 0; step < 160; ++step) {
      phase = "step-" + std::to_string(step);
      const auto action = step % 10;
      std::vector<std::size_t> chosen;
      for (const auto i : expected.visible)
        if (i != 0 && (i + step) % 3 == 0)
          chosen.push_back(i);
      if (action == 0 ||
          (chosen.empty() && (action == 1 || action == 2 || action == 3))) {
        stale = context->head();
        append(Json::object(
            {{"type", Json{"reasoning"}},
             {"encrypted_content", Json{"OPAQUE==" + std::to_string(step)}}}));
      } else if (action == 1) {
        require(accepted(
                    context->manage(proposal("archive", expected.identifiers(chosen)))),
                "archive accepted");
        expected.filter(chosen, false);
      } else if (action == 2) {
        std::vector<std::size_t> keep{0};
        for (const auto i : expected.visible)
          if (i != 0 && (i + step) % 2 == 0)
            keep.push_back(i);
        require(
            accepted(context->manage(proposal("select", expected.identifiers(keep)))),
            "selection accepted");
        expected.filter(keep, true);
      } else if (action == 3) {
        const auto summary = Json::object(
            {{"role", Json{"assistant"}},
             {"content", Json{"explicit summary-" + std::to_string(step)}}});
        auto p = proposal("summarize", expected.identifiers(chosen));
        p.object().emplace_back("summary", summary);
        const auto position =
            static_cast<std::size_t>(std::find(expected.visible.begin(),
                                               expected.visible.end(), chosen.front()) -
                                     expected.visible.begin());
        require(accepted(context->manage(p)), "summary accepted");
        expected.filter(chosen, false);
        expected.capture(context->head() + ".0", summary, chosen);
        const auto fresh = expected.visible.back();
        expected.visible.pop_back();
        expected.visible.insert(
            expected.visible.begin() + static_cast<std::ptrdiff_t>(position), fresh);
      } else if (action == 4 || action == 9) {
        chosen.clear();
        for (std::size_t i = 1; i < expected.ids.size(); ++i)
          if ((i + step) % 4 == 0)
            chosen.push_back(i);
        require(accepted(
                    context->manage(proposal("restore", expected.identifiers(chosen)))),
                "sparse restore accepted");
        expected.restore(chosen);
      } else if (action == 5) {
        auto p = proposal("archive", {});
        p.object()[0].second = Json{stale};
        rejected_candidate = p;
        const auto before = context->view();
        require(!accepted(context->manage(p)), "stale rejected");
        require(context->view() == before, "stale changes no view");
      } else if (action == 6) {
        context->begin_workflow();
        const auto staged =
            context->manage(proposal("archive", expected.identifiers(chosen)));
        require(field(staged, "staged") == Json{true}, "stage status");
        expected.check(*context);
        require(!accepted(context->finish_workflow(false)),
                "cancel accepted no publication");
      } else if (action == 7) {
        context.reset();
        log.reset();
        root.reset();
        root =
            unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(
                                           unwrap(NativeJournalDirectory::open(path))),
                                       "audit", header, capacity));
        unwrap(root->confirm_recovery());
        Custody custody;
        unwrap(root->reconcile(custody));
        log = std::make_unique<AuditLog>(*root);
        context = std::make_unique<ContextStore>(*log);
      } else if (action == 8) {
        context->begin_workflow();
        (void)context->manage(proposal("archive", expected.identifiers(chosen)));
        auto reordered = context->view();
        if (expected.visible.size() > 2) {
          std::swap(expected.visible[1], expected.visible.back());
          std::swap(reordered.object()[1].second.array()[1],
                    reordered.object()[1].second.array().back());
        }
        require(accepted(context->edit(reordered)), "persistent generic reorder");
        require(!accepted(context->finish_workflow(true)),
                "generic edit invalidates staged base");
      }
      expected.check(*context);
      ++transitions;
    }
    // Complete paged source bytes, including split UTF-8, after many compact/reopen
    // cycles.
    phase = "inspection";
    const auto source = context->originals().array();
    for (const auto &entry : source) {
      const auto original_index =
          static_cast<std::size_t>(std::find(expected.ids.begin(), expected.ids.end(),
                                             string_field(entry, "id")) -
                                   expected.ids.begin());
      auto exact_entry = Json::object({{"id", Json{expected.ids[original_index]}},
                                       {"item", expected.originals[original_index]}});
      if (!expected.ancestors[original_index].empty())
        exact_entry.object().emplace_back(
            "ancestry", Json{expected.identifiers(expected.ancestors[original_index])});
      const auto expected_bytes = unwrap(dump_json(exact_entry));
      std::string recovered;
      for (std::size_t offset = 0; offset < expected_bytes.size(); offset += 7) {
        auto page = context->inspect(Json::object({{"kind", Json{"originals"}},
                                                   {"entry", field(entry, "id")},
                                                   {"offset", numeric(offset)},
                                                   {"limit", numeric(7)}}));
        require(field(page, "total_bytes") == numeric(expected_bytes.size()),
                "independent paged source length");
        require(field(page, "next") ==
                    numeric(std::min(offset + 7, expected_bytes.size())),
                "independent page cursor");
        recovered += decode_hex(page);
      }
      // Inspector captures summary ancestry in addition to original item/id.
      require(recovered == expected_bytes, "paged exact capture serialization");
      const auto eof =
          context->inspect(Json::object({{"kind", Json{"originals"}},
                                         {"entry", Json{expected.ids[original_index]}},
                                         {"offset", numeric(expected_bytes.size())}}));
      require(decode_hex(eof).empty() &&
                  field(eof, "next") == numeric(expected_bytes.size()),
              "source EOF page");
      const auto parsed = unwrap(parse_json(recovered));
      require(field(parsed, "id") == Json{expected.ids[original_index]} &&
                  field(parsed, "item") == expected.originals[original_index],
              "paged exact source roundtrip");
    }
    for (const auto limit : {0U, 65537U}) {
      bool rejected = false;
      try {
        (void)context->inspect(
            Json::object({{"kind", Json{"index"}}, {"limit", numeric(limit)}}));
      } catch (const Error &e) {
        rejected = e.code == ErrorCode::invalid_range;
      }
      require(rejected, "invalid bounded inspection limit");
    }
    for (const auto &offset : {Json{JsonNumber{"-1"}}, Json{"nonnumeric"}}) {
      bool rejected = false;
      try {
        (void)context->inspect(
            Json::object({{"kind", Json{"index"}}, {"offset", offset}}));
      } catch (const Error &e) {
        rejected = e.code == ErrorCode::invalid_range;
      }
      require(rejected, "invalid inspection offset rejected");
    }
    // History is source recovery, not a receipt test: recover exact rejected input.
    std::string history_bytes;
    std::size_t history_offset = 0;
    for (;;) {
      const auto page =
          context->inspect(Json::object({{"kind", Json{"history"}},
                                         {"offset", numeric(history_offset)},
                                         {"limit", numeric(65536)}}));
      history_bytes += decode_hex(page);
      history_offset =
          static_cast<std::size_t>(std::stoull(field(page, "next").number().text));
      if (field(page, "next") == field(page, "total_bytes"))
        break;
    }
    const auto history = unwrap(parse_json(history_bytes));
    require(std::any_of(history.array().begin(), history.array().end(),
                        [&](const Json &packet) {
                          const auto *candidate = packet.find("candidate"),
                                     *outcome = packet.find("outcome");
                          return candidate && outcome &&
                                 *candidate == rejected_candidate &&
                                 field(packet, "accepted") == Json{false} &&
                                 string_field(*outcome, "reason") == "stale-base";
                        }),
            "exact stale candidate recoverable from bounded native history");
    for (const auto &kind : {"history", "originals", "index"}) {
      const auto page = context->inspect(
          Json::object({{"kind", Json{kind}}, {"offset", numeric(SIZE_MAX)}}));
      require(decode_hex(page).empty() &&
                  field(page, "next") == field(page, "total_bytes"),
              "beyond EOF defined as clamped empty page");
    }
    bool unknown = false;
    try {
      (void)context->inspect(
          Json::object({{"kind", Json{"originals"}}, {"entry", Json{"unknown"}}}));
    } catch (const Error &e) {
      unknown = e.code == ErrorCode::invalid_identity;
    }
    require(unknown, "unknown original inspection");
    // Additional ordering/repair cases attack protocol faults, not transition counts.
    const auto good = context->view();
    const auto call_item = Json::object(
        {{"type", Json{"function_call"}}, {"call_id", Json{"parallel-A"}}});
    const auto other_call = Json::object(
        {{"type", Json{"function_call"}}, {"call_id", Json{"parallel-B"}}});
    const auto result_item = Json::object({{"type", Json{"function_call_output"}},
                                           {"call_id", Json{"parallel-A"}},
                                           {"output", Json{"α exact output"}}});
    const auto other_result = Json::object({{"type", Json{"function_call_output"}},
                                            {"call_id", Json{"parallel-B"}},
                                            {"output", Json{"β exact output"}}});
    context->append({call_item, other_call, result_item, other_result},
                    "parallel fixture");
    const auto parallel_base = context->head();
    Json::Array group_ids;
    for (unsigned i = 0; i < 4; ++i)
      group_ids.push_back(Json{parallel_base + "." + std::to_string(i)});
    require(
        !accepted(context->manage(proposal("archive", {group_ids[0], group_ids[2]}))),
        "parallel interval partial rejected");
    require(accepted(context->manage(proposal("archive", group_ids))),
            "parallel whole archive");
    require(accepted(context->manage(proposal("restore", group_ids))),
            "parallel exact repair");
    auto bad_order = context->view();
    auto &bad_entries = bad_order.object()[1].second.array();
    const auto start = bad_entries.size() - 4;
    std::swap(bad_entries[start], bad_entries[start + 2]);
    bad_entries[start + 3].object()[1].second =
        Json::object({{"type", Json{"function_call_output"}},
                      {"call_id", Json{"parallel-B"}},
                      {"output", Json{"CORRUPTED"}}});
    require(accepted(context->edit(bad_order)), "generic invalid order fixture");
    require(!accepted(context->manage(proposal("archive", {}))),
            "invalid ordering cannot publish managed view");
    require(accepted(context->manage(proposal("restore", group_ids))),
            "edited reordered parallel repair");
    const auto repaired_items = context->items();
    require(repaired_items[start] == call_item &&
                repaired_items[start + 1] == other_call &&
                repaired_items[start + 2] == result_item &&
                repaired_items[start + 3] == other_result,
            "parallel bytes/order exact");
    const auto expected_repaired_view = context->view();
    context.reset();
    log.reset();
    root.reset();
    root = unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(
                                          unwrap(NativeJournalDirectory::open(path))),
                                      "audit", header, capacity));
    unwrap(root->confirm_recovery());
    Custody protocol_custody;
    unwrap(root->reconcile(protocol_custody));
    log = std::make_unique<AuditLog>(*root);
    context = std::make_unique<ContextStore>(*log);
    require(context->view() == expected_repaired_view &&
                context->items() == repaired_items,
            "native reopen exact repaired parallel group");
    context->append({call_item, result_item}, "duplicate call fixture");
    require(!accepted(context->manage(proposal("archive", {}))),
            "duplicate protocol IDs rejected");
    auto reset = good;
    reset.object()[0].second = Json{context->head()};
    require(accepted(context->edit(reset)), "protocol fixture reset");
    std::cout << "modeled_transitions=" << transitions
              << " immutable_originals=" << expected.ids.size()
              << " native_reopens=16 input_bytes="
              << field(context->stats(), "input_bytes").number().text << '\n';
    phase = "scale";
    context.reset();
    log.reset();
    root.reset();
    const auto scale_path = std::filesystem::path{path} / "scale";
    std::filesystem::create_directory(scale_path);
    std::filesystem::permissions(scale_path, std::filesystem::perms::owner_all);
    root = unwrap(
        RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                  NativeJournalDirectory::open(scale_path.string()))),
                              "audit", header, capacity));
    log = std::make_unique<AuditLog>(*root);
    context = std::make_unique<ContextStore>(*log);
    context->append(
        {Json::object(
            {{"role", Json{"user"}},
             {"content",
              Json{"SCALE MISSION: compact explicitly, exact original repair"}}})},
        "scale mission");
    Json::Array scale_originals;
    std::string scale_needle_id;
    Json scale_needle_expected;
    for (unsigned turn = 0; turn < 64; ++turn) {
      const auto call_id = "scale-call-" + std::to_string(turn);
      const auto output = "needle-" + std::to_string(turn) +
                          ":é:" + std::string(8192, static_cast<char>('a' + turn % 26));
      Json::Array batch{
          Json::object(
              {{"type", Json{"reasoning"}},
               {"encrypted_content", Json{"OPAQUE==" + std::string(128, 'x')}}}),
          Json::object({{"type", Json{"function_call"}},
                        {"call_id", Json{call_id}},
                        {"name", Json{"exec"}},
                        {"arguments", Json{"{}"}}}),
          Json::object({{"type", Json{"function_call_output"}},
                        {"call_id", Json{call_id}},
                        {"output", Json{output}}}),
          Json::object({{"role", Json{"assistant"}},
                        {"content", Json{"result-" + std::to_string(turn)}}})};
      context->append(batch, "scale completed turn");
      if (turn == 17) {
        scale_needle_id = context->head() + ".2";
        scale_needle_expected =
            Json::object({{"id", Json{scale_needle_id}}, {"item", batch[2]}});
      }
      scale_originals.insert(scale_originals.end(), batch.begin(), batch.end());
    }
    const auto scale_before = unwrap(dump_json(Json{context->items()})).size();
    const auto audit_before = disk_bytes(scale_path);
    Json::Array covered;
    const auto scale_view = context->view();
    for (std::size_t i = 1; i < field(scale_view, "entries").array().size(); ++i)
      covered.push_back(field(field(scale_view, "entries").array()[i], "id"));
    auto scale_proposal = proposal("summarize", covered);
    scale_proposal.object().emplace_back(
        "summary",
        Json::object(
            {{"role", Json{"assistant"}},
             {"content", Json{"64 completed tool turns; sources retained by ancestry. "
                              "No semantic-fidelity claim for this scale fixture."}}}));
    using Clock = std::chrono::steady_clock;
    auto begin = Clock::now();
    require(accepted(context->manage(scale_proposal)), "scale summary publish");
    const auto publish_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - begin).count();
    const auto scale_after = unwrap(dump_json(Json{context->items()})).size();
    const auto audit_after = disk_bytes(scale_path);
    const auto scale_head = context->head();
    context.reset();
    log.reset();
    root.reset();
    begin = Clock::now();
    root = unwrap(
        RetainedState::open(std::make_unique<NativeJournalDirectory>(unwrap(
                                NativeJournalDirectory::open(scale_path.string()))),
                            "audit", header, capacity));
    unwrap(root->confirm_recovery());
    Custody scale_custody;
    unwrap(root->reconcile(scale_custody));
    log = std::make_unique<AuditLog>(*root);
    context = std::make_unique<ContextStore>(*log);
    const auto replay_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - begin).count();
    require(context->head() == scale_head, "scale reopen compacted head");
    const auto immutable_scale = context->originals().array();
    for (const auto &item : scale_originals)
      require(std::any_of(immutable_scale.begin(), immutable_scale.end(),
                          [&](const Json &e) { return field(e, "item") == item; }),
              "scale original exact after native replay");
    require(immutable_scale.size() == scale_originals.size() + 2,
            "scale exact capture count incl mission and summary");
    std::string recovered_needle;
    const auto expected_needle = unwrap(dump_json(scale_needle_expected));
    for (std::size_t offset = 0; offset < expected_needle.size(); offset += 4096)
      recovered_needle +=
          decode_hex(context->inspect(Json::object({{"kind", Json{"originals"}},
                                                    {"entry", Json{scale_needle_id}},
                                                    {"offset", numeric(offset)},
                                                    {"limit", numeric(4096)}})));
    require(recovered_needle == expected_needle,
            "bounded exact 8KiB omitted source after scale native reopen");
    // Independent native packet bytes: aggregate history exceeds one JSON document.
    std::string expected_large_history = "[";
    bool first_packet = true;
    for (const auto &fact : root->committed_facts()) {
      const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
      if (!record || record->channel != ApplicationChannel::context)
        continue;
      if (!first_packet)
        expected_large_history += ',';
      first_packet = false;
      expected_large_history.append(
          reinterpret_cast<const char *>(record->payload.data()),
          record->payload.size());
    }
    expected_large_history += ']';
    require(expected_large_history.size() > JsonLimits{}.bytes,
            "large history really exceeds default aggregate JSON ceiling");
    for (const auto offset :
         {std::size_t{0}, expected_large_history.size() / 2, JsonLimits{}.bytes - 3,
          expected_large_history.size() - 8, expected_large_history.size() + 1}) {
      const auto page = context->inspect(Json::object({{"kind", Json{"history"}},
                                                       {"offset", numeric(offset)},
                                                       {"limit", numeric(7)}}));
      const auto clamped = std::min(offset, expected_large_history.size());
      const auto wanted = expected_large_history.substr(clamped, 7);
      require(decode_hex(page) == wanted &&
                  field(page, "total_bytes") ==
                      numeric(expected_large_history.size()) &&
                  field(page, "next") == numeric(clamped + wanted.size()),
              "large history exact bounded native-source page");
    }
    MockProvider mock;
    mock.expected_input = context->items();
    CodingEngine engine{*log, *context, mock, "test"};
    begin = Clock::now();
    engine.turn({"", "arco.request()"});
    const auto assembly_ms =
        std::chrono::duration<double, std::milli>(Clock::now() - begin).count();
    require(mock.calls == 1, "scale replay never requested provider");
    std::cout << "scale_turns=64 input_before=" << scale_before
              << " input_after=" << scale_after << " audit_before=" << audit_before
              << " audit_after=" << audit_after << " publish_ms=" << publish_ms
              << " native_replay_ms=" << replay_ms
              << " request_assembly_audit_mock_ms=" << assembly_ms
              << " request_bytes=" << mock.bytes << '\n';
  } catch (const Error &e) {
    std::cerr << phase << ": " << error_name(e.code) << " native=" << e.detail << '\n';
    std::filesystem::remove_all(path);
    return 1;
  } catch (const std::exception &e) {
    std::cerr << phase << ": " << e.what() << '\n';
    std::filesystem::remove_all(path);
    return 1;
  }
  std::filesystem::remove_all(path);
}
