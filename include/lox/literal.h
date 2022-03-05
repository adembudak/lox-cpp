#pragma once

#include <variant>
#include <string>

namespace lox {

using Literal = std::variant<std::nullptr_t, bool, double, std::string>;

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

inline std::string to_string(const Literal &values) {
  // clang-format off
  using namespace std::string_literals;
  return std::visit(overload {
           [](const std::nullptr_t) { return "nil"s; },
           [](const bool b) { return b ? "true"s: "false"s; },
           [](const double d) { return std::to_string(d); },
           [](const std::string& s) { return s;}
      }, values);
  // clang-format on
}

inline bool operator==(const Literal &left, const Literal &right) {
  if (left.index() == 0 && right.index() == 0)
    return true;
  if (left.index() == 0 && right.index() != 0)
    return false;
  return left == right;
}

inline bool operator!=(const Literal &left, const Literal &right) {
  return !(left == right);
}

}
