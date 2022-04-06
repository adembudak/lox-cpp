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
  return peek().kind == TokenKind::EndOfFile;
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

    if (previous().kind == Semicolon)
      return;

    switch (peek().kind) {
      case Class:
        [[fallthrough]];
      case Fun:
        [[fallthrough]];
      case Var:
        [[fallthrough]];
      case For:
        [[fallthrough]];
      case If:
        [[fallthrough]];
      case While:
        [[fallthrough]];
      case Print:
        [[fallthrough]];
      case Return:
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
    if (match(TokenKind::Class))
      return classDeclaration();

    if (match(TokenKind::Fun))
      return functionStatement("function");

    if (match(TokenKind::Var))
      return variableDeclaration();

    return statement();

  } catch (const ParseError &e) {
    synchronize();
    return boost::blank{};
  }
}

// classDecl -> "class" IDENTIFIER "{" function* "}" ;
Stmt Parser::classDeclaration() {
  Token name = consume(TokenKind::Identifier, "Expect class name.");
  consume(TokenKind::LeftBrace, "Expect '{' before class body.");

  std::vector<FunctionStmt> methods;

  while (!check(TokenKind::RightBrace) && !isAtEnd()) {
    methods.push_back(functionStatement("method"));
  }

  consume(TokenKind::RightBrace, "Expect '}' before class body.");

  return ClassStmt{name, methods};
}

// varDecl -> "var" IDENTIFIER ( "=" expression )? ";" ;
Stmt Parser::variableDeclaration() {
  Token name = consume(TokenKind::Identifier, "Expect variable name.");

  Expr initializer;
  if (match(TokenKind::Equal)) {
    initializer = expression();
  }

  consume(TokenKind::Semicolon, "Expr ';' after variable declaration.");

  return VariableStmt{name, initializer};
}

// statement -> exprStmt | forStmt | ifStmt | printStmt | whileStmt | block ;
Stmt Parser::statement() {
  if (match(TokenKind::For)) {
    return forStatement();
  }

  if (match(TokenKind::If)) {
    return ifStatement();
  }

  if (match(TokenKind::Print)) {
    return printStatement();
  }

  if (match(TokenKind::Return)) {
    return returnStatement();
  }

  if (match(TokenKind::While)) {
    return whileStatement();
  }

  if (match(TokenKind::LeftBrace)) {
    return BlockStmt{block()};
  }

  return expressionStatement();
}

// forStmt -> "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement ;
Stmt Parser::forStatement() {
  consume(TokenKind::LeftParen, "Expect '(' after 'for'.");

  Stmt initializer = [&]() -> Stmt {
    if (match(TokenKind::Semicolon))
      return boost::blank{};

    if (match(TokenKind::Var))
      return variableDeclaration();

    return expressionStatement();
  }();

  Expr condition = [&]() -> Expr {
    if (!check(TokenKind::Semicolon))
      return expression();
    return boost::blank{};
  }();

  consume(TokenKind::Semicolon, "Expect ';' after loop condition.");

  Expr increment = [&]() -> Expr {
    if (!check(TokenKind::RightParen))
      return expression();
    return boost::blank{};
  }();

  consume(TokenKind::RightParen, "Expect ')' after for clauses.");

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
  consume(TokenKind::LeftParen, "Expect '(' after 'if'.");
  Expr condition = expression();
  consume(TokenKind::RightParen, "Expect ')' after if condition.");

  Stmt thenBranch = statement();

  Stmt elseBranch;
  if (match(TokenKind::Else)) {
    elseBranch = statement();
  }

  return IfStmt{condition, thenBranch, elseBranch};
}

Stmt Parser::whileStatement() {
  consume(TokenKind::LeftParen, "Expect '(' after 'while'.");
  Expr condition = expression();
  consume(TokenKind::RightParen, "Expect ')' after condition.");

  Stmt body = statement();

  return WhileStmt{condition, body};
}

// exprStmt -> expression ";";
Stmt Parser::expressionStatement() {
  Expr expr = expression();
  consume(TokenKind::Semicolon, "Expect ';' after expression.");
  return ExpressionStmt{expr};
}

FunctionStmt Parser::functionStatement(const std::string &kind) {
  const Token name = consume(TokenKind::Identifier, std::string("Expect ").append(kind).append(" name."));
  consume(TokenKind::LeftParen, std::string("Expect '(' after ").append(kind).append(" name."));

  std::vector<Token> parameters;
  if (!check(TokenKind::RightParen)) {
    do {
      if (const std::size_t max_param = 255; parameters.size() >= max_param)
        error(peek(), "Can't have more than 255 parameters.");

      parameters.push_back(consume(TokenKind::Identifier, "Expect parameter name."));
    } while (match(TokenKind::Comma));
  }
  consume(TokenKind::RightParen, "Expect ')' after parameters.");

  consume(TokenKind::LeftBrace, std::string("Expect '{' before ").append(kind).append(" body."));
  std::vector<Stmt> body = block();
  return FunctionStmt{name, parameters, body};
}

