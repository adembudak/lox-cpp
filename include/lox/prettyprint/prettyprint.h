#pragma once

#include "lox/ast/ast.h"
#include <iosfwd>

namespace lox {

void PrettyPrint(std::ostream &os, const Expr &expr);

}
