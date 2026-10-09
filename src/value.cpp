#include "blackbird/value.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>

namespace blackbird {
const Value *Value::find(std::string_view key) const noexcept {
  if (const auto *fields = std::get_if<Object>(&value()))
    for (const auto &[name, v] : *fields)
      if (name == key)
        return &v;
  return nullptr;
}
Number::Number(Decimal n) : data_(std::int64_t{0}) {
  if (n.limbs.empty() || std::any_of(n.limbs.begin(), n.limbs.end(),
                                     [](auto limb) { return limb >= 1000000000; }))
    throw Error{ErrorCode::invalid_range};
  while (n.limbs.size() > 1 && n.limbs.back() == 0)
    n.limbs.pop_back();
  if (n.limbs.size() == 1 && n.limbs[0] == 0)
    n.exponent = 0;
  else
    while (n.limbs[0] % 10 == 0) {
      if (n.exponent == INT64_MAX)
        throw Error{ErrorCode::invalid_range};
      std::uint64_t carry = 0;
      for (std::size_t i = n.limbs.size(); i; --i) {
        const auto limb = carry * 1000000000 + n.limbs[i - 1];
        n.limbs[i - 1] = static_cast<std::uint32_t>(limb / 10);
        carry = limb % 10;
      }
      while (n.limbs.size() > 1 && n.limbs.back() == 0)
        n.limbs.pop_back();
      ++n.exponent;
    }
  data_ = std::move(n);
}
Number::Number(double n) : data_(n) {
  if (!std::isfinite(n))
    throw Error{ErrorCode::invalid_range};
}
Number::Number(std::string_view text) : data_(std::int64_t{0}) {
  auto parsed = parse_number(text);
  if (!parsed.has_value())
    throw parsed.error();
  data_ = std::move(parsed).value().data_;
}
Result<Number> parse_number(std::string_view text) {
  try {
    if (text.empty())
      return Result<Number>::failure({ErrorCode::invalid_range});
    std::size_t at = 0;
    const bool negative = text[0] == '-';
    if (negative)
      ++at;
    const auto begin = at;
    if (at == text.size() || text[at] < '0' || text[at] > '9')
      return Result<Number>::failure({ErrorCode::invalid_range});
    if (text[at] == '0')
      ++at;
    else
      while (at < text.size() && text[at] >= '0' && text[at] <= '9')
        ++at;
    std::string digits{text.substr(begin, at - begin)};
    std::int64_t scale = 0;
    bool decimal = false;
    if (at < text.size() && text[at] == '.') {
      decimal = true;
      const auto start = ++at;
      while (at < text.size() && text[at] >= '0' && text[at] <= '9')
        ++at;
      if (at == start)
        return Result<Number>::failure({ErrorCode::invalid_range});
      if (at - start > static_cast<std::size_t>(INT64_MAX))
        return Result<Number>::failure({ErrorCode::capacity});
      scale = -static_cast<std::int64_t>(at - start);
      digits.append(text.substr(start, at - start));
    }
    if (at < text.size() && (text[at] == 'e' || text[at] == 'E')) {
      decimal = true;
      ++at;
      const bool neg = at < text.size() && text[at] == '-';
      if (at < text.size() && (text[at] == '-' || text[at] == '+'))
        ++at;
      const auto start = at;
      while (at < text.size() && text[at] >= '0' && text[at] <= '9')
        ++at;
      std::uint64_t exp = 0;
      const auto r = std::from_chars(text.data() + start, text.data() + at, exp);
      if (start == at || r.ec != std::errc{} ||
          exp >
              (neg ? (std::uint64_t{1} << 63) : static_cast<std::uint64_t>(INT64_MAX)))
        return Result<Number>::failure({ErrorCode::invalid_range});
      const auto signed_exp =
          neg ? (exp == (std::uint64_t{1} << 63) ? INT64_MIN
                                                 : -static_cast<std::int64_t>(exp))
              : static_cast<std::int64_t>(exp);
      if (signed_exp < INT64_MIN - scale)
        return Result<Number>::failure({ErrorCode::invalid_range});
      scale += signed_exp;
    }
    if (at != text.size())
      return Result<Number>::failure({ErrorCode::invalid_range});
    if (!decimal) {
      if (negative && digits != "0") {
        std::int64_t n = 0;
        const auto r = std::from_chars(text.data(), text.data() + text.size(), n);
        if (r.ec == std::errc{})
          return Result<Number>::success(Number{n});
      } else if (!negative) {
        std::uint64_t n = 0;
        const auto r = std::from_chars(text.data(), text.data() + text.size(), n);
        if (r.ec == std::errc{})
          return Result<Number>::success(Number{n});
      }
    }
    const auto first = digits.find_first_not_of('0');
    if (first == std::string::npos)
      digits = "0";
    else
      digits.erase(0, first);
    if (digits == "0")
      scale = 0;
    while (digits.size() > 1 && digits.back() == '0') {
      if (scale == INT64_MAX)
        return Result<Number>::failure({ErrorCode::invalid_range});
      digits.pop_back();
      ++scale;
    }
    Decimal out{negative, scale, {}};
    for (std::size_t end = digits.size(); end;) {
      const auto start = end > 9 ? end - 9 : 0;
      std::uint32_t limb = 0;
      const auto r = std::from_chars(digits.data() + start, digits.data() + end, limb);
      if (r.ec != std::errc{})
        return Result<Number>::failure({ErrorCode::invalid_range});
      out.limbs.push_back(limb);
      end = start;
    }
    return Result<Number>::success(Number{std::move(out)});
  } catch (const std::bad_alloc &) {
    return Result<Number>::failure({ErrorCode::allocation});
  }
}
std::string Number::text() const {
  return std::visit(
      [](const auto &n) -> std::string {
        using T = std::decay_t<decltype(n)>;
        if constexpr (std::is_same_v<T, Decimal>) {
          if (n.limbs.empty())
            throw Error{ErrorCode::invalid_range};
          std::string digits = std::to_string(n.limbs.back());
          for (std::size_t i = n.limbs.size() - 1; i; --i) {
            const auto next = std::to_string(n.limbs[i - 1]);
            digits += std::string(9 - next.size(), '0') + next;
          }
          std::string out = n.negative ? "-" : "";
          if (n.exponent >= 0 && n.exponent <= 16)
            out += digits + std::string(static_cast<std::size_t>(n.exponent), '0');
          else if (n.exponent < 0 && n.exponent >= -16) {
            const auto places = static_cast<std::size_t>(-n.exponent);
            if (places < digits.size()) {
              digits.insert(digits.size() - places, 1, '.');
              out += digits;
            } else
              out += "0." + std::string(places - digits.size(), '0') + digits;
          } else
            out += digits + "e" + std::to_string(n.exponent);
          return out;
        } else if constexpr (std::is_same_v<T, double>) {
          char text[128];
          const auto r = std::to_chars(text, text + sizeof(text), n);
          if (r.ec != std::errc{})
            throw Error{ErrorCode::invalid_range};
          return {text, r.ptr};
        } else
          return std::to_string(n);
      },
      data_);
}
bool Number::operator==(const Number &other) const {
  if (data_ == other.data_)
    return true;
  return text() == other.text();
}
bool valid_utf8(std::string_view s) {
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
} // namespace blackbird
