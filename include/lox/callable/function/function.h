#pragma once

#include "lox/ast/stmt.h"
#include "lox/primitives/literal.h"
#include "lox/environment/environment.h"

#include <vector>
#include <variant>

namespace lox {
class Interpreter;

class Function {
private:
  FunctionStmt m_declaration;
  Environment m_closure;

public:
  Function(const FunctionStmt &declaration, const Environment &closure);

  std::any call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const;
  std::size_t arity() const;
  operator std::string() const;
};

}
