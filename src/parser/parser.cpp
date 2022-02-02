#include "lox/parser/parser.h"
#include "lox/token.h"

#include <initializer_list>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <stdexcept>

using namespace lox;

class parse_error : public std::runtime_error {
  std::string err;
  Token t;

public:
  parse_error(const Token &t, const std::string_view err)
      : std::runtime_error(err.data())
      , err(err)
      , t(t) {
  }

  std::string error_string() const {
    return "Error: " + err + "\nAt: " + std::to_string(t.line);
  }
};

Token Parser::peek() const {
  return m_tokens[m_current];
}

bool Parser::isAtEnd() const {
  return peek().kind == TokenKind::END_OF_FILE;
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

bool Parser::check(const TokenKind &t) const {
  if (isAtEnd())
    return false;
  return peek().kind == t;
}

bool Parser::match(std::initializer_list<TokenKind> tokens) {
  bool isThereAnMatch = std::any_of(tokens.begin(), tokens.end(), [&](const auto token) { return check(token); });

  if (isThereAnMatch) {
    advance();
  }
  return isThereAnMatch;
}

Token Parser::consume(const TokenKind &token, const std::string_view err) {
  if (check(token)) {
    return advance();
  }

  throw parse_error(peek(), err);
}

/////////////////////////

// expression -> equality ;
Expr Parser::expression() {
  return equality();
}

// equality -> comparison ( ( "!=" | "==" ) comparison )* ;
Expr Parser::equality() {
  Expr expr = comparison();

  while (match({TokenKind::BANG_EQUAL, TokenKind::EQUAL_EQUAL})) {
    Token op = previous();
    Expr right = comparison();
    expr = BinaryExpr(expr, op, right);
  }

  return expr;
}

// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
Expr Parser::comparison() {
  Expr expr = term();
  while (match({TokenKind::GREATER, TokenKind::GREATER_EQUAL, TokenKind::LESS, TokenKind::LESS_EQUAL})) {
    Token op = previous();
    Expr right = term();
    expr = BinaryExpr(expr, op, right);
  }
  return expr;
}

// term -> factor ( ( "-" | "+" ) factor )* ;
Expr Parser::term() {
  Expr expr = factor();

  while (match({TokenKind::MINUS, TokenKind::PLUS})) {
    Token op = previous();
    Expr right = factor();
    expr = BinaryExpr(expr, op, right);
  }
  return expr;
}

// factor -> unary ( ( "/" | "*" ) unary )* ;
Expr Parser::factor() {
  Expr expr = unary();

  while (match({TokenKind::SLASH, TokenKind::STAR})) {
    Token op = previous();
    Expr right = unary();
    expr = BinaryExpr(expr, op, right);
  }
  return expr;
}

// unary -> ( "!" | "-" ) unary | primary ;
Expr Parser::unary() {
  if (match({TokenKind::BANG, TokenKind::MINUS})) {
    Token op = previous();
    Expr right = unary();
    return UnaryExpr(op, right);
  }

  return primary();
}

// primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
Expr Parser::primary() {
  if (match({TokenKind::NUMBER, TokenKind::STRING})) {
    Literal literal = previous().lexeme;
    return LiteralExpr(literal);
  }

  if (match({TokenKind::TRUE})) {
    Literal trueLiteral = "true";
    return LiteralExpr(trueLiteral);
  }

  if (match({TokenKind::FALSE})) {
    Literal falseLiteral = "false";
    return LiteralExpr(falseLiteral);
  }

  if (match({TokenKind::NIL})) {
    Literal nilLiteral = "nil";
    return LiteralExpr(nilLiteral);
  }

  if (match({TokenKind::LEFT_PAREN})) {
    Expr expr = expression();

    try {
      consume(TokenKind::RIGHT_PAREN, "Expected ')' after expression.");
    } catch (const parse_error &p) {
      std::cerr << p.error_string() << '\n';
    }

    return GroupingExpr(expr);
  }

  throw(peek(), "Expected an expression");
}

std::optional<Expr> Parser::parse() {
  try {
    return expression();
  } catch (const parse_error &e) {
    return std::nullopt;
  }
}
