#pragma once

#include "lox/token.h"
#include "lox/ast/ast.h"

#include <vector>
#include <cstddef>
#include <initializer_list>
#include <string_view>
#include <stdexcept>
#include <optional>

namespace lox {

struct parse_error final : virtual public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class Parser {
private:
  std::vector<Token> m_tokens;
  std::size_t m_current = 0;

public:
  Parser(const std::vector<Token> &tokens);

  std::optional<Expr> parse();

private:
  Token peek() const;
  bool isAtEnd() const;
  Token previous() const;
  bool check(const TokenKind &t) const;
  Token advance();
  bool match(std::initializer_list<TokenKind> tokens);
  Token consume(const TokenKind &token, const std::string_view err);

private:
  parse_error error(const Token &t, const std::string_view message) const;

  Expr expression();
  Expr equality();
  Expr comparison();
  Expr term();
  Expr factor();
  Expr unary();
  Expr primary();
};

}
