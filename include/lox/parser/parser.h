#pragma once

#include "lox/token.h"
#include "lox/ast/ast.h"

#include <vector>
#include <cstdint>
#include <string_view>

namespace lox {

class Parser {
private:
  std::vector<Token> m_tokens;
  std::size_t m_current = 0;

public:
  Parser(const std::vector<Token> &tokens)
      : m_tokens(tokens) {
  }

private:
  Token peek() const;
  bool isAtEnd() const;
  Token previous() const;
  bool check(const tokenKind &t) const;
  Token advance();
  bool match(std::initializer_list<tokenKind> tokens);
  Token consume(const tokenKind &token, const std::string_view err);

private:
  Expr expression();
  Expr equality();
  Expr comparison();
  Expr term();
  Expr factor();
  Expr unary();
  Expr primary();
};

}
