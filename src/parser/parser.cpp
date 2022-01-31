#include "lox/parser/parser.h"
#include "lox/token.h"

#include <initializer_list>
#include <string_view>
#include <algorithm>

using namespace lox;

Token Parser::peek() const {
  return m_tokens[m_current];
}

bool Parser::isAtEnd() const {
  return peek().m_kind == tokenKind::END_OF_FILE;
}

Token Parser::previous() const {
  return m_tokens[m_current - 1];
}

Token Parser::advance() {
  if (!isAtEnd()) {
    ++m_current;
  }
  return previous();
}

bool Parser::check(const tokenKind &t) const {
  if (isAtEnd())
    return false;
  return peek().m_kind == t;
}

bool Parser::match(std::initializer_list<tokenKind> tokens) {
  bool isThereAnMatch = std::any_of(tokens.begin(), tokens.end(), [&](const auto token) { return check(token); });

  if (isThereAnMatch) {
    advance();
  }
  return isThereAnMatch;
}

Token Parser::consume(const tokenKind &token, const std::string_view err) {
  if (check(token)) {
    return advance();
  }

  assert(false && err.data());
}

/////////////////////////

// expression -> equality ;
Expr Parser::expression() {
  return equality();
}

// equality -> comparison ( ( "!=" | "==" ) comparison )* ;
Expr Parser::equality() {
  Expr expr = comparison();

  while (match({tokenKind::BANG_EQUAL, tokenKind::EQUAL_EQUAL})) {
    Token op = previous();
    Expr right = comparison();
    expr = BinaryExpr(expr, op, right);
  }

  return expr;
}

// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
Expr Parser::comparison() {
  Expr expr = term();
  while (match({tokenKind::GREATER, tokenKind::GREATER_EQUAL, tokenKind::LESS, tokenKind::LESS_EQUAL})) {
    Token op = previous();
    Expr right = term();
    expr = BinaryExpr(expr, op, right);
  }
  return expr;
}

// term -> factor ( ( "-" | "+" ) factor )* ;
Expr Parser::term() {
  Expr expr = factor();

  while (match({tokenKind::MINUS, tokenKind::PLUS})) {
    Token op = previous();
    Expr right = factor();
    expr = BinaryExpr(expr, op, right);
  }
  return expr;
}

// factor -> unary ( ( "/" | "*" ) unary )* ;
Expr Parser::factor() {
  Expr expr = unary();

  while (match({tokenKind::SLASH, tokenKind::STAR})) {
    Token op = previous();
    Expr right = unary();
    expr = BinaryExpr(expr, op, right);
  }
  return expr;
}

// unary -> ( "!" | "-" ) unary | primary ;
Expr Parser::unary() {
  if (match({tokenKind::BANG, tokenKind::MINUS})) {
    Token op = previous();
    Expr right = unary();
    return UnaryExpr(op, right);
  }

  return primary();
}

// primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
Expr Parser::primary() {
  if (match({tokenKind::NUMBER, tokenKind::STRING})) {
    literal_t literal = previous().m_lexeme;
    return LiteralExpr(literal);
  }

  if (match({tokenKind::TRUE})) {
    literal_t trueLiteral = "true";
    return LiteralExpr(trueLiteral);
  }

  if (match({tokenKind::FALSE})) {
    literal_t falseLiteral = "false";
    return LiteralExpr(falseLiteral);
  }

  if (match({tokenKind::NIL})) {
    literal_t nilLiteral = "nil";
    return LiteralExpr(nilLiteral);
  }

  if (match({tokenKind::LEFT_PAREN})) {
    Expr expr = expression();
    consume(tokenKind::RIGHT_PAREN, "Expect ')' after expression.");
    return GroupingExpr(expr);
  }

  assert(false);
}

//
