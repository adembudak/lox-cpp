#include "lox/token.h"
#include "lox/scanner/scanner.h"
#include "lox/ast/ast.h"
#include "lox/parser/parser.h"
#include "lox/prettyprint/prettyprint.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

// clang-format off

int main() {

  auto expr = 
        lox::BinaryExpr(
               lox::UnaryExpr(lox::Token{lox::TokenKind::MINUS, "-", 1}, lox::LiteralExpr(lox::Literal{123.0})), 
               lox::Token{lox::TokenKind::STAR, "*", 1},
               lox::GroupingExpr(lox::LiteralExpr(45.67))
             );

  // clang-format on

  lox::PrettyPrint(std::cout, expr);

  return 0;
}
