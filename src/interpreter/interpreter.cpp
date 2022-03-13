#include "lox/token.h"
#include "lox/literal.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/callable/return.h"
#include "lox/interpreter/interpreter.h"
#include "lox/environment/environment.h"
#include "lox/callable/function.h"

#include <boost/variant/static_visitor.hpp>

#include <variant>
#include <string>
#include <iostream>

namespace lox {

Interpreter::ExpressionVisitor::ExpressionVisitor(Interpreter &interpreter)
    : m_interpreter(interpreter) {
}

std::any Interpreter::ExpressionVisitor::evaluate(const Expr &expr) const {
  return boost::apply_visitor(m_interpreter.m_expressionVisitor, expr);
}

Literal Interpreter::ExpressionVisitor::operator()(const LiteralExpr &expr) const {
  return expr.literal;
}

Literal Interpreter::ExpressionVisitor::operator()(const LogicalExpr &expr) const {
  Literal left = std::any_cast<Literal>(evaluate(expr.left));
  if (expr.op.kind == TokenKind::OR) {
    if (isTruthy(left))
      return left;
  } else {
    if (!isTruthy(left))
      return left;
  }

  return std::any_cast<Literal>(evaluate(expr.right));
}

Literal Interpreter::ExpressionVisitor::operator()(const GroupingExpr &expr) const {
  return std::any_cast<Literal>(evaluate(expr.expression));
}

Literal Interpreter::ExpressionVisitor::operator()(const UnaryExpr &expr) const {
  Literal right = std::any_cast<Literal>(evaluate(expr.right));

  switch (expr.op.kind) {
    using enum TokenKind;

    case MINUS:
      return -std::get<double>(right);

    case BANG:
      return !isTruthy(right);

    default:;
  }

  return nullptr;
}

Literal Interpreter::ExpressionVisitor::operator()(const BinaryExpr &expr) const {
  Literal left = std::any_cast<Literal>(evaluate(expr.left));
  Literal right = std::any_cast<Literal>(evaluate(expr.right));

  switch (expr.op.kind) {
    using enum TokenKind;

    case PLUS: {
      if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
        return std::get<double>(left) + std::get<double>(right);

      if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
        return std::get<std::string>(left) + std::get<std::string>(right);
    }

    case MINUS:
      return std::get<double>(left) - std::get<double>(right);

    case SLASH:
      return std::get<double>(left) / std::get<double>(right);

    case STAR:
      return std::get<double>(left) * std::get<double>(right);

    case GREATER:
      return std::get<double>(left) > std::get<double>(right);

    case GREATER_EQUAL:
      return std::get<double>(left) >= std::get<double>(right);

    case LESS:
      return std::get<double>(left) < std::get<double>(right);

    case LESS_EQUAL:
      return std::get<double>(left) <= std::get<double>(right);

    case EQUAL_EQUAL:
      return left == right;

    case BANG_EQUAL:
      return left != right;

    default:
      break;
  }

  return nullptr;
}

Literal Interpreter::ExpressionVisitor::operator()(const CallExpr &expr) const {
  try {
    Callable callee = std::any_cast<Function>(evaluate(expr.callee));

    std::vector<Literal> arguments;
    std::transform(begin(expr.arguments), end(expr.arguments), std::back_inserter(arguments), //
                   [this](const Expr &expr) { return std::any_cast<Literal>(evaluate(expr)); });

    Function function = std::get<Function>(callee);

    if (function.arity() != arguments.size()) {
      throw std::runtime_error("Expected [" + std::to_string(function.arity()) + "] arguments but got " + std::to_string(arguments.size()) + '.');
    }

    return function.call(m_interpreter, arguments);

  } catch (const std::bad_any_cast &e) {
    std::cerr << "Can only call functions and classes.\n" << e.what();
    return {};
  } catch (const std::runtime_error &e) {
    std::cout << e.what();
    return {};
  }
}

std::any Interpreter::ExpressionVisitor::operator()(const VariableExpr &expr) const {
  return m_interpreter.m_environment->get(expr.name);
}

Literal Interpreter::ExpressionVisitor::operator()(const AssignExpr &expr) const {
  Literal value = std::any_cast<Literal>(evaluate(expr.value));
  m_interpreter.m_environment->assign(expr.name, value);
  return value;
}

Literal Interpreter::ExpressionVisitor::operator()([[maybe_unused]] const auto & /*unused*/) const {
  return {};
}

Interpreter::StatementVisitor::StatementVisitor(Interpreter &interpreter)
    : m_interpreter(interpreter) {
}

void Interpreter::StatementVisitor::execute(const Stmt &stmt) const {
  boost::apply_visitor(m_interpreter.m_statementVisitor, stmt);
}

void Interpreter::StatementVisitor::operator()(const ExpressionStmt &stmt) const {
  m_interpreter.m_expressionVisitor.evaluate(stmt.expression);
}

void Interpreter::StatementVisitor::operator()(const FunctionStmt &stmt) const {
  Function function{stmt};
  m_interpreter.m_environment->define(stmt.name.lexeme, function);
}

void Interpreter::StatementVisitor::operator()(const PrintStmt &stmt) const {
  Literal val = std::any_cast<Literal>(m_interpreter.m_expressionVisitor.evaluate(stmt.expression));
  std::cout << to_string(val) << '\n';
}

void Interpreter::StatementVisitor::operator()(const ReturnStmt &stmt) const {
  Literal value;
  if (stmt.value.which() != 0) // is its type boost::blank?
    value = std::any_cast<Literal>(m_interpreter.m_expressionVisitor.evaluate(stmt.value));

  throw Return{value};
}

void Interpreter::StatementVisitor::operator()(const VarStmt &stmt) const {
  Literal val;

  if (!stmt.initializer.empty()) {
    val = std::any_cast<Literal>(m_interpreter.m_expressionVisitor.evaluate(stmt.initializer));
  }

  m_interpreter.m_environment->define(to_string(stmt.name.literal), val);
}

void Interpreter::StatementVisitor::operator()(const BlockStmt &stmt) const {
  executeBlock(stmt.statements, std::make_shared<Environment>(m_interpreter.m_environment));
}

void Interpreter::StatementVisitor::operator()(const IfStmt &stmt) const {
  if (isTruthy(std::any_cast<Literal>(m_interpreter.m_expressionVisitor.evaluate(stmt.condition)))) {
    execute(stmt.thenBranch);
  } else if (!stmt.elseBranch.empty()) {
    execute(stmt.elseBranch);
  }
}

void Interpreter::StatementVisitor::operator()(const WhileStmt &stmt) const {
  while (isTruthy(std::any_cast<Literal>(m_interpreter.m_expressionVisitor.evaluate(stmt.condition))))
    execute(stmt.body);
}

void Interpreter::StatementVisitor::operator()([[maybe_unused]] const auto & /*unused*/) const {
  /* sink */
}

void Interpreter::StatementVisitor::executeBlock(const std::vector<Stmt> &statements, const std::shared_ptr<Environment> &env) const {
  const auto previous = m_interpreter.m_environment;
  m_interpreter.m_environment = env;

  for (const auto &statement : statements) {
    execute(statement);
  }
  m_interpreter.m_environment = previous;
}

Interpreter::Interpreter(const std::vector<Stmt> &statements)
    : m_statements(statements)
    , m_globals(std::make_shared<Environment>())
    , m_environment(m_globals)
    , m_expressionVisitor(*this)
    , m_statementVisitor(*this) {
}

void Interpreter::interpret() const {
  try {
    for (const auto &statement : m_statements) {
      m_statementVisitor.execute(statement);
    }
  } catch (const std::runtime_error &e) {
    throw e.what();
  }
}

Interpreter::ExpressionVisitor &Interpreter::expressionVisitor() {
  return m_expressionVisitor;
}

const Interpreter::ExpressionVisitor &Interpreter::expressionVisitor() const {
  return m_expressionVisitor;
}

Interpreter::StatementVisitor &Interpreter::statementVisitor() {
  return m_statementVisitor;
}

const Interpreter::StatementVisitor &Interpreter::statementVisitor() const {
  return m_statementVisitor;
}

std::shared_ptr<Environment> Interpreter::globals() {
  return m_globals;
}

const std::shared_ptr<Environment> Interpreter::globals() const {
  return m_globals;
}
}
