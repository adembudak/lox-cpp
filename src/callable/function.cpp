#include "lox/callable/function.h"
#include "lox/callable/return.h"
#include "lox/environment/environment.h"
#include "lox/interpreter/interpreter.h"

#include <vector>
#include <memory>
#include <any>

namespace lox {

Function::Function(const FunctionStmt &declaration)
    : m_declaration(declaration) {
}

Literal Function::call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const {
  auto pEnvironment = interpreter.globals();

  for (std::size_t i = 0; i < m_declaration.params.size(); i++) {
    pEnvironment->define(m_declaration.params[i].lexeme, arguments[i]);
  }
  try {
    interpreter.statementVisitor().executeBlock(m_declaration.body, pEnvironment);
  } catch (const Return &ret) {
    return std::any_cast<Literal>(ret.value());
  }

  return nullptr;
}

std::size_t Function::arity() const {
  return m_declaration.params.size();
}

}
