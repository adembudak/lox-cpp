#pragma once

#include "lox/token.h"
#include "lox/literal.h"

#include <unordered_map>
#include <memory>
#include <any>

namespace lox {

class Environment;
using pEnvironement = std::shared_ptr<Environment>;

class Environment : public std::enable_shared_from_this<Environment> {
private:
  std::unordered_map<std::string, std::any> m_values;
  pEnvironement m_enclosing;

public:
  Environment() = default;
  explicit Environment(const pEnvironement &enclosing);

  void define(const std::string &name, const std::any &val);
  std::any get(const Token &t) const;
  void assign(const Token &t, const std::any &value);

private:
  pEnvironement ancestor(const int distance);
  void assignAt(const int distance, const Token &t, const std::any &value);
};

}
