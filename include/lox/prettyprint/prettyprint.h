#pragma once

#include "lox/ast/expr.h"
#include <iosfwd>

namespace lox {

void PrettyPrint(std::ostream &os, const Expr &expr);

}
