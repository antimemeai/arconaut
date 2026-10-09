#include "blackbird/packet.hpp"
#include <algorithm>
#include <charconv>
#include <string_view>

namespace blackbird {
namespace {
// Wire IDs are array index + 1. Never reorder/remove entries in this version.
constexpr std::string_view words[] = {"label",
                                      "metadata",
                                      "time",
                                      "clock_id",
                                      "utc_ns",
                                      "monotonic_ns",
                                      "sampling_span_ns",
                                      "synchronization",
                                      "variable",
                                      "value",
                                      "source",
                                      "sample_id",
                                      "observation_status",
                                      "kind",
                                      "attempt",
                                      "invocation",
                                      "operation",
                                      "revision",
                                      "generation",
                                      "duration_ns",
                                      "input_binding",
                                      "retry_group",
                                      "ordinal",
                                      "field",
                                      "identity_scope",
                                      "content_observation",
                                      "repository",
                                      "requested_path",
                                      "path",
                                      "exit_code",
                                      "commit",
                                      "parents",
                                      "author_unix_seconds",
                                      "committer_unix_seconds",
                                      "host",
                                      "pid",
                                      "scope",
                                      "diagnostic_file",
                                      "expires_at",
                                      "bytes",
                                      "available",
                                      "storage_error",
                                      "complaint",
                                      "activation",
                                      "unknown",
                                      "observed",
                                      "unavailable",
                                      "variable.sample",
                                      "clock.domain",
                                      "doctrine.effective",
                                      "git.head",
                                      "git.commit",
                                      "native_request_field",
                                      "instructions",
                                      "git_cli",
                                      "operation.result",
                                      "provider.request",
                                      "provider.stream",
                                      "process.output",
                                      "invocation-v1",
                                      "attempt-invocation-v1",
    "turn-boundary", "result_id", "result_record"};
enum Tag : unsigned char {
  null_value = 0,
  false_value = 1,
  true_value = 2,
  unsigned_integer = 3,
  negative_integer = 4,
  number_text = 5,
  text = 6,
  identity = 7,
  word = 8,
  array = 9,
  object = 10
};
std::uint64_t word_id(std::string_view s) {
  for (std::size_t i = 0; i < std::size(words); ++i)
    if (words[i] == s)
      return i + 1;
  return 0;
}
int hex_digit(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1;
}
class Encoder {
public:
  explicit Encoder(JsonLimits limits) : limits_(limits) {}
  void byte(unsigned char b) {
    if (out_.size() == limits_.bytes)
      throw Error{ErrorCode::capacity};
    out_.push_back(static_cast<std::byte>(b));
  }
  void varint(std::uint64_t n) {
    do {
      const auto b = static_cast<unsigned char>(n & 127);
      n >>= 7;
      byte(static_cast<unsigned char>(b | (n ? 128 : 0)));
    } while (n);
  }
  void raw_text(std::string_view s) {
    varint(s.size());
    if (s.size() > limits_.bytes - out_.size())
      throw Error{ErrorCode::capacity};
    const auto data = std::as_bytes(std::span{s.data(), s.size()});
    out_.insert(out_.end(), data.begin(), data.end());
  }
  void string(std::string_view s) {
    if (const auto id = word_id(s)) {
      byte(word);
      varint(id);
      return;
    }
    if (s.size() == 32 &&
        std::all_of(s.begin(), s.end(), [](char c) { return hex_digit(c) >= 0; })) {
      byte(identity);
      for (std::size_t i = 0; i < 32; i += 2)
        byte(static_cast<unsigned char>(16 * hex_digit(s[i]) + hex_digit(s[i + 1])));
      return;
    }
    byte(text);
    raw_text(s);
  }
  void value(const Json &v, std::size_t depth) {
    if (depth > limits_.depth || ++nodes_ > limits_.nodes)
      throw Error{ErrorCode::capacity};
    std::visit(
        [&](const auto &x) {
          using T = std::decay_t<decltype(x)>;
          if constexpr (std::is_same_v<T, std::nullptr_t>)
            byte(null_value);
          else if constexpr (std::is_same_v<T, bool>)
            byte(x ? true_value : false_value);
          else if constexpr (std::is_same_v<T, std::string>)
            string(x);
          else if constexpr (std::is_same_v<T, JsonNumber>) {
            const bool neg = x.text.starts_with('-');
            const auto digits = std::string_view{x.text}.substr(neg ? 1 : 0);
            std::uint64_t n = 0;
            const auto r =
                std::from_chars(digits.data(), digits.data() + digits.size(), n);
            if (!digits.empty() && r.ec == std::errc{} &&
                r.ptr == digits.data() + digits.size() && std::to_string(n) == digits &&
                (!neg || n != 0)) {
              byte(neg ? negative_integer : unsigned_integer);
              varint(n);
            } else {
              // Preserve exact lexical form for decimals, exponents and negative zero.
              const auto valid = parse_json(x.text, limits_);
              if (!valid.has_value() ||
                  !std::holds_alternative<JsonNumber>(valid.value().value()) ||
                  valid.value().number().text != x.text)
                throw Error{ErrorCode::invalid_range};
              byte(number_text);
              raw_text(x.text);
            }
          } else if constexpr (std::is_same_v<T, Json::Array>) {
            byte(array);
            varint(x.size());
            for (const auto &child : x)
              value(child, depth + 1);
          } else {
            byte(object);
            varint(x.size());
            for (const auto &[key, child] : x) {
              string(key);
              value(child, depth + 1);
            }
          }
        },
        v.value());
  }
  std::vector<std::byte> finish() { return std::move(out_); }

private:
  JsonLimits limits_;
  std::vector<std::byte> out_;
  std::size_t nodes_ = 0;
};
class Decoder {
public:
  Decoder(ByteView data, JsonLimits limits) : data_(data), limits_(limits) {}
  unsigned char byte() {
    if (at_ == data_.size())
      throw Error{ErrorCode::corrupt};
    return std::to_integer<unsigned char>(data_[at_++]);
  }
  std::uint64_t varint() {
    std::uint64_t n = 0;
    for (unsigned i = 0; i < 10; ++i) {
      const auto b = byte();
      if (i == 9 && b > 1)
        throw Error{ErrorCode::corrupt};
      n |= static_cast<std::uint64_t>(b & 127) << (7 * i);
      if (!(b & 128)) {
        if (i && b == 0)
          throw Error{ErrorCode::corrupt};
        return n;
      }
    }
    throw Error{ErrorCode::corrupt};
  }
  std::string raw_text() {
    const auto n = varint();
    if (n > data_.size() - at_)
      throw Error{ErrorCode::corrupt};
    const auto size = static_cast<std::size_t>(n);
    std::string s{reinterpret_cast<const char *>(data_.data() + at_), size};
    at_ += size;
    return s;
  }
  std::string string(unsigned char tag) {
    if (tag == text)
      return raw_text();
    if (tag == word) {
      const auto n = varint();
      if (!n || n > std::size(words))
        throw Error{ErrorCode::corrupt};
      return std::string{words[n - 1]};
    }
    if (tag == identity) {
      constexpr char hex[] = "0123456789abcdef";
      std::string s;
      s.reserve(32);
      for (unsigned i = 0; i < 16; ++i) {
        const auto b = byte();
        s += hex[b >> 4];
        s += hex[b & 15];
      }
      return s;
    }
    throw Error{ErrorCode::corrupt};
  }
  Json value(std::size_t depth) {
    if (depth > limits_.depth || ++nodes_ > limits_.nodes)
      throw Error{ErrorCode::capacity};
    const auto tag = byte();
    switch (tag) {
    case null_value:
      return Json{};
    case false_value:
      return Json{false};
    case true_value:
      return Json{true};
    case unsigned_integer:
      return Json{JsonNumber{std::to_string(varint())}};
    case negative_integer: {
      const auto n = varint();
      if (!n)
        throw Error{ErrorCode::corrupt};
      return Json{JsonNumber{"-" + std::to_string(n)}};
    }
    case number_text: {
      const auto s = raw_text();
      const auto valid = parse_json(s, limits_);
      if (!valid.has_value() ||
          !std::holds_alternative<JsonNumber>(valid.value().value()) ||
          valid.value().number().text != s)
        throw Error{ErrorCode::corrupt};
      return Json{JsonNumber{s}};
    }
    case text:
    case word:
    case identity:
      return Json{string(tag)};
    case array:
    case object: {
      const auto count = varint();
      // Every child consumes a node and at least one byte (keys consume another).
      if (count > limits_.nodes - nodes_)
        throw Error{ErrorCode::capacity};
      if (count > (data_.size() - at_) / (tag == object ? 2 : 1))
        throw Error{ErrorCode::corrupt};
      if (tag == array) {
        Json::Array out;
        out.reserve(static_cast<std::size_t>(count));
        for (std::uint64_t i = 0; i < count; ++i)
          out.push_back(value(depth + 1));
        return Json{std::move(out)};
      }
      Json::Object out;
      out.reserve(static_cast<std::size_t>(count));
      for (std::uint64_t i = 0; i < count; ++i) {
        auto key = string(byte());
        out.emplace_back(std::move(key), value(depth + 1));
      }
      return Json::object(std::move(out));
    }
    default:
      throw Error{ErrorCode::corrupt};
    }
  }
  bool complete() const { return at_ == data_.size(); }

private:
  ByteView data_;
  JsonLimits limits_;
  std::size_t at_ = 0, nodes_ = 0;
};
} // namespace
Result<std::vector<std::byte>> encode_packet(const Json &value, JsonLimits limits) {
  try {
    Encoder e{limits};
    for (const auto b : {'B', 'B', 'M', '\1'})
      e.byte(static_cast<unsigned char>(b));
    e.value(value, 0);
    return Result<std::vector<std::byte>>::success(e.finish());
  } catch (const Error &e) {
    return Result<std::vector<std::byte>>::failure(e);
  } catch (const std::bad_alloc &) {
    return Result<std::vector<std::byte>>::failure({ErrorCode::allocation});
  }
}
Result<Json> decode_packet(ByteView bytes, JsonLimits limits) {
  try {
    if (bytes.size() > limits.bytes)
      return Result<Json>::failure({ErrorCode::capacity});
    if (bytes.size() >= 3 && bytes[0] == std::byte{'B'} && bytes[1] == std::byte{'B'} &&
        bytes[2] == std::byte{'M'}) {
      if (bytes.size() < 4)
        return Result<Json>::failure({ErrorCode::corrupt});
      if (bytes[3] != std::byte{1})
        return Result<Json>::failure({ErrorCode::unsupported});
      Decoder d{bytes.subspan(4), limits};
      auto value = d.value(0);
      if (!d.complete())
        return Result<Json>::failure({ErrorCode::corrupt});
      return Result<Json>::success(std::move(value));
    }
    return parse_json(
        std::string_view{reinterpret_cast<const char *>(bytes.data()), bytes.size()},
        limits);
  } catch (const Error &e) {
    return Result<Json>::failure(e);
  } catch (const std::bad_alloc &) {
    return Result<Json>::failure({ErrorCode::allocation});
  }
}
} // namespace blackbird
