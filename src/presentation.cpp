#include "blackbird/value.hpp"
namespace blackbird {
namespace {
class Writer {
public:
  explicit Writer(ValueLimits limits) : limits_(limits) {}
  void append(std::string_view s) {
    if (s.size() > limits_.bytes - out_.size())
      throw Error{ErrorCode::capacity};
    out_.append(s);
  }
  void string(std::string_view s) {
    append("\"");
    const bool utf8 = valid_utf8(s);
    for (const auto c : s) {
      const auto b = static_cast<unsigned char>(c);
      if (b < 32 || b == 127 || (!utf8 && b >= 128) || c == '"' || c == '\\') {
        char escaped[4]{'\\', static_cast<char>('0' + b / 100),
                        static_cast<char>('0' + (b / 10) % 10),
                        static_cast<char>('0' + b % 10)};
        append({escaped, 4});
      } else
        append({&c, 1});
    }
    append("\"");
  }
  void value(const Value &v, std::size_t depth) {
    if (depth > limits_.depth || ++nodes_ > limits_.nodes)
      throw Error{ErrorCode::capacity};
    std::visit(
        [&](const auto &x) {
          using T = std::decay_t<decltype(x)>;
          if constexpr (std::is_same_v<T, std::nullptr_t>)
            append("nil");
          else if constexpr (std::is_same_v<T, bool>)
            append(x ? "true" : "false");
          else if constexpr (std::is_same_v<T, Number>)
            append(x.text());
          else if constexpr (std::is_same_v<T, std::string>)
            string(x);
          else {
            append("{");
            bool first = true;
            for (const auto &entry : x) {
              if (!first)
                append(", ");
              first = false;
              if constexpr (std::is_same_v<T, Value::Object>) {
                append("[");
                string(entry.first);
                append("] = ");
                value(entry.second, depth + 1);
              } else
                value(entry, depth + 1);
            }
            append("}");
          }
        },
        v.value());
  }
  std::string finish() { return std::move(out_); }

private:
  ValueLimits limits_;
  std::size_t nodes_ = 0;
  std::string out_;
};
} // namespace
Result<std::string> format_value(const Value &v, ValueLimits limits) {
  try {
    Writer w{limits};
    w.value(v, 0);
    return Result<std::string>::success(w.finish());
  } catch (const Error &e) {
    return Result<std::string>::failure(e);
  } catch (const std::bad_alloc &) {
    return Result<std::string>::failure({ErrorCode::allocation});
  }
}
} // namespace blackbird