// block -> "{" declaration "}";
std::vector<Stmt> Parser::block() {
  std::vector<Stmt> statements;
  while (!check(TokenKind::RightBrace) && !isAtEnd()) {
    statements.push_back(declaration());
  }

  consume(TokenKind::RightBrace, "Expect '}' after block.");
  return statements;
}

// printStmt -> "print" expression ";";
Stmt Parser::printStatement() {
  Expr value = expression();
  consume(TokenKind::Semicolon, "Expect ';' after value.");
  return PrintStmt{value};
}

Stmt Parser::returnStatement() {
  Token keyword = previous();

  Expr value;
  if (!check(TokenKind::Semicolon)) {
    value = expression();
  }

  consume(TokenKind::Semicolon, "Expect ';' after return value.");
  return ReturnStmt{keyword, value};
}

// expression -> assignment;
Expr Parser::expression() {
  return assignment();
}

// assignment -> ( call "." )? IDENTIFIER "=" assignment | logic_or ;
Expr Parser::assignment() {
  Expr expr = or_();

  if (match(TokenKind::Equal)) {
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

  while (match(TokenKind::Or)) {
    Token op = previous();
    Expr right = and_();
    expr = LogicalExpr{expr, op, right};
  }

  return expr;
}

// logic_and -> equality ( "and" equality )* ;
Expr Parser::and_() {
  Expr expr = equality();

  while (match(TokenKind::And)) {
    Token op = previous();
    Expr right = equality();
    expr = LogicalExpr{expr, op, right};
  }

  return expr;
}

// equality -> comparison ( ( "!=" | "==" ) comparison )* ;
Expr Parser::equality() {
  Expr expr = comparison();

  while (match({TokenKind::BangEqual, TokenKind::EqualEqual})) {
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
  while (match({Greater, GreaterEqual, Less, LessEqual})) {
    Token op = previous();
    Expr right = term();
    expr = BinaryExpr{expr, op, right};
  }
  return expr;
}

// term -> factor ( ( "-" | "+" ) factor )* ;
Expr Parser::term() {
  Expr expr = factor();

  while (match({TokenKind::Minus, TokenKind::Plus})) {
    Token op = previous();
    Expr right = factor();
    expr = BinaryExpr{expr, op, right};
  }
  return expr;
}

// factor -> unary ( ( "/" | "*" ) unary )* ;
Expr Parser::factor() {
  Expr expr = unary();

  while (match({TokenKind::Slash, TokenKind::Star})) {
    Token op = previous();
    Expr right = unary();
    expr = BinaryExpr{expr, op, right};
  }
  return expr;
}

// unary -> ( "!" | "-" ) unary | call ;
Expr Parser::unary() {
  if (match({TokenKind::Bang, TokenKind::Minus})) {
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
    if (match(TokenKind::LeftParen)) {
      expr = finishCall(expr);
    } else if (match(TokenKind::Dot)) {
      Token name = consume(TokenKind::Identifier, "Expect property name after '.'.");
      expr = GetExpr(expr, name);
    } else {
      break;
    }
  }

  return expr;
}

// primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER ;
Expr Parser::primary() {
  if (match({TokenKind::Number, TokenKind::String})) {
    Literal literal = previous().literal;
    return LiteralExpr{literal};
  }

  if (match(TokenKind::True)) {
    Literal trueLiteral = true;
    return LiteralExpr{trueLiteral};
  }

  if (match(TokenKind::False)) {
    Literal falseLiteral = false;
    return LiteralExpr{falseLiteral};
  }

  if (match(TokenKind::Nil)) {
    Literal nilLiteral = nullptr;
    return LiteralExpr{nilLiteral};
  }

  if (match(TokenKind::Identifier)) {
    return VariableExpr{previous()};
  }

  if (match(TokenKind::LeftParen)) {
    Expr expr = expression();
    consume(TokenKind::RightParen, "Expected ')' after expression.");
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
  if (!check(TokenKind::RightParen)) {
    do {
      if (const std::size_t max_arg = 255; arguments.size() >= max_arg)
        error(peek(), "Can't have more than 255 arguments.");

      arguments.push_back(expression());
    } while (match(TokenKind::Comma));
  }

  Token paren = consume(TokenKind::RightParen, "Expect ')' after arguments.");

  return CallExpr{callee, paren, arguments};
}

}
