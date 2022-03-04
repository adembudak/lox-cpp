#include "lox/token.h"
#include "lox/values.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/interpreter/interpreter.h"
#include "lox/environment/environment.h"

#include <boost/variant/static_visitor.hpp>
#include <variant>
#include <string>
#include <iostream>

namespace lox {

bool isTruthy(const Values &values) {
  // clang-format off
  return std::visit(overload {
           [](const std::nullptr_t) { return false; },
           [](const bool ) { return true; },
           [](const double ) { return true; },
           [](const std::string &) { return true; }
      }, values);
  // clang-format on
}

Interpreter::ExpressionVisitor::ExpressionVisitor(Interpreter &interpreter)
    : m_interpreter(interpreter) {
}

Values Interpreter::ExpressionVisitor::evaluate(const Expr &expr) const {
  return boost::apply_visitor(m_interpreter.m_expressionVisitor, expr);
}

Values Interpreter::ExpressionVisitor::operator()(const LiteralExpr &expr) const {
  if (std::holds_alternative<double>(expr.literal)) {
    return std::get<double>(expr.literal);
  }
  return std::get<std::string>(expr.literal);
}

Values Interpreter::ExpressionVisitor::operator()(const GroupingExpr &expr) const {
  return evaluate(expr.expression);
}

Values Interpreter::ExpressionVisitor::operator()(const UnaryExpr &expr) const {
  Values right = evaluate(expr.right);

  switch (expr.op.kind) {
    using enum TokenKind;

    case MINUS:
      return -std::get<double>(right);

    case BANG:
      return isTruthy(right);

    default:;
  }

  return nullptr;
}

Values Interpreter::ExpressionVisitor::operator()(const BinaryExpr &expr) const {
  Values left = evaluate(expr.left);
  Values right = evaluate(expr.right);

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

Values Interpreter::ExpressionVisitor::operator()(const VariableExpr &expr) const {
  return m_interpreter.m_environment->get(expr.name);
}

Values Interpreter::ExpressionVisitor::operator()(const AssignExpr &expr) const {
  Values value = evaluate(expr.value);
  m_interpreter.m_environment->assign(expr.name, value);
  return value;
}

Values Interpreter::ExpressionVisitor::operator()([[maybe_unused]] const auto & /*unused*/) const {
  return {};
}

void executeBlock(const std::vector<Stmt> &statements, const std::shared_ptr<Environment> &env);

Interpreter::StatementVisitor::StatementVisitor(Interpreter &interpreter)
    : m_interpreter(interpreter) {
}

void Interpreter::StatementVisitor::execute(const Stmt &stmt) const {
  boost::apply_visitor(m_interpreter.m_statementVisitor, stmt);
}

void Interpreter::StatementVisitor::operator()(const ExpressionStmt &stmt) const {
  m_interpreter.m_expressionVisitor.evaluate(stmt.expression);
}

void Interpreter::StatementVisitor::operator()(const PrintStmt &stmt) const {
  Values val = m_interpreter.m_expressionVisitor.evaluate(stmt.expression);
  std::cout << to_string(val);
}

void Interpreter::StatementVisitor::operator()(const VarStmt &stmt) const {
  Values val;

  if (!stmt.initializer.empty()) {
    val = m_interpreter.m_expressionVisitor.evaluate(stmt.initializer);
  }

  m_interpreter.m_environment->define(to_string(stmt.name.lexeme), val);
}

void Interpreter::StatementVisitor::operator()(const BlockStmt &stmt) const {
  executeBlock(stmt.statements, std::make_shared<Environment>(m_interpreter.m_environment));
}

void Interpreter::StatementVisitor::operator()(const IfStmt &stmt) const {
  if (isTruthy(m_interpreter.m_expressionVisitor.evaluate(stmt.condition))) {
    execute(stmt.thenBranch);
  } else if (!stmt.elseBranch.empty()) {
    execute(stmt.elseBranch);
  }
}

void Interpreter::StatementVisitor::operator()([[maybe_unused]] const auto & /*unused*/) const {
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
    , m_environment(new Environment)
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

}
