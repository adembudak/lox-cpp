#pragma once

#include "lox/token.h"
#include "lox/literal.h"

#include <unordered_map>
#include <memory>

namespace lox {

class Environment : public std::enable_shared_from_this<Environment> {
private:
  std::unordered_map<std::string, Literal> m_values;
  std::shared_ptr<Environment> m_enclosing;

public:
  Environment() = default;
  explicit Environment(const std::shared_ptr<Environment> &enclosing);

  void define(const std::string &name, const Literal &val);
  Literal get(const Token &t) const;
  void assign(const Token &name, const Literal &value);
  void assignAt(const int distance, const Token &t, const Literal &value);
  std::shared_ptr<Environment> ancestor(const int distance);
};

}
