#include "blackbird/context.hpp"
#include "blackbird/json.hpp"
#include "blackbird/packet.hpp"
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
  auto out = bytes("BBM\2");
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
  check(unwrap(encode_packet(Value{})) == wire({0}), "null layout");
  check(unwrap(encode_packet(Value{Number{"300"}})) == wire({3, 0xac, 2}),
        "integer varint layout");
  check(unwrap(encode_packet(Value{Number{"-300"}})) == wire({4, 0xac, 2}),
        "negative layout");
  check(unwrap(encode_packet(Value::object({{"label", Value{"variable.sample"}}}))) ==
            wire({10, 1, 8, 1, 8, 48}),
        "dictionary IDs changed");
  const auto identity = std::string{"0102030405060708090a0b0c0d0e0f10"};
  auto expected = wire({7});
  for (unsigned char n = 1; n <= 16; ++n)
    expected.push_back(static_cast<std::byte>(n));
  check(unwrap(encode_packet(Value{identity})) == expected,
        "identity persisted as hex");
  const Value all =
      Value::object({{"arbitrary-key",
                      Value{Value::Array{
                          Value{}, Value{false}, Value{true}, Value{Number{"0"}},
                          Value{Number{"-0"}}, Value{Number{"0.500"}},
                          Value{Number{"1e+23"}}, Value{Number{"18446744073709551615"}},
                          Value{Number{"-18446744073709551615"}},
                          Value{Number{"18446744073709551616"}},
                          Value{std::string{"a\0b", 3}}, Value{"é\n"}, Value{identity},
                          Value{Value::Array{}}, Value::object({})}}}});
  const auto encoded = unwrap(encode_packet(all));
  check(unwrap(decode_packet(encoded)) == all,
        "tagged values changed exact representation");
  const auto legacy = bytes(unwrap(dump_json(all)));
  check(!decode_packet(legacy).has_value(), "JSON admitted as native packet");
  for (std::size_t size = 0; size < encoded.size(); ++size)
    check(!decode_packet(ByteView{encoded}.first(size)).has_value(),
          "truncated packet admitted");
  auto trailing = encoded;
  trailing.push_back(std::byte{0});
  check(!decode_packet(trailing).has_value(), "trailing bytes admitted");
  auto version = encoded;
  version[3] = std::byte{3};
  check(decode_packet(version).error().code == ErrorCode::unsupported,
        "unknown version reinterpreted");
  for (const auto &bad :
       {wire({255}), wire({8, 0}), wire({8, 127}), wire({3, 0x80, 0}),
        wire({3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 2}),
        wire({4, 0}), wire({9, 0xff, 0xff, 0xff, 0xff, 0x0f}), wire({10, 1, 0, 0}),
        wire({5, 1, 'x'}), wire({6, 99, 'x'})})
    check(!decode_packet(bad).has_value(), "malformed packet admitted");
  ValueLimits small;
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
  check(!parse_number("01").has_value(), "invalid number admitted");
  const auto sample = Value::object(
      {{"label", Value{"variable.sample"}},
       {"sample_id", Value{identity}},
       {"variable", Value{"doctrine.effective"}},
       {"value", Value::object({{"content_observation", Value{identity}}})},
       {"source", Value::object({{"kind", Value{"native_request_field"}},
                                 {"invocation", Value{identity}},
                                 {"attempt", Value{identity}},
                                 {"field", Value{"instructions"}}})},
       {"observation_status", Value{"observed"}},
       {"time", Value::object({{"clock_id", Value{identity}},
                               {"utc_ns", Value{Number{"1791518400000000000"}}},
                               {"monotonic_ns", Value{Number{"123456789000"}}},
                               {"sampling_span_ns", Value{Number{"50"}}},
                               {"synchronization", Value{"unknown"}}})}});
  const auto json_size = unwrap(dump_json(sample)).size();
  const auto binary_size = unwrap(encode_packet(sample)).size();
  check(binary_size * 2 < json_size, "metadata no longer substantially smaller");
  check(unwrap(decode_packet(unwrap(encode_packet(sample)))) == sample,
        "size fixture changed values");
  std::cout << "Doctrine metadata fixture: JSON=" << json_size
            << " binary=" << binary_size << " bytes\n";
}
void native_history(const char *path) {
  const JournalHeader h{id<EnvironmentId>(1),
                        id<AuditStreamId>(2),
                        3,
                        {1024 * 1024, 4 * 1024 * 1024},
                        std::nullopt};
  const JournalCapacity cap{8 * 1024 * 1024, 10000};
  const auto old = Value::object({{"label", Value{"variable.sample"}},
                                  {"variable", Value{"experiment.old"}},
                                  {"value", Value{Number{"17"}}}});
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
                                                unwrap(encode_packet(old))}}));
    const auto next = Value::object({{"label", Value{"variable.sample"}},
                                     {"variable", Value{"experiment.new"}},
                                     {"value", Value{Number{"19"}}}});
    log.record(ApplicationChannel::log, next);
    const auto original = log.issue();
    record = root->fact_count();
    const auto cursor = root->cursor();
    const auto source = bytes(std::string_view{"EXACT ORIGINAL \0 BYTES", 22});
    const ByteView view{source};
    const auto metadata = Value::object(
        {{"label", Value{"fixture.original"}}, {"metadata", Value::object({})}});
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
      log.trajectory(Value::object({{"variables", Value{true}},
                                    {"cursor", Value{Number{"0"}}},
                                    {"end", Value{Number{std::to_string(end)}}},
                                    {"scan", Value{Number{"256"}}}}));
  const auto &events = field(page, "events").array();
  check(events.size() == 2 && string_field(events[0], "variable") == "experiment.old" &&
            field(events[0], "value") == Value{Number{"17"}} &&
            field(events[1], "value") == Value{Number{"19"}},
        "native history changed on reopen");
  const auto original_fact = unwrap(root->fact(record));
  const auto decoded = log.inspect(Value::object(
      {{"record", Value{Number{std::to_string(record)}}}, {"packet", Value{true}}}));
  check(string_field(field(decoded, "packet"), "label") == "fixture.original" &&
            string_field(decoded, "encoding") == "decoded-metadata",
        "model inspection requires binary decoding");
  bool bounded = false;
  try {
    (void)log.inspect(Value::object({{"record", Value{Number{std::to_string(record)}}},
                                     {"packet", Value{true}},
                                     {"limit", Value{Number{"8"}}}}));
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
    native_history(path);
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
