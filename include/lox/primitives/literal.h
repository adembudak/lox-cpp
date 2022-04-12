#pragma once

#include <variant>
#include <string>

namespace lox {

class Literal {
public:
  using literal_t = std::variant<std::nullptr_t, bool, double, std::string>;

private:
  literal_t m_data;

public:
  Literal() = default;
  Literal(std::nullptr_t n);
  Literal(const bool b);
  Literal(const double d);
  Literal(const std::string &s);

  bool isTruthy() const;
  literal_t data();
  const literal_t data() const;
  operator std::string() const;
};

bool operator==(const Literal &left, const Literal &right);
bool operator!=(const Literal &left, const Literal &right);
std::size_t hash_value(const Literal &value);

}
