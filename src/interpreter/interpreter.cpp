#include "lox/token.h"
#include "lox/literal.h"
#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"
#include "lox/callable/callable.h"
#include "lox/callable/class.h"
#include "lox/callable/instance.h"
#include "lox/callable/function.h"
#include "lox/callable/return.h"
#include "lox/interpreter/interpreter.h"
#include "lox/environment/environment.h"
#include "lox/error/error.h"

#include <boost/variant/static_visitor.hpp>
#include <fmt/core.h>

#include <variant>
#include <string>
#include <cassert>

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
    if (left.isTruthy())
      return left;
  } else {
    if (!left.isTruthy())
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
      return -std::get<double>(right.data());

    case BANG:
      return !right.isTruthy();

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
      if (std::holds_alternative<double>(left.data()) && std::holds_alternative<double>(right.data()))
        return std::get<double>(left.data()) + std::get<double>(right.data());

      if (std::holds_alternative<std::string>(left.data()) && std::holds_alternative<std::string>(right.data()))
        return std::get<std::string>(left.data()) + std::get<std::string>(right.data());

      throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
    }

    case MINUS:
      return std::get<double>(left.data()) - std::get<double>(right.data());

    case SLASH:
      return std::get<double>(left.data()) / std::get<double>(right.data());

    case STAR:
      return std::get<double>(left.data()) * std::get<double>(right.data());

    case GREATER:
      return std::get<double>(left.data()) > std::get<double>(right.data());

    case GREATER_EQUAL:
      return std::get<double>(left.data()) >= std::get<double>(right.data());

    case LESS:
      return std::get<double>(left.data()) < std::get<double>(right.data());

    case LESS_EQUAL:
      return std::get<double>(left.data()) <= std::get<double>(right.data());

    case EQUAL_EQUAL:
      return left == right;

    case BANG_EQUAL:
      return left != right;

    default:
      break;
  }

  return nullptr;
}

std::any Interpreter::ExpressionVisitor::operator()(const CallExpr &expr) const {

  std::any ret = evaluate(expr.callee);
  Callable callee;

  if (ret.type() == typeid(Function))
    callee = std::any_cast<Function>(evaluate(expr.callee));
  else if (ret.type() == typeid(Class))
    callee = std::any_cast<Class>(evaluate(expr.callee));
  else
    throw RuntimeError(expr.paren, "Can only call functions and classes.");

  std::vector<Literal> arguments;
  std::transform(begin(expr.arguments), end(expr.arguments), back_inserter(arguments), //
                 [this](const Expr &expr) { return std::any_cast<Literal>(evaluate(expr)); });

  auto checkArity = [&expr](std::size_t funcArgs, std::size_t argsSize) {
    if (funcArgs != argsSize)
      throw RuntimeError(expr.paren,
                         std::string("Expected [")
                             .append(std::to_string(funcArgs)) //
                             .append("] arguments but got ")   //
                             .append(std::to_string(argsSize)) //
                             .append(".\n"));
  };

  if (Function *callable = std::get_if<Function>(&callee)) {
    checkArity(callable->arity(), arguments.size());
    return callable->call(m_interpreter, arguments);
  }

  Class *callable = std::get_if<Class>(&callee);
  checkArity(callable->arity(), arguments.size());
  return callable->call(m_interpreter, arguments);
}

std::any Interpreter::ExpressionVisitor::operator()(const GetExpr &expr) const {
  std::any ret = evaluate(expr.object);
  if (ret.type() == typeid(Instance)) {
    Instance instance = std::any_cast<Instance>(ret);
    return instance.get(expr.name);
  }
  throw RuntimeError(expr.name, "Only instances have properties.");
}

std::any Interpreter::ExpressionVisitor::operator()(const VariableExpr &expr) const {
  return m_interpreter.m_environment.get(expr.name);
}

