#include "lox/environment/environment.h"

#include "lox/token.h"
#include "lox/literal.h"
#include "lox/error/error.h"

#include <string>
#include <memory>
#include <any>

namespace lox {

Environment::Environment(const pEnvironement &enclosing)
    : m_enclosing(enclosing) {
}

void Environment::define(const std::string &name, const std::any &val) {
  m_values[name] = val;
}

std::any Environment::get(const Token &t) const {
  if (const auto lexeme = t.lexeme; m_values.contains(lexeme)) {
    return m_values.find(lexeme)->second;
  }

  if (m_enclosing) {
    return m_enclosing->get(t);
  }

  throw RuntimeError{t, std::string("Undefined variable '").append(t.lexeme).append("'.")};
}

void Environment::assign(const Token &t, const std::any &value) {
  if (const auto lexeme = t.lexeme; m_values.contains(lexeme)) {
    m_values[lexeme] = value;
    return;
  }

  if (m_enclosing) {
    m_enclosing->assign(t, value);
    return;
  }

  throw RuntimeError{t, std::string("Undefined variable '").append(t.lexeme).append("'.")};
}

pEnvironement Environment::ancestor(const int distance) {
  auto environment = this->shared_from_this();
  for (int i = 0; i < distance; i++) {
    environment = environment->m_enclosing;
  }
  return environment;
}

void Environment::assignAt(const int distance, const Token &t, const std::any &value) {
  ancestor(distance)->m_values[to_string(t.literal)] = value;
}

}
