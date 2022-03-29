#include "lox/resolver/resolver.h"
#include "lox/ast/stmt.h"
#include "lox/error/error.h"

#include <ranges>

namespace lox {

Resolver::Resolver(const Interpreter &interpreter)
    : m_interpreter(interpreter) {
}

void Resolver::resolve(const std::vector<Stmt> &statements) {
  for (const Stmt &statement : statements)
    resolve(statement);
}

void Resolver::resolve(const Stmt &stmt) {
  boost::apply_visitor(*this, stmt);
}

void Resolver::operator()(const BlockStmt &stmt) {
  beginScope();
  resolve(stmt.statements);
  endScope();
}

void Resolver::operator()(const ExpressionStmt &stmt) {
  resolve(stmt.expression);
}

void Resolver::operator()(const FunctionStmt &stmt) {
  declare(stmt.name);
  define(stmt.name);
  resolveFunction(stmt);
}

void Resolver::operator()(const IfStmt &stmt) {
  resolve(stmt.condition);
  resolve(stmt.thenBranch);

  if (stmt.elseBranch.which() != 0)
    resolve(stmt.elseBranch);
}

void Resolver::operator()(const PrintStmt &stmt) {
  resolve(stmt.expression);
}

void Resolver::operator()(const ReturnStmt &stmt) {
  if (stmt.value.which() != 0)
    resolve(stmt.value);
}

void Resolver::operator()(const VarStmt &stmt) {
  declare(stmt.name);

  if (stmt.initializer.which() != 0) {
    resolve(stmt.initializer);
  }

  define(stmt.name);
}

void Resolver::operator()(const WhileStmt &stmt) {
  resolve(stmt.condition);
  resolve(stmt.body);
}

//////////////

void Resolver::resolve(const Expr &expr) {
  boost::apply_visitor(*this, expr);
}

void Resolver::operator()(const AssignExpr &expr) {
  resolve(expr.value);
  resolveLocal(expr, expr.name);
}

void Resolver::operator()(const BinaryExpr &expr) {
  resolve(expr.left);
  resolve(expr.left);
}

void Resolver::operator()(const CallExpr &expr) {
  resolve(expr.callee);

  for (const Expr &argument : expr.arguments)
    resolve(argument);
}

void Resolver::operator()(const GroupingExpr &expr) {
  resolve(expr.expression);
}

void Resolver::operator()(const LiteralExpr & /*unused*/) {
}

void Resolver::operator()(const LogicalExpr &expr) {
  resolve(expr.left);
  resolve(expr.right);
}

void Resolver::operator()(const UnaryExpr &expr) {
  resolve(expr.right);
}

void Resolver::operator()(const VariableExpr &expr) {
  if (!m_scopes.empty() && m_scopes.back()[expr.name.lexeme] == false) {
    error(expr.name, "Can't read local variable in its own initializer.");
  }

  resolveLocal(expr, expr.name);
}

void Resolver::operator()(const auto & /*unused*/) {
  // sink
}

void Resolver::beginScope() {
  m_scopes.push_back({});
}

void Resolver::endScope() {
  m_scopes.pop_back();
}

void Resolver::declare(const Token &token) {
  if (m_scopes.empty())
    return;

  m_scopes.back()[token.lexeme] = false;
}

void Resolver::define(const Token &token) {
  if (m_scopes.empty())
    return;

  m_scopes.back()[token.lexeme] = true;
}

void Resolver::resolveLocal(const Expr &expr, const Token &name) {
  for (const auto &scope : m_scopes | std::views::reverse)
    if (scope.contains(name.lexeme))
      m_interpreter.resolve(expr, m_scopes.size());
}

void Resolver::resolveFunction(const FunctionStmt &function) {
  beginScope();

  for (const Token &param : function.params) {
    declare(param);
    define(param);
  }

  resolve(function.body);

  endScope();
}

}
