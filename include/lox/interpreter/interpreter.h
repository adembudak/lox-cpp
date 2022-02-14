#pragma once

#include "lox/ast/ast.h"

namespace lox {

using Values = std::variant<std::nullptr_t, bool, double, std::string>;

class Interpreter {
private:
  Expr expr;

public:
  Interpreter(const Expr &expr);

  Values interpret() const;
};

}
