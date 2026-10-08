#include "blackbird/json.hpp"
#include <iostream>
#include <stdexcept>
using namespace blackbird;
void check(bool value) {
  if (!value)
    throw std::runtime_error("JSON contract");
}
int main() {
  try {
    auto original =
        Json::object({{"nested", Json{Json::Array{Json{std::string(65536, 'x')}}}}});
    auto shared = original;
    check(original.find("nested")->array()[0].string().data() ==
          shared.find("nested")->array()[0].string().data());
    auto &borrow = original.object()[0].second.array();
    auto pinned = original;
    borrow[0] = Json{"changed"};
    check(pinned.find("nested")->array()[0].string().size() == 65536);
    check(shared.find("nested")->array()[0].string().size() == 65536);
    auto &nested_borrow = pinned.object()[0].second.array();
    auto copied_after_borrow = pinned;
    nested_borrow.push_back(Json{"new"});
    check(copied_after_borrow.find("nested")->array().size() == 1);
    auto detached = shared;
    detached.object().emplace_back("another", Json{true});
    check(!shared.find("another"));
    constexpr std::array<std::string_view, 1> omitted{"candidate"};
    for (const auto input : {R"({"candidate":{"candidate":"nested","a":[1,true,null,"\uD83D\uDE80"]},"keep":{"candidate":"yes"}})",
                             R"({"candidate":"plain UTF-8 🚀","keep":42})",
                             R"({"candidate":"escaped\ntext","keep":false})"}) {
      auto full = parse_json(input).value();
      auto projection = parse_json_projection(input, omitted).value();
      for (auto &[key, value] : full.object())
        if (key == "candidate") value = Json{};
      check(full == projection);
    }
    for (const auto input : {R"({"candidate":{"a":1,"a":2}})",
                             R"({"candidate":1,"cand\u0069date":2})",
                             R"({"candidate":[1,]})", R"({"candidate":"\uD800"})",
                             R"({"candidate":01})", R"({"candidate":"unterminated})"}) {
      check(!parse_json_projection(input, omitted).has_value());
    }
    for (const auto bounds : {JsonLimits{8, 64, 16}, JsonLimits{64, 2, 16}, JsonLimits{64, 64, 1}}) {
      const auto input = R"({"candidate":[[0,1]]})";
      auto full = parse_json(input, bounds);
      auto projection = parse_json_projection(input, omitted, bounds);
      check(!full.has_value() && !projection.has_value() && full.error() == projection.error());
    }
    for (const auto &invalid : {std::string{"\xc0\xaf", 2}, std::string{"\xed\xa0\x80", 3}, std::string{"\n", 1}})
      check(!parse_json_projection("{\"candidate\":\"" + invalid + "\"}", omitted).has_value());
    for (std::size_t n = 0; n < 65; ++n) {
      const auto prefix = std::string(n, 'x');
      for (const auto &tail : {std::string{""}, std::string{"🚀"}, std::string{"\\nend"}}) {
        const auto input = "{\"candidate\":\"" + prefix + tail + "\",\"keep\":1}";
        auto full = parse_json(input).value();
        for (auto &[key, value] : full.object()) if (key == "candidate") value = Json{};
        check(parse_json_projection(input, omitted).value() == full);
      }
      for (const auto &tail : {std::string{"\xc0\xaf", 2}, std::string{"\xed\xa0\x80", 3}, std::string{"\n", 1}})
        check(!parse_json_projection("{\"candidate\":\"" + prefix + tail + "\"}", omitted).has_value());
    }
    const auto parsed = parse_json(
        R"({"big":9007199254740993,"opaque":{"encrypted_content":"AB=="},"text":"\uD83D\uDE80\u0000","a":[true,null,-1.20e+3]})");
    check(parsed.has_value());
    const auto &value = parsed.value();
    check(value.find("big")->number().text == "9007199254740993");
    check(value.find("opaque")->find("encrypted_content")->string() == "AB==");
    check(value.find("text")->string() == std::string("\xF0\x9F\x9A\x80\0", 5));
    check(value.find("a")->array()[2].number().text == "-1.20e+3");
    check(
        dump_json(value).value() ==
        R"({"big":9007199254740993,"opaque":{"encrypted_content":"AB=="},"text":"🚀\u0000","a":[true,null,-1.20e+3]})");
    for (const auto input :
         {"[1,]", "{\"a\":1,\"a\":2}", "01", "1.", "1e", "true false", "\"\\uD800\"",
          "\"\\uDC00\"", "\"\\q\"", "{", "["})
      check(!parse_json(input).has_value());
    check(!parse_json(std::string("\"\xC0\xAF\"", 4)).has_value());
    check(!parse_json(std::string("\"\n\"", 3)).has_value());
    check(!parse_json("[[0]]", {64, 64, 1}).has_value());
    check(!parse_json("[0,1]", {64, 2, 16}).has_value());
    check(!parse_json("[0,1]", {4, 64, 16}).has_value());
    check(!dump_json(Json{JsonNumber{"01"}}).has_value());
    check(
        !dump_json(Json::object({{"a", Json{true}}, {"a", Json{false}}})).has_value());
    check(!dump_json(value, {16, 64, 16}).has_value());
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
