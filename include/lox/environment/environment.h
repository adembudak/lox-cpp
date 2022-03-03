#pragma once

#include "lox/values.h"
#include "lox/token.h"

#include <unordered_map>
#include <memory>

namespace lox {

class Environment : public std::enable_shared_from_this<Environment> {
private:
  std::unordered_map<std::string, Values> m_values;

public:
  std::shared_ptr<Environment> m_enclosing;

  Environment() = default;
  explicit Environment(std::shared_ptr<Environment> enclosing);

  void define(const std::string &name, const Values &val);
  Values get(const Token &t) const;
  void assign(const Token &name, const Values &value);
  void assignAt(const int distance, const Token &t, const Values &value);
  std::shared_ptr<Environment> ancestor(const int distance);
};

}
