#include "lox/parser/parser.h"
#include "lox/token.h"

#include <initializer_list>
#include <string_view>
#include <string>
#include <algorithm>
#include <iostream>

namespace {

void report(const std::size_t line, const std::string_view where, const std::string_view message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
}

}

namespace lox {

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
  bool is_there_a_match = std::any_of(tokens.begin(), tokens.end(), [&](const auto &token) { return check(token); });

  if (is_there_a_match) {
    advance();
  }
  return is_there_a_match;
}

Token Parser::consume(const TokenKind &token, const std::string_view err) {
  if (check(token)) {
    return advance();
  }

  throw error(peek(), err);
}

/////////////////////////

parse_error Parser::error(const Token &t, const std::string_view message) const {
  if (t.kind == TokenKind::END_OF_FILE) {
    report(t.line, " at end", message);
  } else {
    report(t.line, " at " + to_string(t.lexeme), message);
  }

  return parse_error{""};
}

// statement -> exprStmt | printStmt;
Stmt Parser::statement() {
  if (match({TokenKind::PRINT})) {
    return printStatement();
  }

  return expressionStatement();
}

// exprStmt -> expression ";";
Stmt Parser::expressionStatement() {
  Expr expr = expression();
  consume(TokenKind::SEMICOLON, "Expect ';' after expression.");
  return ExpressionStmt(expr);
}

// printStmt -> "print" expression ";";
Stmt Parser::printStatement() {
  Expr value = expression();
  consume(TokenKind::SEMICOLON, "Expect ';' after value.");
  return PrintStmt(value);
}

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

  using enum TokenKind;
  while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
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
    consume(TokenKind::RIGHT_PAREN, "Expected ')' after expression.");
    return GroupingExpr(expr);
  }

  throw error(peek(), "Expect expression");
}

Parser::Parser(const std::vector<Token> &tokens)
    : m_tokens(tokens) {
}

std::vector<Stmt> Parser::parse() {
  std::vector<Stmt> statements;

  while (!isAtEnd()) {
    statements.push_back(statement());
  }

  return statements;
}

}
