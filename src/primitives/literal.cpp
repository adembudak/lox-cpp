#include "lox/primitives/literal.h"

#include <functional>
#include <variant>

namespace {

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

}

namespace lox {

Literal::Literal(std::nullptr_t n)
    : m_data(n) {}

Literal::Literal(const bool b)
    : m_data(b) {}

Literal::Literal(const double d)
    : m_data(d) {}

Literal::Literal(const std::string &s)
    : m_data(s) {}

bool Literal::isTruthy() const {
  // clang-format off
  return std::visit(overload {
           [](const std::nullptr_t) { return false; },
           [](const bool b) { return b; },
           [](const double ) { return true; },
           [](const std::string &) { return true; }
      }, m_data);
  // clang-format on
}

Literal::literal_t Literal::data() {
  return m_data;
}

const Literal::literal_t Literal::data() const {
  return m_data;
}

Literal::operator std::string() const {
  // clang-format off
  using namespace std::string_literals;
  return std::visit(overload {
           [](const std::nullptr_t) { return "nil"s; },
           [](const bool b) { return b ? "true"s: "false"s; },
           [](const double d) { return std::to_string(d); },
           [](const std::string& s) { return s;}
      }, m_data);
  // clang-format on
}

bool operator==(const Literal &left, const Literal &right) {
  if (left.data().index() != right.data().index()) // not the same type
    return false;

  if (left.data().index() == 2 && right.data().index() == 2) // double comparison is tricky, NaN etc.
    return std::get<double>(left.data()) == std::get<double>(right.data());

  return left.isTruthy() == right.isTruthy();
}

bool operator!=(const Literal &left, const Literal &right) {
  return !(left == right);
}

std::size_t hash_value(const Literal &value) {
  return std::hash<Literal::literal_t>{}(value.data());
}

}
