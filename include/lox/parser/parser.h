#pragma once

#include "lox/token.h"
#include "lox/ast/stmt.h"
#include "lox/ast/expr.h"

#include <vector>
#include <cstddef>
#include <initializer_list>
#include <string_view>
#include <stdexcept>

namespace lox {

struct parse_error final : virtual public std::runtime_error {
  explicit parse_error(const std::string &what)
      : std::runtime_error(what) {
  }
};

class Parser {
private:
  std::vector<Token> m_tokens;
  std::size_t m_current = 0;

public:
  Parser(const std::vector<Token> &tokens);

  std::vector<Stmt> parse();

private:
  Token peek() const;
  bool isAtEnd() const;
  Token previous() const;
  bool check(const TokenKind t) const;
  Token advance();
  bool match(const TokenKind token);
  bool match(std::initializer_list<TokenKind> tokens);
  Token consume(const TokenKind token, const std::string_view err);
  void synchronize();

private:
  Stmt declaration();
  Stmt variableDeclaration();
  Stmt statement();
  Stmt forStatement();
  Stmt ifStatement();
  Stmt whileStatement();
  Stmt expressionStatement();
  Stmt functionStatment(const std::string &kind);
  std::vector<Stmt> block();
  Stmt printStatement();
  Stmt returnStatement();

  Expr expression();
  Expr assignment();
  Expr or_();
  Expr and_(); // revisit: better naming, get rid of _
  Expr equality();
  Expr comparison();
  Expr term();
  Expr factor();
  Expr unary();
  Expr call();
  Expr primary();

  Expr finishCall(const Expr &callee);
};
}
