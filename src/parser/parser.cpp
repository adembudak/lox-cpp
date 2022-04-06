#include "lox/parser/parser.h"
#include "lox/token.h"
#include "lox/literal.h"
#include "lox/error/error.h"

#include <boost/variant/get.hpp>

#include <initializer_list>
#include <string_view>
#include <string>
#include <algorithm>

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

bool Parser::check(const TokenKind t) const {
  if (isAtEnd())
    return false;
  return peek().kind == t;
}

bool Parser::match(const TokenKind token) {
  bool isMatched = check(token);
  if (isMatched) {
    advance();
  }
  return isMatched;
}

bool Parser::match(std::initializer_list<TokenKind> tokens) {
  bool isMatched = std::any_of(cbegin(tokens), cend(tokens), [&](const auto &token) { return check(token); });

  if (isMatched) {
    advance();
  }
  return isMatched;
}

[[maybe_unused]] Token Parser::consume(const TokenKind token, const std::string_view err) {
  if (check(token)) {
    return advance();
  }

  throw error(peek(), err);
}
void Parser::synchronize() {
  advance();

  using enum TokenKind;
  while (!isAtEnd()) {

    if (previous().kind == SEMICOLON)
      return;

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
    if (match(TokenKind::CLASS))
      return classDeclaration();

    if (match(TokenKind::FUN))
      return functionStatement("function");

    if (match(TokenKind::VAR))
      return variableDeclaration();

    return statement();

  } catch (const ParseError &e) {
    synchronize();
    return boost::blank{};
  }
}

// classDecl -> "class" IDENTIFIER "{" function* "}" ;
Stmt Parser::classDeclaration() {
  Token name = consume(TokenKind::IDENTIFIER, "Expect class name.");
  consume(TokenKind::LEFT_BRACE, "Expect '{' before class body.");

  std::vector<FunctionStmt> methods;

  while (!check(TokenKind::RIGHT_BRACE) && !isAtEnd()) {
    methods.push_back(functionStatement("method"));
  }

  consume(TokenKind::RIGHT_BRACE, "Expect '}' before class body.");

  return ClassStmt{name, methods};
}

// varDecl -> "var" IDENTIFIER ( "=" expression )? ";" ;
Stmt Parser::variableDeclaration() {
  Token name = consume(TokenKind::IDENTIFIER, "Expect variable name.");

  Expr initializer;
  if (match(TokenKind::EQUAL)) {
    initializer = expression();
  }

  consume(TokenKind::SEMICOLON, "Expr ';' after variable declaration.");

  return VariableStmt{name, initializer};
}

// statement -> exprStmt | forStmt | ifStmt | printStmt | whileStmt | block ;
Stmt Parser::statement() {
  if (match(TokenKind::FOR)) {
    return forStatement();
  }

  if (match(TokenKind::IF)) {
    return ifStatement();
  }

  if (match(TokenKind::PRINT)) {
    return printStatement();
  }

  if (match(TokenKind::RETURN)) {
    return returnStatement();
  }

  if (match(TokenKind::WHILE)) {
    return whileStatement();
  }

  if (match(TokenKind::LEFT_BRACE)) {
    return BlockStmt{block()};
  }

  return expressionStatement();
}

// forStmt -> "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement ;
Stmt Parser::forStatement() {
  consume(TokenKind::LEFT_PAREN, "Expect '(' after 'for'.");

  Stmt initializer = [&]() -> Stmt {
    if (match(TokenKind::SEMICOLON))
      return boost::blank{};

    if (match(TokenKind::VAR))
      return variableDeclaration();

    return expressionStatement();
  }();

  Expr condition = [&]() -> Expr {
    if (!check(TokenKind::SEMICOLON))
      return expression();
    return boost::blank{};
  }();

  consume(TokenKind::SEMICOLON, "Expect ';' after loop condition.");

  Expr increment = [&]() -> Expr {
    if (!check(TokenKind::RIGHT_PAREN))
      return expression();
    return boost::blank{};
  }();

  consume(TokenKind::RIGHT_PAREN, "Expect ')' after for clauses.");

  Stmt body = statement();

  if (increment.which() != 0) {
    body = BlockStmt{{body, ExpressionStmt{increment}}};
  }

  if (condition.which() == 0) { // is its value boost::blank?
    condition = LiteralExpr{true};
  }
  body = WhileStmt{condition, body};

  if (initializer.which() != 0) {
    body = BlockStmt{{initializer, body}};
  }

  return body;
}

// ifStmt -> "if" "(" expression ")" statement ( "else" statement )? ;
Stmt Parser::ifStatement() {
  consume(TokenKind::LEFT_PAREN, "Expect '(' after 'if'.");
  Expr condition = expression();
  consume(TokenKind::RIGHT_PAREN, "Expect ')' after if condition.");

  Stmt thenBranch = statement();

  Stmt elseBranch;
  if (match(TokenKind::ELSE)) {
    elseBranch = statement();
  }

  return IfStmt{condition, thenBranch, elseBranch};
}

Stmt Parser::whileStatement() {
  consume(TokenKind::LEFT_PAREN, "Expect '(' after 'while'.");
  Expr condition = expression();
  consume(TokenKind::RIGHT_PAREN, "Expect ')' after condition.");

  Stmt body = statement();

  return WhileStmt{condition, body};
}

// exprStmt -> expression ";";
Stmt Parser::expressionStatement() {
  Expr expr = expression();
  consume(TokenKind::SEMICOLON, "Expect ';' after expression.");
  return ExpressionStmt{expr};
}

