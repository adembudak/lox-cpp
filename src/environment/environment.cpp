#include "lox/environment/environment.h"

#include "lox/token.h"
#include "lox/literal.h"
#include "lox/error/error.h"

#include <string>
#include <memory>
#include <any>

namespace lox {

Environment::Environment(const std::shared_ptr<Environment> &enclosing)
    : m_enclosing(enclosing) {
}

void Environment::define(const std::string &name, const std::any &val) {
  m_values[name] = val;
}

std::any Environment::get(const Token &token) const {
  if (const auto lexeme = token.lexeme; m_values.contains(lexeme)) {
    return m_values.find(lexeme)->second;
  }

  if (m_enclosing) {
    return m_enclosing->get(token);
  }

  throw RuntimeError{token, std::string("Undefined variable '").append(token.lexeme).append("'.")};
}

void Environment::assign(const Token &token, const std::any &value) {
  if (const auto lexeme = token.lexeme; m_values.contains(lexeme)) {
    m_values[lexeme] = value;
    return;
  }

  if (m_enclosing) {
    m_enclosing->assign(token, value);
    return;
  }

  throw RuntimeError{token, std::string("Undefined variable '").append(token.lexeme).append("'.")};
}

std::shared_ptr<Environment> Environment::ancestor(const int distance) {
  auto environment = this->shared_from_this();
  for (int i = 0; i < distance; i++) {
    environment = environment->m_enclosing;
  }
  return environment;
}

void Environment::assignAt(const int distance, const Token &token, const std::any &value) {
  ancestor(distance)->m_values[to_string(token.literal)] = value;
}

}
