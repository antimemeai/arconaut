#include "arconaut/json.hpp"
#include <iostream>
#include <stdexcept>
using namespace arconaut;
void check(bool value) {
  if (!value)
    throw std::runtime_error("JSON contract");
}
int main() {
  try {
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
