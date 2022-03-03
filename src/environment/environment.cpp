#include "lox/environment/environment.h"

#include "lox/values.h"
#include "lox/token.h"

#include <string>
#include <stdexcept>

namespace lox {

void Environment::define(const std::string &name, const Values &val) {
  m_values[name] = val;
}

Values Environment::get(const Token &t) const {
  const std::string lexeme = to_string(t.lexeme);

  if (m_values.contains(lexeme)) {
    return m_values.find(lexeme)->second;
  }

  throw std::runtime_error("Undefined variable '" + lexeme + "'.");
}

void Environment::assign(const Token &name, const Values &value) {
  const std::string lexeme = to_string(name.lexeme);
  if (m_values.contains(lexeme)) {
    m_values[lexeme] = value;
    return;
  }
  throw new std::runtime_error("Undefined variable '" + lexeme + "'.");
}

}
