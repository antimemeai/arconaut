#include "blackbird/context.hpp"
#include <filesystem>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
namespace {
void check(bool good, const char *reason) {
  if (!good)
    throw std::runtime_error(reason);
}
std::vector<std::byte> bytes(std::string_view s) {
  const auto view = std::as_bytes(std::span{s.data(), s.size()});
  return {view.begin(), view.end()};
}
std::vector<std::byte> wire(std::initializer_list<unsigned char> body) {
  auto out = bytes("BBM\1");
  for (const auto b : body)
    out.push_back(static_cast<std::byte>(b));
  return out;
}
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = static_cast<std::byte>(n);
  return unwrap(T::from_bytes(b));
}
void codec() {
  check(unwrap(encode_packet(Json{})) == wire({0}), "null layout");
  check(unwrap(encode_packet(Json{JsonNumber{"300"}})) == wire({3, 0xac, 2}),
        "integer varint layout");
  check(unwrap(encode_packet(Json{JsonNumber{"-300"}})) == wire({4, 0xac, 2}),
        "negative layout");
  check(unwrap(encode_packet(Json::object({{"label", Json{"variable.sample"}}}))) ==
            wire({10, 1, 8, 1, 8, 48}),
        "dictionary IDs changed");
  const auto identity = std::string{"0102030405060708090a0b0c0d0e0f10"};
  auto expected = wire({7});
  for (unsigned char n = 1; n <= 16; ++n)
    expected.push_back(static_cast<std::byte>(n));
  check(unwrap(encode_packet(Json{identity})) == expected, "identity persisted as hex");
  const Json all = Json::object(
      {{"arbitrary-key",
        Json{Json::Array{
            Json{}, Json{false}, Json{true}, Json{JsonNumber{"0"}},
            Json{JsonNumber{"-0"}}, Json{JsonNumber{"0.500"}},
            Json{JsonNumber{"1e+23"}}, Json{JsonNumber{"18446744073709551615"}},
            Json{JsonNumber{"-18446744073709551615"}},
            Json{JsonNumber{"18446744073709551616"}}, Json{std::string{"a\0b", 3}},
            Json{"é\n"}, Json{identity}, Json{Json::Array{}}, Json::object({})}}}});
  const auto encoded = unwrap(encode_packet(all));
  check(unwrap(decode_packet(encoded)) == all,
        "tagged values changed exact representation");
  const auto legacy = bytes(unwrap(dump_json(all)));
  check(unwrap(decode_packet(legacy)) == all, "legacy JSON unreadable");
  for (std::size_t size = 0; size < encoded.size(); ++size)
    check(!decode_packet(ByteView{encoded}.first(size)).has_value(),
          "truncated packet admitted");
  auto trailing = encoded;
  trailing.push_back(std::byte{0});
  check(!decode_packet(trailing).has_value(), "trailing bytes admitted");
  auto version = encoded;
  version[3] = std::byte{2};
  check(decode_packet(version).error().code == ErrorCode::unsupported,
        "unknown version reinterpreted");
  for (const auto &bad :
       {wire({255}), wire({8, 0}), wire({8, 127}), wire({3, 0x80, 0}),
        wire({3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 2}),
        wire({4, 0}), wire({9, 0xff, 0xff, 0xff, 0xff, 0x0f}), wire({10, 1, 0, 0}),
        wire({5, 1, 'x'}), wire({6, 99, 'x'})})
    check(!decode_packet(bad).has_value(), "malformed packet admitted");
  JsonLimits small;
  small.bytes = encoded.size() - 1;
  check(!encode_packet(all, small).has_value() &&
            !decode_packet(encoded, small).has_value(),
        "byte bound ignored");
  small = {};
  small.nodes = 1;
  check(!encode_packet(all, small).has_value() &&
            !decode_packet(encoded, small).has_value(),
        "node bound ignored");
  small = {};
  small.depth = 0;
  check(!encode_packet(all, small).has_value() &&
            !decode_packet(encoded, small).has_value(),
        "depth bound ignored");
  check(!encode_packet(Json{JsonNumber{"01"}}).has_value(), "invalid number written");
  const auto sample = Json::object(
      {{"label", Json{"variable.sample"}},
       {"sample_id", Json{identity}},
       {"variable", Json{"doctrine.effective"}},
       {"value", Json::object({{"content_observation", Json{identity}}})},
       {"source", Json::object({{"kind", Json{"native_request_field"}},
                                {"invocation", Json{identity}},
                                {"attempt", Json{identity}},
                                {"field", Json{"instructions"}}})},
       {"observation_status", Json{"observed"}},
       {"time", Json::object({{"clock_id", Json{identity}},
                              {"utc_ns", Json{JsonNumber{"1791518400000000000"}}},
                              {"monotonic_ns", Json{JsonNumber{"123456789000"}}},
                              {"sampling_span_ns", Json{JsonNumber{"50"}}},
                              {"synchronization", Json{"unknown"}}})}});
  const auto json_size = unwrap(dump_json(sample)).size();
  const auto binary_size = unwrap(encode_packet(sample)).size();
  check(binary_size * 2 < json_size, "metadata no longer substantially smaller");
  check(unwrap(decode_packet(unwrap(encode_packet(sample)))) == sample,
        "size fixture changed values");
  std::cout << "Doctrine metadata fixture: JSON=" << json_size
            << " binary=" << binary_size << " bytes\n";
}
void mixed_history(const char *path) {
  const JournalHeader h{id<EnvironmentId>(1),
                        id<AuditStreamId>(2),
                        3,
                        {1024 * 1024, 4 * 1024 * 1024},
                        std::nullopt};
  const JournalCapacity cap{8 * 1024 * 1024, 10000};
  const auto old = Json::object({{"label", Json{"variable.sample"}},
                                 {"variable", Json{"experiment.old"}},
                                 {"value", Json{JsonNumber{"17"}}}});
  std::size_t record = 0, end = 0;
  {
    auto root =
        unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                         unwrap(NativeJournalDirectory::open(path))),
                                     "audit", h, cap));
    AuditLog log{*root};
    const auto app = log.issue();
    unwrap(root->submit({{},
                         ApplicationRecordEvent{app, ApplicationChannel::log,
                                                bytes(unwrap(dump_json(old)))}}));
    const auto next = Json::object({{"label", Json{"variable.sample"}},
                                    {"variable", Json{"experiment.new"}},
                                    {"value", Json{JsonNumber{"19"}}}});
    log.record(ApplicationChannel::log, next);
    const auto original = log.issue();
    record = root->fact_count();
    const auto cursor = root->cursor();
    const auto source = bytes(std::string_view{"EXACT ORIGINAL \0 BYTES", 22});
    const ByteView view{source};
    const auto metadata = Json::object(
        {{"label", Json{"fixture.original"}}, {"metadata", Json::object({})}});
    RetainedEvent event{{SourceReference{cursor.journal, cursor.sequence + 1}},
                        ApplicationRecordEvent{original, ApplicationChannel::log,
                                               unwrap(encode_packet(metadata))}};
    unwrap(root->append(cursor, std::span{&view, 1}, std::span{&event, 1}));
    end = root->fact_count();
  }
  auto root =
      unwrap(RetainedState::open(std::make_unique<NativeJournalDirectory>(
                                     unwrap(NativeJournalDirectory::open(path))),
                                 "audit", h, cap));
  unwrap(root->confirm_recovery());
  AuditLog log{*root};
  const auto page =
      log.trajectory(Json::object({{"variables", Json{true}},
                                   {"cursor", Json{JsonNumber{"0"}}},
                                   {"end", Json{JsonNumber{std::to_string(end)}}},
                                   {"scan", Json{JsonNumber{"256"}}}}));
  const auto &events = field(page, "events").array();
  check(events.size() == 2 && string_field(events[0], "variable") == "experiment.old" &&
            field(events[0], "value") == Json{JsonNumber{"17"}} &&
            field(events[1], "value") == Json{JsonNumber{"19"}},
        "mixed-format history changed on reopen");
  const auto original_fact = unwrap(root->fact(record));
  const auto decoded = log.inspect(Json::object(
      {{"record", Json{JsonNumber{std::to_string(record)}}}, {"packet", Json{true}}}));
  check(string_field(field(decoded, "packet"), "label") == "fixture.original" &&
            string_field(decoded, "encoding") == "decoded-metadata",
        "model inspection requires binary decoding");
  bool bounded = false;
  try {
    (void)log.inspect(
        Json::object({{"record", Json{JsonNumber{std::to_string(record)}}},
                      {"packet", Json{true}},
                      {"limit", Json{JsonNumber{"8"}}}}));
  } catch (const Error &e) {
    bounded = e.code == ErrorCode::capacity;
  }
  check(bounded, "decoded packet exceeded inspection bound");
  const auto original = unwrap(root->source(original_fact.event.dependencies[0]));
  check(original == bytes(std::string_view{"EXACT ORIGINAL \0 BYTES", 22}),
        "source bytes changed");
}
} // namespace
int main() {
  char path[] = "/tmp/blackbird-packet-XXXXXX";
  if (!mkdtemp(path))
    return 2;
  int result = 0;
  try {
    codec();
    mixed_history(path);
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
    result = 1;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    result = 1;
  }
  std::filesystem::remove_all(path);
  return result;
}
