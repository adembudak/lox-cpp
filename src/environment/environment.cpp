#include "lox/environment/environment.h"

#include "lox/primitives/token.h"
#include "lox/primitives/literal.h"
#include "lox/error/error.h"

#include <string>
#include <memory>
#include <any>

namespace lox {

Environment::Environment(const std::shared_ptr<Environment> &enclosing)
    : m_enclosing(enclosing) {
}

void Environment::define(const Token &token, const std::any &value) {
  m_values[token.lexeme] = value;
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

std::any Environment::getAt(const Token &token, const std::size_t distance) const {
  return ancestor(distance)->get(token);
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

void Environment::assignAt(const Token &token, const std::size_t distance, const std::any &value) {
  ancestor(distance)->m_values[std::string(token.literal)] = value;
}

bool Environment::isGlobalEnvironment() const {
  return m_enclosing == nullptr;
}

std::shared_ptr<Environment> Environment::ancestor(const std::size_t distance) {
  auto environment = this->shared_from_this();
  for (std::size_t i = 0; i < distance; i++) {
    environment = environment->m_enclosing;
  }
  return environment;
}

std::shared_ptr<const Environment> Environment::ancestor(const std::size_t distance) const {
  auto environment = this->shared_from_this();
  for (std::size_t i = 0; i < distance; i++) {
    environment = environment->m_enclosing;
  }
  return environment;
}

}
