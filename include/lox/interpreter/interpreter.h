#pragma once

#include "lox/ast/stmt.h"

namespace lox {

class Interpreter {
private:
  std::vector<Stmt> statements;

public:
  Interpreter(const std::vector<Stmt> &statements);

  void interpret() const;
};

}
