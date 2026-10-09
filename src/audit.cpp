#include "blackbird/context.hpp"
#include <algorithm>
#include <charconv>
#include <chrono>
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
            const auto p = unwrap(read_packet(e.payload));
            if (const auto *t = p.find("time"))
              add("time", *t);
            if (const auto *variable = p.find("variable");
                variable && std::holds_alternative<std::string>(variable->value()) &&
                p.find("label") &&
                std::holds_alternative<std::string>(p.find("label")->value()) &&
                p.find("label")->string() == "variable.sample") {
              add("variable", *variable);
              for (const auto key :
                   {"sample_id", "value", "source", "observation_status"})
                if (const auto *v = p.find(key))
                  add(key, *v);
            }
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
              if (const auto *t = m->find("time"))
                add("time", *t);
              if (const auto *duration = m->find("duration_ns"))
                add("duration_ns", *duration);
              if (const auto *operation = m->find("operation"))
                add("operation", *operation);
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
          if (e.continuation.size() <= 4096) {
            const auto packet = read_packet(e.continuation);
            if (packet.has_value())
              if (const auto *t = packet.value().find("time"))
                add("time", *t);
            if (packet.has_value())
              if (const auto *name = packet.value().find("operation");
                  name && std::holds_alternative<std::string>(name->value()) &&
                  name->string().size() <= 256)
                add("operation", *name);
          }
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
          if (e.observation.size() <= 4096) {
            const auto packet = read_packet(e.observation);
            if (packet.has_value())
              for (const auto key : {"result_id", "result_record"})
                if (const auto *v = packet.value().find(key)) add(key, *v);
          }
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
Json AuditLog::trajectory(const Json &q) {
  if (!std::holds_alternative<Json::Object>(q.value()))
    throw Error{ErrorCode::invalid_range};
  for (const auto &[key, value] : q.object()) {
    (void)value;
    if (key != "cursor" && key != "end" && key != "count" && key != "scan" &&
        key != "attempt" && key != "variables" && key != "variable")
      throw Error{ErrorCode::invalid_range};
  }
  const auto end = index(q, "end", root_.fact_count());
  const auto count = index(q, "count", 32), scan = index(q, "scan", 128);
  if (end > root_.fact_count() || count == 0 || count > 64 || scan == 0 || scan > 256)
    throw Error{ErrorCode::invalid_range};
  std::string attempt, invocation, decision;
  bool variables = false;
  std::string variable;
  if (const auto *v = q.find("variables")) {
    const auto *b = std::get_if<bool>(&v->value());
    if (!b)
      throw Error{ErrorCode::invalid_range};
    variables = *b;
  }
  if (const auto *v = q.find("variable")) {
    const auto *s = std::get_if<std::string>(&v->value());
    if (!s || s->empty() || s->size() > 64)
      throw Error{ErrorCode::invalid_range};
    variable = *s;
    variables = true;
  }
  if (const auto *selector = q.find("attempt")) {
    const auto *text = std::get_if<std::string>(&selector->value());
    if (!text || text->size() != 32)
      throw Error{ErrorCode::invalid_range};
    IdentityBytes bytes{};
    auto nibble = [](char c) -> unsigned {
      if (c >= '0' && c <= '9')
        return static_cast<unsigned>(c - '0');
      if (c >= 'a' && c <= 'f')
        return static_cast<unsigned>(c - 'a') + 10U;
      throw Error{ErrorCode::invalid_range};
    };
    for (std::size_t i = 0; i < bytes.size(); ++i)
      bytes[i] = std::byte{static_cast<unsigned char>((nibble((*text)[2 * i]) << 4U) |
                                                      nibble((*text)[2 * i + 1]))};
    const auto state =
        unwrap(root_.attempt(unwrap(OperationAttemptId::from_bytes(bytes))));
    attempt = *text;
    invocation = hex_identity(state.admission.invocation.bytes());
    decision = hex_identity(state.admission.decision.bytes());
  }
  const auto cursor = index(
      q, "cursor", attempt.empty() && !variables ? (end > 128 ? end - 128 : 0) : 0);
  if (cursor > end)
    throw Error{ErrorCode::invalid_range};
  const auto stop = cursor + std::min(scan, end - cursor);
  auto next = cursor;
  Json::Array rows;
  while (next < stop && rows.size() < count) {
    auto row = summary(unwrap(root_.fact(next)), next);
    ++next;
    if (variables) {
      const auto *v = row.find("variable");
      if (!v || (!variable.empty() && v->string() != variable))
        continue;
    }
    if (!attempt.empty()) {
      auto matches = [&](std::string_view key, const std::string &value) {
        const auto *v = row.find(key);
        return v && v->string() == value;
      };
      const auto *metadata = row.find("metadata");
      const auto *capture_attempt = metadata ? metadata->find("attempt") : nullptr;
      const auto *source = row.find("source");
      const auto *sample_attempt = source ? source->find("attempt") : nullptr;
      if (!matches("attempt", attempt) &&
          !(row.find("attempt") == nullptr && matches("invocation", invocation)) &&
          !(row.find("attempt") == nullptr && row.find("invocation") == nullptr &&
            matches("decision", decision)) &&
          !(capture_attempt && capture_attempt->string() == attempt) &&
          !(sample_attempt && sample_attempt->string() == attempt))
        continue;
    }
    const auto kind =
        static_cast<std::size_t>(std::stoull(field(row, "kind").number().text));
    constexpr const char *names[] = {"",
                                     "identity_reserved",
                                     "decision",
                                     "invocation",
                                     "attempt_admitted",
                                     "attempt_open",
                                     "observation",
                                     "retry",
                                     "identity_conflict",
                                     "complaint",
                                     "rejected_submission",
                                     "adapter_receipt",
                                     "recovery_choice",
                                     "provisional_capture",
                                     "application"};
    if (kind == 0 || kind >= std::size(names))
      throw Error{ErrorCode::corrupt};
    row.object().emplace_back("event", Json{names[kind]});
    rows.push_back(std::move(row));
  }
  return Json::object(
      {{"schema", Json{"blackbird.trajectory.v1"}},
       {"journal", Json{hex_identity(root_.cursor().journal.bytes())}},
       {"cursor", number(cursor)},
       {"end", number(end)},
       {"next", number(next)},
       {"scanned", number(next - cursor)},
       {"caught_up", Json{next == end}},
       {"events", Json{std::move(rows)}},
       {"scope",
        Json{"committed prefix only; recorded order, not inferred causation; "
             "missing settlement is not success; originals via audit_inspect"}}});
}
Json AuditLog::inspect(const Json &q) {
  if (!std::holds_alternative<Json::Object>(q.value()))
    throw Error{ErrorCode::invalid_range};
  const auto fact_count = root_.fact_count();
  const auto end = index(q, "end", fact_count);
  const auto count = index(q, "count", 32), limit = index(q, "limit", 4096);
  if (end > fact_count || count == 0 || count > 64 || limit == 0 || limit > 65536)
    throw Error{ErrorCode::invalid_range};
  if (q.find("record")) {
    const auto i = index(q, "record", 0);
    if (i >= end)
      throw Error{ErrorCode::invalid_range};
    const auto fact = unwrap(root_.fact(i));
    auto result = summary(fact, i);
    std::vector<std::byte> original;
    auto raw = payload(fact.event.body);
    if (const auto *requested = q.find("packet")) {
      const auto *decode = std::get_if<bool>(&requested->value());
      if (!decode)
        throw Error{ErrorCode::invalid_range};
      if (*decode) {
        const auto *application = std::get_if<ApplicationRecordEvent>(&fact.event.body);
        if ((!application && !std::holds_alternative<DecisionEvent>(fact.event.body) &&
             !std::holds_alternative<AttemptObservationEvent>(fact.event.body)) ||
            (application && application->channel == ApplicationChannel::context) ||
            q.find("source") || q.find("diagnostic") || q.find("offset"))
          throw Error{ErrorCode::invalid_range};
        if (raw.size() > limit)
          throw Error{ErrorCode::capacity};
        JsonLimits limits;
        limits.bytes = limit;
        auto packet = unwrap(decode_packet(raw, limits));
        const auto expanded = unwrap(dump_json(packet, limits));
        result.object().emplace_back("end", number(end));
        result.object().emplace_back("total_bytes", number(raw.size()));
        result.object().emplace_back("decoded_bytes", number(expanded.size()));
        result.object().emplace_back("encoding", Json{"decoded-metadata"});
        result.object().emplace_back("packet", std::move(packet));
        return result;
      }
    }
    if (q.find("source")) {
      const auto s = index(q, "source", 0);
      if (s >= fact.event.dependencies.size())
        throw Error{ErrorCode::invalid_range};
      original = unwrap(root_.source(fact.event.dependencies[s]));
      raw = original;
    }
    if (q.find("diagnostic")) {
      const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
      if (!record || record->channel != ApplicationChannel::log)
        throw Error{ErrorCode::invalid_range};
      const auto packet = unwrap(read_packet(record->payload));
      if (string_field(packet, "label") != "provider.stream")
        throw Error{ErrorCode::invalid_range};
      const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count();
      if (seconds < 0)
        throw Error{ErrorCode::invalid_range};
      original = unwrap(root_.read_diagnostic(
          string_field(field(packet, "metadata"), "diagnostic_file"),
          static_cast<std::uint64_t>(seconds)));
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
  if (q.find("packet"))
    throw Error{ErrorCode::invalid_range};
  const auto cursor = std::min(index(q, "cursor", 0), end);
  const auto next = cursor + std::min(count, end - cursor);
  Json::Array rows;
  for (auto i = cursor; i < next; ++i)
    rows.push_back(summary(unwrap(root_.fact(i)), i));
  return Json::object(
      {{"end", number(end)},
       {"next", number(next)},
       {"records", Json{std::move(rows)}},
       {"scope", Json{"committed facts only; end pins append-only prefix; record "
                      "indices stable within this journal lineage"}}});
}
} // namespace blackbird
