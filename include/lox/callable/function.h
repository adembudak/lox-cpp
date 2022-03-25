#pragma once

#include "lox/ast/stmt.h"
#include "lox/literal.h"

#include <vector>
#include <variant>

namespace lox {
class Interpreter;
class Environment;

class Function {
private:
  const FunctionStmt &m_declaration;
  const Environment &m_closure;

public:
  Function(const FunctionStmt &declaration, const Environment &closure);

  Literal call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const;
  std::size_t arity() const;
};

using Callable = std::variant<std::monostate, Function>;

}
