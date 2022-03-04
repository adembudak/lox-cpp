#include "lox/environment/environment.h"

#include "lox/values.h"
#include "lox/token.h"

#include <string>
#include <memory>
#include <stdexcept>

namespace lox {

Environment::Environment(const std::shared_ptr<Environment> &enclosing)
    : m_enclosing(enclosing) {
}

void Environment::define(const std::string &name, const Values &val) {
  m_values[name] = val;
}

Values Environment::get(const Token &t) const {
  const std::string lexeme = to_string(t.lexeme);

  if (m_values.contains(lexeme)) {
    return m_values.find(lexeme)->second;
  }

  if (m_enclosing) {
    return m_enclosing->get(t);
  }

  throw std::runtime_error("Undefined variable '" + lexeme + "'.");
}

void Environment::assign(const Token &name, const Values &value) {
  const std::string lexeme = to_string(name.lexeme);
  if (m_values.contains(lexeme)) {
    m_values[lexeme] = value;
    return;
  }

  if (m_enclosing) {
    m_enclosing->assign(name, value);
    return;
  }

  throw std::runtime_error("Undefined variable '" + lexeme + "'.");
}

void Environment::assignAt(const int distance, const Token &t, const Values &value) {
  ancestor(distance)->m_values[to_string(t.lexeme)] = value;
}

std::shared_ptr<Environment> Environment::ancestor(const int distance) {
  auto environment = shared_from_this();
  for (int i = 0; i < distance; i++) {
    environment = environment->m_enclosing;
  }
  return environment;
}

}