FunctionStmt Parser::functionStatement(const std::string &kind) {
  const Token name = consume(TokenKind::IDENTIFIER, std::string("Expect ").append(kind).append(" name."));
  consume(TokenKind::LEFT_PAREN, std::string("Expect '(' after ").append(kind).append(" name."));

  std::vector<Token> parameters;
  if (!check(TokenKind::RIGHT_PAREN)) {
    do {
      if (const std::size_t max_param = 255; parameters.size() >= max_param)
        error(peek(), "Can't have more than 255 parameters.");

      parameters.push_back(consume(TokenKind::IDENTIFIER, "Expect parameter name."));
    } while (match(TokenKind::COMMA));
  }
  consume(TokenKind::RIGHT_PAREN, "Expect ')' after parameters.");

  consume(TokenKind::LEFT_BRACE, std::string("Expect '{' before ").append(kind).append(" body."));
  std::vector<Stmt> body = block();
  return FunctionStmt{name, parameters, body};
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
  return PrintStmt{value};
}

Stmt Parser::returnStatement() {
  Token keyword = previous();

  Expr value;
  if (!check(TokenKind::SEMICOLON)) {
    value = expression();
  }

  consume(TokenKind::SEMICOLON, "Expect ';' after return value.");
  return ReturnStmt{keyword, value};
}

// expression -> assignment;
Expr Parser::expression() {
  return assignment();
}

// assignment -> ( call "." )? IDENTIFIER "=" assignment | logic_or ;
Expr Parser::assignment() {
  Expr expr = or_();

  if (match(TokenKind::EQUAL)) {
    Token equals = previous();
    Expr value = assignment();

    if (auto *pVariableExpr = boost::get<VariableExpr>(&expr))
      return AssignExpr{pVariableExpr->name, value};
    else if (auto *pGetExpr = boost::get<GetExpr>(&expr))
      return SetExpr{pGetExpr->object, pGetExpr->name, value};

    error(equals, "Invalid assignment target.");
  }

  return expr;
}

// logic_or -> logic_and ( "or" logic_and )* ;
Expr Parser::or_() {
  Expr expr = and_();

  while (match(TokenKind::OR)) {
    Token op = previous();
    Expr right = and_();
    expr = LogicalExpr{expr, op, right};
  }

  return expr;
}

// logic_and -> equality ( "and" equality )* ;
Expr Parser::and_() {
  Expr expr = equality();

  while (match(TokenKind::AND)) {
    Token op = previous();
    Expr right = equality();
    expr = LogicalExpr{expr, op, right};
  }

  return expr;
}

// equality -> comparison ( ( "!=" | "==" ) comparison )* ;
Expr Parser::equality() {
  Expr expr = comparison();

  while (match({TokenKind::BANG_EQUAL, TokenKind::EQUAL_EQUAL})) {
    Token op = previous();
    Expr right = comparison();
    expr = BinaryExpr{expr, op, right};
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
    expr = BinaryExpr{expr, op, right};
  }
  return expr;
}

// term -> factor ( ( "-" | "+" ) factor )* ;
Expr Parser::term() {
  Expr expr = factor();

  while (match({TokenKind::MINUS, TokenKind::PLUS})) {
    Token op = previous();
    Expr right = factor();
    expr = BinaryExpr{expr, op, right};
  }
  return expr;
}

// factor -> unary ( ( "/" | "*" ) unary )* ;
Expr Parser::factor() {
  Expr expr = unary();

  while (match({TokenKind::SLASH, TokenKind::STAR})) {
    Token op = previous();
    Expr right = unary();
    expr = BinaryExpr{expr, op, right};
  }
  return expr;
}

// unary -> ( "!" | "-" ) unary | call ;
Expr Parser::unary() {
  if (match({TokenKind::BANG, TokenKind::MINUS})) {
    Token op = previous();
    Expr right = unary();
    return UnaryExpr{op, right};
  }

  return call();
}

// call -> primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
Expr Parser::call() {
  Expr expr = primary();

  while (true) {
    if (match(TokenKind::LEFT_PAREN)) {
      expr = finishCall(expr);
    } else if (match(TokenKind::DOT)) {
      Token name = consume(TokenKind::IDENTIFIER, "Expect property name after '.'.");
      expr = GetExpr(expr, name);
    } else {
      break;
    }
  }

  return expr;
}

// primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER ;
Expr Parser::primary() {
  if (match({TokenKind::NUMBER, TokenKind::STRING})) {
    Literal literal = previous().literal;
    return LiteralExpr{literal};
  }

  if (match(TokenKind::TRUE)) {
    Literal trueLiteral = true;
    return LiteralExpr{trueLiteral};
  }

  if (match(TokenKind::FALSE)) {
    Literal falseLiteral = false;
    return LiteralExpr{falseLiteral};
  }

  if (match(TokenKind::NIL)) {
    Literal nilLiteral = nullptr;
    return LiteralExpr{nilLiteral};
  }

  if (match(TokenKind::IDENTIFIER)) {
    return VariableExpr{previous()};
  }

  if (match(TokenKind::LEFT_PAREN)) {
    Expr expr = expression();
    consume(TokenKind::RIGHT_PAREN, "Expected ')' after expression.");
    return GroupingExpr{expr};
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

Expr Parser::finishCall(const Expr &callee) {
  std::vector<Expr> arguments;
  if (!check(TokenKind::RIGHT_PAREN)) {
    do {
      if (const std::size_t max_arg = 255; arguments.size() >= max_arg)
        error(peek(), "Can't have more than 255 arguments.");

      arguments.push_back(expression());
    } while (match(TokenKind::COMMA));
  }

  Token paren = consume(TokenKind::RIGHT_PAREN, "Expect ')' after arguments.");

  return CallExpr{callee, paren, arguments};
}

}