Literal Interpreter::ExpressionVisitor::operator()(const AssignExpr &expr) const {
  Literal value = std::any_cast<Literal>(evaluate(expr.value));

  if (const auto &locals = m_interpreter.m_locals; locals.contains(expr)) {
    const std::size_t distance = locals.find(expr)->second;
    m_interpreter.m_environment.assignAt(expr.name, distance, value);
  } else {
    m_interpreter.m_globals.assign(expr.name, value);
  }

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
  Function function{stmt, m_interpreter.m_environment};
  m_interpreter.m_environment.define(stmt.name, function);
}

void Interpreter::StatementVisitor::operator()(const PrintStmt &stmt) const {
  std::any ret = m_interpreter.m_expressionVisitor.evaluate(stmt.expression);

  if (ret.type() == typeid(Literal)) {
    fmt::print("{}\n", std::string(std::any_cast<Literal>(ret)));
  } else if (ret.type() == typeid(Class)) {
    fmt::print("{}\n", std::string(std::any_cast<Class>(ret)));
  } else {
    fmt::print("{}\n", std::string(std::any_cast<Instance>(ret)));
  }
}

void Interpreter::StatementVisitor::operator()(const ReturnStmt &stmt) const {
  Literal value;
  if (stmt.value.which() != 0) // is its type boost::blank?
    value = std::any_cast<Literal>(m_interpreter.m_expressionVisitor.evaluate(stmt.value));

  throw Return{value};
}

void Interpreter::StatementVisitor::operator()(const VariableStmt &stmt) const {
  std::any val;

  if (stmt.initializer.which() != 0) {
    val = m_interpreter.m_expressionVisitor.evaluate(stmt.initializer);
  }

  m_interpreter.m_environment.define(stmt.name, val);
}

void Interpreter::StatementVisitor::operator()(const BlockStmt &stmt) const {
  const auto newEnvironment = m_interpreter.m_environment;
  executeBlock(stmt.statements, newEnvironment);
}

void Interpreter::StatementVisitor::operator()(const ClassStmt &stmt) const {
  m_interpreter.m_environment.define(stmt.name, nullptr);
  m_interpreter.m_environment.assign(stmt.name, Class{stmt.name.lexeme});
}

void Interpreter::StatementVisitor::operator()(const IfStmt &stmt) const {
  if (std::any_cast<Literal>(m_interpreter.m_expressionVisitor.evaluate(stmt.condition)).isTruthy()) {
    execute(stmt.thenBranch);
  } else if (stmt.elseBranch.which() != 0) {
    execute(stmt.elseBranch);
  }
}

void Interpreter::StatementVisitor::operator()(const WhileStmt &stmt) const {
  while (std::any_cast<Literal>(m_interpreter.m_expressionVisitor.evaluate(stmt.condition)).isTruthy())
    execute(stmt.body);
}

void Interpreter::StatementVisitor::operator()([[maybe_unused]] const auto & /*unused*/) const {
  /* sink */
}

void Interpreter::StatementVisitor::executeBlock(const std::vector<Stmt> &statements, const Environment &env) const {
  const auto previous = m_interpreter.m_environment;

  m_interpreter.m_environment = env;

  for (const auto &statement : statements) {
    execute(statement);
  }

  m_interpreter.m_environment = previous;
}

Interpreter::Interpreter(const std::vector<Stmt> &statements)
    : m_statements(statements)
    , m_environment(m_globals)
    , m_expressionVisitor(*this)
    , m_statementVisitor(*this) {
}

void Interpreter::interpret() const {
  try {
    for (const Stmt &statement : m_statements) {
      m_statementVisitor.execute(statement);
    }
  } catch (const RuntimeError &e) {
    runtimeError(e);
  }
}

void Interpreter::resolve(const Expr &expr, const std::size_t depth) {
  m_locals[expr] = depth;
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

std::any Interpreter::lookUpVariable(const Token &name, const Expr &expr) const {
  if (m_locals.contains(expr))
    return m_environment.getAt(name, m_locals.find(expr)->second);
  return m_globals.get(name);
}
}
