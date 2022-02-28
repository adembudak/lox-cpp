#pragma once

#include <variant>
#include <string>

namespace lox {

using Values = std::variant<std::nullptr_t, bool, double, std::string>;

template <class... Ts>
struct overload_ : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload_(Ts...) -> overload_<Ts...>;

inline std::string to_string(const Values &values) {
  // clang-format off
  using namespace std::string_literals;
  return std::visit(overload_ {
           [](const std::nullptr_t) { return "null"s; },
           [](const bool b) { return b ? "true"s: "false"s; },
           [](const double d) { return std::to_string(d); },
           [](const std::string& s) { return s;}
      }, values);
  // clang-format on
}

}
