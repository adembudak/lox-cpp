#pragma once

#include <variant>
#include <string>

namespace lox {

using Literal = std::variant<std::monostate, std::string, double>;
constexpr const std::monostate noopt;

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

inline std::string to_string(const lox::Literal &t) {
  // clang-format off
  return std::visit(overload {
               [](const std::monostate) { return std::string{}; },
               [](const std::string &s) { return s; },
               [](const double d)      { return std::to_string(d); }
             }, t);
  // clang-format on
}

}
