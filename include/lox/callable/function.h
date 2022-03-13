#pragma once

#include "lox/ast/stmt.h"
#include "lox/literal.h"

#include <vector>
#include <variant>

namespace lox {
class Interpreter;

class Function {
private:
  FunctionStmt m_declaration;

public:
  explicit Function(const FunctionStmt &declaration);

  Literal call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const;
  std::size_t arity() const;
};

using Callable = std::variant<std::monostate, Function>;

}
