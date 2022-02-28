#include "lox/environment/environment.h"

#include "lox/values.h"
#include "lox/token.h"

#include <string>
#include <stdexcept>

namespace lox {

void Environment::define(const std::string &name, const Values &val) {
  m_values[name] = val;
}

Values Environment::get(const Token &t) {
  const std::string lexeme = to_string(t.lexeme);

  if (m_values.contains(lexeme)) {
    return m_values[lexeme];
  }

  throw std::runtime_error(" Undefined variable '" + lexeme + '\'');
}

}
