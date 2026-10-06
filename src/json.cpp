#include "arconaut/json.hpp"
#include <stdexcept>

namespace arconaut {
const Json *Json::find(std::string_view key) const noexcept {
  const auto *fields = std::get_if<Object>(&data_);
  if (fields != nullptr)
    for (const auto &[name, value] : *fields)
      if (name == key)
        return &value;
  return nullptr;
}
namespace {
[[noreturn]] void bad() { throw Error{ErrorCode::corrupt}; }
[[noreturn]] void full() { throw Error{ErrorCode::capacity}; }
bool digit(char c) { return c >= '0' && c <= '9'; }
// Strict scalar-value UTF-8, rejecting overlong forms and surrogate encodings.
bool utf8(std::string_view s) {
  for (std::size_t i = 0; i < s.size();) {
    const auto first = static_cast<unsigned char>(s[i++]);
    if (first < 128)
      continue;
    unsigned count = 0, scalar = 0, minimum = 0;
    if (first >= 0xc2 && first <= 0xdf) {
      count = 1;
      scalar = first & 31U;
      minimum = 128;
    } else if (first >= 0xe0 && first <= 0xef) {
      count = 2;
      scalar = first & 15U;
      minimum = 2048;
    } else if (first >= 0xf0 && first <= 0xf4) {
      count = 3;
      scalar = first & 7U;
      minimum = 65536;
    } else
      return false;
    if (s.size() - i < count)
      return false;
    for (unsigned j = 0; j < count; ++j) {
      const auto next = static_cast<unsigned char>(s[i++]);
      if ((next & 0xc0U) != 0x80U)
        return false;
      scalar = (scalar << 6U) | (next & 63U);
    }
    if (scalar < minimum || scalar > 0x10ffff || (scalar >= 0xd800 && scalar <= 0xdfff))
      return false;
  }
  return true;
}
void scalar_utf8(std::string &out, unsigned scalar) {
  if (scalar < 128)
    out.push_back(static_cast<char>(scalar));
  else if (scalar < 2048) {
    out.push_back(static_cast<char>(0xc0U | (scalar >> 6U)));
    out.push_back(static_cast<char>(0x80U | (scalar & 63U)));
  } else if (scalar < 65536) {
    out.push_back(static_cast<char>(0xe0U | (scalar >> 12U)));
    out.push_back(static_cast<char>(0x80U | ((scalar >> 6U) & 63U)));
    out.push_back(static_cast<char>(0x80U | (scalar & 63U)));
  } else {
    out.push_back(static_cast<char>(0xf0U | (scalar >> 18U)));
    out.push_back(static_cast<char>(0x80U | ((scalar >> 12U) & 63U)));
    out.push_back(static_cast<char>(0x80U | ((scalar >> 6U) & 63U)));
    out.push_back(static_cast<char>(0x80U | (scalar & 63U)));
  }
}
class Parser {
public:
  Parser(std::string_view input, JsonLimits bounds) : text(input), limits(bounds) {}
  Json run() {
    if (text.size() > limits.bytes)
      full();
    auto result = item(0);
    space();
    if (pos != text.size())
      bad();
    return result;
  }

private:
  std::string_view text;
  JsonLimits limits;
  std::size_t pos = 0, nodes = 0;
  void space() {
    while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\n' ||
                                 text[pos] == '\r' || text[pos] == '\t'))
      ++pos;
  }
  char take() {
    if (pos == text.size())
      bad();
    return text[pos++];
  }
  bool eat(char c) {
    if (pos < text.size() && text[pos] == c) {
      ++pos;
      return true;
    }
    return false;
  }
  unsigned hex() {
    unsigned value = 0;
    for (unsigned i = 0; i < 4; ++i) {
      const char c = take();
      unsigned d = 0;
      if (digit(c))
        d = static_cast<unsigned>(c - '0');
      else if (c >= 'a' && c <= 'f')
        d = static_cast<unsigned>(c - 'a') + 10U;
      else if (c >= 'A' && c <= 'F')
        d = static_cast<unsigned>(c - 'A') + 10U;
      else
        bad();
      value = (value << 4U) | d;
    }
    return value;
  }
  std::string string() {
    if (take() != '"')
      bad();
    std::string out;
    while (true) {
      const auto c = take();
      if (c == '"')
        break;
      if (static_cast<unsigned char>(c) < 32)
        bad();
      if (c != '\\') {
        out.push_back(c);
        continue;
      }
      switch (take()) {
      case '"':
        out.push_back('"');
        break;
      case '\\':
        out.push_back('\\');
        break;
      case '/':
        out.push_back('/');
        break;
      case 'b':
        out.push_back('\b');
        break;
      case 'f':
        out.push_back('\f');
        break;
      case 'n':
        out.push_back('\n');
        break;
      case 'r':
        out.push_back('\r');
        break;
      case 't':
        out.push_back('\t');
        break;
      case 'u': {
        auto scalar = hex();
        if (scalar >= 0xd800 && scalar <= 0xdbff) {
          if (take() != '\\' || take() != 'u')
            bad();
          const auto low = hex();
          if (low < 0xdc00 || low > 0xdfff)
            bad();
          scalar = 0x10000U + ((scalar - 0xd800U) << 10U) + low - 0xdc00U;
        } else if (scalar >= 0xdc00 && scalar <= 0xdfff)
          bad();
        scalar_utf8(out, scalar);
        break;
      }
      default:
        bad();
      }
    }
    if (!utf8(out))
      bad();
    return out;
  }
  Json number() {
    const auto start = pos;
    (void)eat('-');
    if (!eat('0')) {
      if (pos == text.size() || text[pos] < '1' || text[pos] > '9')
        bad();
      while (pos < text.size() && digit(text[pos]))
        ++pos;
    }
    if (eat('.')) {
      const auto before = pos;
      while (pos < text.size() && digit(text[pos]))
        ++pos;
      if (before == pos)
        bad();
    }
    if (eat('e') || eat('E')) {
      if (!eat('+'))
        (void)eat('-');
      const auto before = pos;
      while (pos < text.size() && digit(text[pos]))
        ++pos;
      if (before == pos)
        bad();
    }
    return Json{JsonNumber{std::string{text.substr(start, pos - start)}}};
  }
  Json item(std::size_t depth) {
    if (depth > limits.depth || nodes == limits.nodes)
      full();
    ++nodes;
    space();
    if (pos == text.size())
      bad();
    if (text[pos] == '"')
      return Json{string()};
    if (eat('[')) {
      Json::Array values;
      space();
      if (eat(']'))
        return Json{std::move(values)};
      do {
        values.push_back(item(depth + 1));
        space();
        if (eat(']'))
          return Json{std::move(values)};
      } while (eat(','));
      bad();
    }
    if (eat('{')) {
      Json::Object fields;
      space();
      if (eat('}'))
        return Json::object(std::move(fields));
      do {
        space();
        auto key = string();
        for (const auto &field : fields)
          if (field.first == key)
            bad();
        space();
        if (!eat(':'))
          bad();
        fields.emplace_back(std::move(key), item(depth + 1));
        space();
        if (eat('}'))
          return Json::object(std::move(fields));
      } while (eat(','));
      bad();
    }
    for (const auto literal : {std::string_view{"true"}, std::string_view{"false"},
                               std::string_view{"null"}}) {
      if (text.substr(pos, literal.size()) == literal) {
        pos += literal.size();
        return literal == "null" ? Json{} : Json{literal == "true"};
      }
    }
    return number();
  }
};
class Writer {
public:
  explicit Writer(JsonLimits bounds) : limits(bounds) {}
  std::string run(const Json &value) {
    item(value, 0);
    return std::move(out);
  }

private:
  JsonLimits limits;
  std::size_t nodes = 0;
  std::string out;
  void add(std::string_view s) {
    if (s.size() > limits.bytes - out.size())
      full();
    out.append(s);
  }
  void string(std::string_view s) {
    if (!utf8(s))
      bad();
    add("\"");
    constexpr char hex[] = "0123456789abcdef";
    for (const auto c : s) {
      if (c == '"')
        add("\\\"");
      else if (c == '\\')
        add("\\\\");
      else if (static_cast<unsigned char>(c) < 32) {
        const auto n = static_cast<unsigned char>(c);
        const char escape[] = {'\\', 'u', '0', '0', hex[n >> 4U], hex[n & 15U]};
        add(std::string_view{escape, 6});
      } else
        add(std::string_view{&c, 1});
    }
    add("\"");
  }
  void item(const Json &j, std::size_t depth) {
    if (depth > limits.depth || nodes == limits.nodes)
      full();
    ++nodes;
    std::visit(
        [&](const auto &v) {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, std::nullptr_t>)
            add("null");
          else if constexpr (std::is_same_v<T, bool>)
            add(v ? "true" : "false");
          else if constexpr (std::is_same_v<T, JsonNumber>) {
            auto check = parse_json(v.text, {limits.bytes, 1, 0});
            if (!check.has_value() ||
                !std::holds_alternative<JsonNumber>(check.value().value()))
              bad();
            add(v.text);
          } else if constexpr (std::is_same_v<T, std::string>)
            string(v);
          else if constexpr (std::is_same_v<T, Json::Array>) {
            add("[");
            bool first = true;
            for (const auto &child : v) {
              if (!first)
                add(",");
              first = false;
              item(child, depth + 1);
            }
            add("]");
          } else {
            add("{");
            bool first = true;
            for (std::size_t i = 0; i < v.size(); ++i) {
              for (std::size_t k = 0; k < i; ++k)
                if (v[k].first == v[i].first)
                  bad();
              if (!first)
                add(",");
              first = false;
              string(v[i].first);
              add(":");
              item(v[i].second, depth + 1);
            }
            add("}");
          }
        },
        j.value());
  }
};
} // namespace
Result<Json> parse_json(std::string_view input, JsonLimits limits) {
  try {
    return Result<Json>::success(Parser{input, limits}.run());
  } catch (const Error &e) {
    return Result<Json>::failure(e);
  } catch (const std::bad_alloc &) {
    return Result<Json>::failure({ErrorCode::allocation});
  } catch (const std::length_error &) {
    return Result<Json>::failure({ErrorCode::capacity});
  }
}
Result<std::string> dump_json(const Json &value, JsonLimits limits) {
  try {
    return Result<std::string>::success(Writer{limits}.run(value));
  } catch (const Error &e) {
    return Result<std::string>::failure(e);
  } catch (const std::bad_alloc &) {
    return Result<std::string>::failure({ErrorCode::allocation});
  } catch (const std::length_error &) {
    return Result<std::string>::failure({ErrorCode::capacity});
  }
}
} // namespace arconaut
