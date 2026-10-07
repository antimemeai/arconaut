#include "blackbird/context.hpp"
#include <algorithm>
#include <charconv>
namespace blackbird {
namespace {
Json number(std::size_t n) { return Json{JsonNumber{std::to_string(n)}}; }
std::size_t index(const Json &q, std::string_view key, std::size_t fallback) {
  const auto *v = q.find(key);
  if (!v)
    return fallback;
  const auto *n = std::get_if<JsonNumber>(&v->value());
  if (!n || n->text.empty())
    throw Error{ErrorCode::invalid_range};
  std::size_t out = 0;
  const auto &s = n->text;
  const auto r = std::from_chars(s.data(), s.data() + s.size(), out);
  if (r.ec != std::errc{} || r.ptr != s.data() + s.size())
    throw Error{ErrorCode::invalid_range};
  return out;
}
Json summary(const RetainedFact &fact, std::size_t i) {
  Json row = Json::object(
      {{"record", number(i)},
       {"journal", Json{hex_identity(fact.record.journal.bytes())}},
       {"sequence", number(fact.record.sequence)},
       {"kind", number(static_cast<std::size_t>(retained_kind(fact.event.body)))}});
  auto add = [&](std::string key, Json v) {
    row.object().emplace_back(std::move(key), std::move(v));
  };
  auto identity = [&](std::string key, const auto &id) {
    add(std::move(key), Json{hex_identity(id.bytes())});
  };
  Json::Array sources;
  for (const auto &s : std::span{fact.event.dependencies}.first(
           std::min<std::size_t>(64, fact.event.dependencies.size())))
    sources.push_back(Json::object({{"journal", Json{hex_identity(s.journal.bytes())}},
                                    {"sequence", number(s.sequence)}}));
  add("sources", Json{std::move(sources)});
  add("source_count", number(fact.event.dependencies.size()));
  std::visit(
      [&](const auto &e) {
        using T = std::decay_t<decltype(e)>;
        if constexpr (std::is_same_v<T, ApplicationRecordEvent>) {
          identity("id", e.identity);
          add("channel", number(static_cast<std::size_t>(e.channel)));
          add("payload_bytes", number(e.payload.size()));
          // Context packets can be enormous; their full JSON is recovered only on
          // demand.
          if (e.payload.size() <= 65536 && (e.channel == ApplicationChannel::log ||
                                            e.channel == ApplicationChannel::program)) {
            const auto p = unwrap(parse_json(read_text(e.payload)));
            for (const auto key : {"label", "generation", "activation"})
              if (const auto *v = p.find(key);
                  v && std::holds_alternative<std::string>(v->value()) &&
                  v->string().size() <= 256)
                add(key, *v);
            if (const auto *m = p.find("metadata")) {
              Json::Object kept;
              for (const auto key :
                   {"attempt", "revision", "generation", "operation", "complaint"})
                if (const auto *v = m->find(key);
                    v && std::holds_alternative<std::string>(v->value()) &&
                    v->string().size() <= 256)
                  kept.emplace_back(key, *v);
              add("metadata", Json::object(std::move(kept)));
            }
          }
        } else if constexpr (std::is_same_v<T, DecisionEvent>) {
          identity("decision", e.decision);
          identity("actor", e.actor);
          identity("conversation", e.conversation);
          identity("workflow", e.workflow);
          identity("generation", e.definition);
          identity("revision", e.context);
          add("payload_bytes", number(e.continuation.size()));
        } else if constexpr (std::is_same_v<T, InvocationEvent>) {
          identity("invocation", e.invocation);
          identity("decision", e.decision);
          identity("generation", e.definition);
          add("payload_bytes", number(e.input.size()));
        } else if constexpr (std::is_same_v<T, AttemptAdmissionEvent>) {
          identity("attempt", e.attempt);
          identity("invocation", e.invocation);
          identity("decision", e.decision);
          add("payload_bytes", number(e.input.size()));
          add("outcome", Json{"not established by admission"});
        } else if constexpr (std::is_same_v<T, AttemptObservationEvent>) {
          identity("attempt", e.attempt);
          add("phase", number(static_cast<std::size_t>(e.phase)));
          constexpr const char *outcomes[] = {"none", "success", "failure",
                                              "cancellation", "unknown"};
          add("outcome", Json{outcomes[static_cast<std::size_t>(e.disposition)]});
          add("payload_bytes", number(e.observation.size()));
        } else if constexpr (std::is_same_v<T, AttemptOpenEvent> ||
                             std::is_same_v<T, AdapterReceiptEvent>) {
          identity("attempt", e.attempt);
        } else if constexpr (std::is_same_v<T, ComplaintEvent>) {
          identity("complaint", e.complaint);
          identity("actor", e.actor);
          add("payload_bytes", number(e.detail.size()));
        }
      },
      fact.event.body);
  return row;
}
std::vector<std::byte> payload(const RetainedBody &body) {
  return std::visit(
      [](const auto &e) -> std::vector<std::byte> {
        using T = std::decay_t<decltype(e)>;
        if constexpr (std::is_same_v<T, ApplicationRecordEvent>)
          return unwrap(e.payload.read());
        else if constexpr (std::is_same_v<T, DecisionEvent>)
          return unwrap(e.continuation.read());
        else if constexpr (std::is_same_v<T, InvocationEvent> ||
                           std::is_same_v<T, AttemptAdmissionEvent>)
          return unwrap(e.input.read());
        else if constexpr (std::is_same_v<T, AttemptObservationEvent>)
          return unwrap(e.observation.read());
        else if constexpr (std::is_same_v<T, ComplaintEvent>)
          return unwrap(e.detail.read());
        else
          return {};
      },
      body);
}
} // namespace
Json AuditLog::inspect(const Json &q) {
  if (!std::holds_alternative<Json::Object>(q.value()))
    throw Error{ErrorCode::invalid_range};
  const auto facts = root_.committed_facts();
  const auto end = index(q, "end", facts.size());
  const auto count = index(q, "count", 32), limit = index(q, "limit", 4096);
  if (end > facts.size() || count == 0 || count > 64 || limit == 0 || limit > 65536)
    throw Error{ErrorCode::invalid_range};
  if (q.find("record")) {
    const auto i = index(q, "record", 0);
    if (i >= end)
      throw Error{ErrorCode::invalid_range};
    const auto &fact = facts[i];
    auto result = summary(fact, i);
    std::vector<std::byte> original;
    auto raw = payload(fact.event.body);
    if (q.find("source")) {
      const auto s = index(q, "source", 0);
      if (s >= fact.event.dependencies.size())
        throw Error{ErrorCode::invalid_range};
      original = unwrap(root_.source(fact.event.dependencies[s]));
      raw = original;
    }
    const auto offset = std::min(index(q, "offset", 0), raw.size());
    const auto n = std::min(limit, raw.size() - offset);
    constexpr char digits[] = "0123456789abcdef";
    std::string hex;
    hex.reserve(n * 2);
    for (const auto b : std::span<const std::byte>{raw}.subspan(offset, n)) {
      const auto v = std::to_integer<unsigned>(b);
      hex += digits[v >> 4U];
      hex += digits[v & 15U];
    }
    result.object().emplace_back("end", number(end));
    result.object().emplace_back("total_bytes", number(raw.size()));
    result.object().emplace_back("offset", number(offset));
    result.object().emplace_back("next", number(offset + n));
    result.object().emplace_back("encoding", Json{"hex-original-bytes"});
    result.object().emplace_back("hex", Json{std::move(hex)});
    return result;
  }
  const auto cursor = std::min(index(q, "cursor", 0), end);
  const auto next = cursor + std::min(count, end - cursor);
  Json::Array rows;
  for (auto i = cursor; i < next; ++i)
    rows.push_back(summary(facts[i], i));
  return Json::object(
      {{"end", number(end)},
       {"next", number(next)},
       {"records", Json{std::move(rows)}},
       {"scope", Json{"committed facts only; end pins append-only prefix; record "
                      "indices stable within this journal lineage"}}});
}
} // namespace blackbird
