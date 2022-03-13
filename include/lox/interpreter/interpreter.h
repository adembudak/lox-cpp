#pragma once

#include "lox/ast/stmt.h"
#include "lox/callable/function.h"
#include "lox/literal.h"

#include <memory>
#include <any>

namespace lox {
class Environment;

class Interpreter {
private:
  std::vector<Stmt> m_statements;
  const std::shared_ptr<Environment> m_globals;
  std::shared_ptr<Environment> m_environment;

  struct ExpressionVisitor : public boost::static_visitor<std::any> {
    ExpressionVisitor(Interpreter &interpreter);

    std::any evaluate(const Expr &expr) const;

    Literal operator()(const LiteralExpr &expr) const;
    Literal operator()(const LogicalExpr &expr) const;
    Literal operator()(const GroupingExpr &expr) const;
    Literal operator()(const UnaryExpr &expr) const;
    Literal operator()(const BinaryExpr &expr) const;
    Literal operator()(const CallExpr &expr) const;
    std::any operator()(const VariableExpr &expr) const;
    Literal operator()(const AssignExpr &expr) const;
    Literal operator()([[maybe_unused]] const auto & /*unused*/) const;

  private:
    Interpreter &m_interpreter;
  };

  struct StatementVisitor : public boost::static_visitor<void> {
    StatementVisitor(Interpreter &interpreter);

    void execute(const Stmt &stmt) const;

    void operator()(const ExpressionStmt &stmt) const;
    void operator()(const FunctionStmt &stmt) const;
    void operator()(const PrintStmt &stmt) const;
    void operator()(const ReturnStmt &stmt) const;
    void operator()(const VarStmt &stmt) const;
    void operator()(const BlockStmt &stmt) const;
    void operator()(const IfStmt &stmt) const;
    void operator()(const WhileStmt &stmt) const;
    void operator()([[maybe_unused]] const auto & /*unused*/) const;

    void executeBlock(const std::vector<Stmt> &statements, const std::shared_ptr<Environment> &env) const;

  private:
    Interpreter &m_interpreter;
  };

  ExpressionVisitor m_expressionVisitor;
  StatementVisitor m_statementVisitor;

public:
  Interpreter(const std::vector<Stmt> &statements);
  void interpret() const;

  ExpressionVisitor &expressionVisitor();
  const ExpressionVisitor &expressionVisitor() const;

  StatementVisitor &statementVisitor();
  const StatementVisitor &statementVisitor() const;

  std::shared_ptr<Environment> globals();
  const std::shared_ptr<Environment> globals() const;
};

}
