#include "blackbird/coding.hpp"
#include "blackbird/json.hpp"
#include "blackbird/packet.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <unistd.h>
using namespace blackbird;
namespace {
void check(bool ok, const char *reason) {
  if (!ok)
    throw std::runtime_error(reason);
}
template <class T> T id(unsigned char n) {
  IdentityBytes bytes{};
  bytes[0] = static_cast<std::byte>(n);
  return unwrap(T::from_bytes(bytes));
}
struct NoProvider final : CodingProvider {
  Value respond(const Value &, const std::function<void(std::string_view)> &) override {
    throw std::runtime_error("unexpected provider request");
  }
};
} // namespace
int main() {
  char directory[] = "/tmp/blackbird-native-value-XXXXXX";
  if (!mkdtemp(directory))
    return 2;
  try {
    const auto decimal = Number{Decimal{false, -3, {125}}};
    check(decimal.text() == "0.125", "decimal presentation");
    const auto encoded = unwrap(encode_packet(Value{decimal}));
    const std::vector<std::byte> expected{
        std::byte{'B'}, std::byte{'B'}, std::byte{'M'}, std::byte{2},
        std::byte{5},   std::byte{0},   std::byte{5},   std::byte{1},
        std::byte{125}, std::byte{0},   std::byte{0},   std::byte{0}};
    check(encoded == expected, "decimal coefficient/exponent wire layout");
    const Value floating{Number{0.5}};
    check(unwrap(encode_packet(floating)) ==
              std::vector<std::byte>{
                  std::byte{'B'}, std::byte{'B'}, std::byte{'M'}, std::byte{2},
                  std::byte{11}, std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
                  std::byte{0}, std::byte{0}, std::byte{224}, std::byte{63}},
          "binary64 layout");
    const auto values = Value::object({{"minimum", Value{Number{INT64_MIN}}},
                                       {"maximum", Value{Number{UINT64_MAX}}},
                                       {"decimal", Value{decimal}},
                                       {"floating", floating},
                                       {"opaque", Value{std::string{"a\xff\0b", 4}}},
                                       {"empty", Value{Value::Array{}}},
                                       {"null", Value{}}});
    const auto decoded = unwrap(decode_packet(unwrap(encode_packet(values))));
    check(decoded == values, "native binary roundtrip");
    check(std::holds_alternative<double>(field(decoded, "floating").number().storage()),
          "real lost its native type");
    check(std::holds_alternative<Decimal>(field(decoded, "decimal").number().storage()),
          "decimal lost its native type");
    constexpr std::array<std::string_view, 1> omitted{"opaque"};
    const auto projected =
        unwrap(decode_packet_projection(unwrap(encode_packet(values)), omitted));
    check(field(projected, "opaque") == Value{} &&
              field(projected, "maximum") == field(values, "maximum"),
          "binary projection");
    check(valid_utf8(unwrap(format_value(values))),
          "binary native presentation is not UTF-8");
    check(unwrap(parse_number("1e-9223372036854775808")).text() ==
              "1e-9223372036854775808",
          "minimum native decimal exponent");
    bool rejected = false;
    try {
      (void)Number{std::numeric_limits<double>::infinity()};
    } catch (const Error &) {
      rejected = true;
    }
    check(rejected, "nonfinite native number admitted");
    auto root =
        unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(unwrap(
                                         NativeJournalDirectory::open(directory))),
                                     "audit",
                                     {id<EnvironmentId>(1),
                                      id<AuditStreamId>(2),
                                      3,
                                      {1024 * 1024, 4 * 1024 * 1024},
                                      std::nullopt},
                                     {16 * 1024 * 1024, 10000}));
    AuditLog log{*root};
    ContextStore context{log};
    NoProvider provider;
    CodingEngine engine{log, context, provider, "test"};
    const auto definition = Value::object(
        {{"name", Value{"native_echo"}},
         {"description", Value{"Return native arguments unchanged"}},
         {"parameters",
          Value::object(
              {{"type", Value{"object"}},
               {"properties",
                Value::object({{"minimum", Value::object({{"type", Value{"integer"}}})},
                               {"maximum", Value::object({{"type", Value{"integer"}}})},
                               {"decimal", Value::object({{"type", Value{"number"}}})},
                               {"floating", Value::object({{"type", Value{"number"}}})},
                               {"opaque", Value::object({{"type", Value{"string"}}})},
                               {"null", Value::object({{"type", Value{"null"}}})}})},
               {"required", Value{Value::Array{}}},
               {"additionalProperties", Value{false}}})},
         {"source", Value{"return args"}}});
    check(field(engine.operator_call("tool_define",
                                     Value::object({{"definition", definition}})),
                "staged") == Value{true},
          "native tool registration");
    auto arguments = values;
    std::erase_if(arguments.object(),
                  [](const auto &entry) { return entry.first == "empty"; });
    const auto result = engine.operator_call("native_echo", arguments);
    check(result.object().size() == arguments.object().size(), "native Lua map size");
    for (const auto &[key, value] : arguments.object())
      check(field(result, key) == value, "operator/native Lua transfer lost data");
    check(std::holds_alternative<double>(field(result, "floating").number().storage()),
          "Lua floating point became decimal text");
    engine.turn({"", "assert(_native_args==nil); local v={opaque='x\\255\\000'}; "
                     "assert(blackbird.binary.decode(blackbird.binary.encode(v))."
                     "opaque==v.opaque)"});
    const auto binary_path = std::filesystem::path{directory} / "values.bbm";
    write_file(binary_path, unwrap(encode_packet_string(values)));
    engine.turn({"", "local r=blackbird.call('read_file',{path='" +
                         binary_path.string() +
                         "'}); local v=blackbird.binary.decode(r.content); "
                         "assert(v.opaque=='a\\255\\000b' and #v.empty==0)"});
    check(unwrap(dump_json(Value{std::string{"\xff", 1}})) ==
              "{\"encoding\":\"hex\",\"bytes\":\"ff\"}",
          "binary external JSON presentation");
    const auto item = Value::object({{"type", Value{"function_call_output"}},
                                     {"call_id", Value{"x"}},
                                     {"output", Value::object({{"ok", Value{true}}})}});
    const auto request =
        export_provider_request(Value::object({{"input", Value{Value::Array{item}}}}));
    check(string_field(field(request, "input").array()[0], "output") == "{\"ok\":true}",
          "provider output wire adapter");
    const auto response = import_provider_response(unwrap(parse_json(
        R"({"output":[{"type":"function_call","arguments":"{\"n\":18446744073709551615}"}]})")));
    check(field(field(field(response, "output").array()[0], "arguments"), "n") ==
              Value{Number{UINT64_MAX}},
          "provider argument wire adapter");
    std::filesystem::remove_all(directory);
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  } catch (const Error &e) {
    std::cerr << error_name(e.code) << '\n';
  }
  std::filesystem::remove_all(directory);
  return 1;
}
