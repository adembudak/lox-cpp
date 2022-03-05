#include "lox/parser/parser.h"
#include "lox/token.h"
#include "lox/literal.h"

#include <boost/variant/get.hpp>

#include <initializer_list>
#include <string_view>
#include <string>
#include <algorithm>
#include <iostream>

namespace lox {

static void report(const std::size_t line, const std::string_view where, const std::string_view message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
}

static parse_error error(const Token &t, const std::string_view message) {
  if (t.kind == TokenKind::END_OF_FILE) {
    report(t.line, " at end", message);
  } else {
    report(t.line, " at '" + to_string(t.lexeme) + "'", message);
  }

  return parse_error{""};
}

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
  bool is_there_a_match = std::any_of(cbegin(tokens), cend(tokens), [&](const auto &token) { return check(token); });

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
void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    using enum TokenKind;

    if (previous().kind == SEMICOLON) {
      return;
    }

    switch (peek().kind) {
      case CLASS:
        [[fallthrough]];
      case FUN:
        [[fallthrough]];
      case VAR:
        [[fallthrough]];
      case FOR:
        [[fallthrough]];
      case IF:
        [[fallthrough]];
      case WHILE:
        [[fallthrough]];
      case PRINT:
        [[fallthrough]];
      case RETURN:
        return;
      default:
        break;
    }
    advance();
  }
}

/////////////////////////

// declaration -> variableDeclaration | statement
Stmt Parser::declaration() {
  try {
    if (match({TokenKind::VAR})) {
      return variableDeclaration();
    }
    return statement();
  } catch (const parse_error &e) {
    synchronize();
    return boost::blank{};
  }
}

// varDecl -> "var" IDENTIFIER ( "=" expression )? ";" ;
Stmt Parser::variableDeclaration() {
  Token name = consume(TokenKind::IDENTIFIER, "Expect variable name.");

  Expr initializer;
  if (match({TokenKind::EQUAL})) {
    initializer = expression();
  }

  consume(TokenKind::SEMICOLON, "Expr ';' after variable declaration.");

  return VarStmt(name, initializer);
}

// statement -> exprStmt | ifStmt | printStmt | block;
Stmt Parser::statement() {
  if (match({TokenKind::IF})) {
    return ifStatement();
  }

  if (match({TokenKind::WHILE})) {
    return whileStatement();
  }

  if (match({TokenKind::PRINT})) {
    return printStatement();
  }

  if (match({TokenKind::LEFT_BRACE})) {
    return BlockStmt(block());
  }

  return expressionStatement();
}

// ifStmt -> "if" "(" expression ")" statement ( "else" statement )? ;
Stmt Parser::ifStatement() {
  consume(TokenKind::LEFT_PAREN, "Expect '(' after 'if'.");
  Expr condition = expression();
  consume(TokenKind::RIGHT_PAREN, "Expect ')' after if condition.");

  Stmt thenBranch = statement();

  Stmt elseBranch;
  if (match({TokenKind::ELSE})) {
    elseBranch = statement();
  }

  return IfStmt(condition, thenBranch, elseBranch);
}

Stmt Parser::whileStatement() {
  consume(TokenKind::LEFT_PAREN, "Expect '(' after 'while'.");
  Expr condition = expression();
  consume(TokenKind::RIGHT_PAREN, "Expect ')' after condition.");

  Stmt body = statement();

  return WhileStmt(condition, body);
}

// exprStmt -> expression ";";
Stmt Parser::expressionStatement() {
  Expr expr = expression();
  consume(TokenKind::SEMICOLON, "Expect ';' after expression.");
  return ExpressionStmt(expr);
}

// block -> "{" declaration "}";
std::vector<Stmt> Parser::block() {
  std::vector<Stmt> statements;
  while (!check(TokenKind::RIGHT_BRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }

  consume(TokenKind::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

// printStmt -> "print" expression ";";
Stmt Parser::printStatement() {
  Expr value = expression();
  consume(TokenKind::SEMICOLON, "Expect ';' after value.");
  return PrintStmt(value);
}

// expression -> assignment;
Expr Parser::expression() {
  return assignment();
}

// assignment -> IDENTIFIER "=" assignment | logic_or;
Expr Parser::assignment() {
  Expr expr = or_();

  if (match({TokenKind::EQUAL})) {
    Token equals = previous();
    Expr value = assignment();

    if (auto *pVariableExpr = boost::get<VariableExpr>(&expr)) {
      Token name = pVariableExpr->name;
      return AssignExpr(name, value);
    }
    error(equals, "Invalid assignment target.");
  }

  return expr;
}

// logic_or -> logic_and ( "or" logic_and )* ;
Expr Parser::or_() {
  Expr expr = and_();

  while (match({TokenKind::OR})) {
    Token op = previous();
    Expr right = and_();
    expr = LogicalExpr(expr, op, right);
  }

  return expr;
}

// logic_and -> equality ( "and" equality )* ;
Expr Parser::and_() {
  Expr expr = equality();

  while (match({TokenKind::AND})) {
    Token op = previous();
    Expr right = equality();
    expr = LogicalExpr(expr, op, right);
  }

  return expr;
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

// primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER ;
Expr Parser::primary() {
  if (match({TokenKind::NUMBER, TokenKind::STRING})) {
    Literal literal = previous().lexeme;
    return LiteralExpr(literal);
  }

  if (match({TokenKind::TRUE})) {
    Literal trueLiteral = true;
    return LiteralExpr(trueLiteral);
  }

  if (match({TokenKind::FALSE})) {
    Literal falseLiteral = false;
    return LiteralExpr(falseLiteral);
  }

  if (match({TokenKind::NIL})) {
    Literal nilLiteral = nullptr;
    return LiteralExpr(nilLiteral);
  }

  if (match({TokenKind::IDENTIFIER})) {
    return VariableExpr(previous());
  }

  if (match({TokenKind::LEFT_PAREN})) {
    Expr expr = expression();
    consume(TokenKind::RIGHT_PAREN, "Expected ')' after expression.");
    return GroupingExpr(expr);
  }

  throw error(peek(), "Expect expression.");
}

Parser::Parser(const std::vector<Token> &tokens)
    : m_tokens(tokens) {
}

std::vector<Stmt> Parser::parse() {
  std::vector<Stmt> statements;

  while (!isAtEnd()) {
    statements.push_back(declaration());
  }

  return statements;
}
}
