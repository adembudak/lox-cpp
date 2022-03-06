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

inline bool isTruthy(const Literal &value) {
  // clang-format off
  return std::visit(overload {
           [](const std::nullptr_t) { return false; },
           [](const bool b) { return b; },
           [](const double ) { return true; },
           [](const std::string &) { return true; }
      }, value);
  // clang-format on
}

inline bool operator==(const Literal &left, const Literal &right) {
  if (left.index() == 0 && right.index() == 0) // nullptr == nullptr
    return true;
  if (left.index() == 0 && right.index() != 0) // nullptr == bool, double, string
    return false;
  if (left.index() != right.index())
    return false;
  return isTruthy(left) == isTruthy(right);
}

inline bool operator!=(const Literal &left, const Literal &right) {
  return !(left == right);
}

}
