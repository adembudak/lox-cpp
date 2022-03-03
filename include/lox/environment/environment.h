#pragma once

#include "lox/values.h"
#include "lox/token.h"

#include <unordered_map>

namespace lox {

class Environment {
private:
  std::unordered_map<std::string, Values> m_values;

public:
  void define(const std::string &name, const Values &val);
  Values get(const Token &t) const;
  void assign(const Token &name, const Values &value);
};

}
