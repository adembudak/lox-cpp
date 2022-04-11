#include "lox/callable/function.h"
#include "lox/callable/return.h"
#include "lox/environment/environment.h"
#include "lox/interpreter/interpreter.h"

#include <vector>
#include <memory>
#include <any>

namespace lox {

Function::Function(const FunctionStmt &declaration, const Environment &closure)
    : m_declaration(declaration)
    , m_closure(closure) {
}

std::any Function::call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const {
  auto environment = m_closure;

  for (std::size_t i = 0; i < m_declaration.params.size(); i++) {
    environment.define(m_declaration.params[i], arguments[i]);
  }
  try {
    interpreter.statementVisitor().executeBlock(m_declaration.body, environment);
  } catch (const Return &ret) {
    return std::any_cast<Literal>(ret.value());
  }

  return nullptr;
}

std::size_t Function::arity() const {
  return m_declaration.params.size();
}

Function::operator std::string() const {
  return std::string("<fn ").append(m_declaration.name.lexeme).append(">");
}

}
