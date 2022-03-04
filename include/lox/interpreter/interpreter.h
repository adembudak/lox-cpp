#pragma once

#include "lox/ast/stmt.h"
#include "lox/values.h"

#include <memory>

namespace lox {

class Environment;

class Interpreter {
private:
  std::vector<Stmt> m_statements;
  std::shared_ptr<Environment> m_environment;

  struct ExpressionVisitor : public boost::static_visitor<Values> {
    ExpressionVisitor(Interpreter &interpreter);

    Values evaluate(const Expr &expr) const;

    Values operator()(const LiteralExpr &expr) const;
    Values operator()(const GroupingExpr &expr) const;
    Values operator()(const UnaryExpr &expr) const;
    Values operator()(const BinaryExpr &expr) const;
    Values operator()(const VariableExpr &expr) const;
    Values operator()(const AssignExpr &expr) const;
    Values operator()([[maybe_unused]] const auto & /*unused*/) const;

  private:
    Interpreter &m_interpreter;
  };

  struct StatementVisitor : public boost::static_visitor<void> {
    StatementVisitor(Interpreter &interpreter);

    void execute(const Stmt &stmt) const;

    void operator()(const ExpressionStmt &stmt) const;
    void operator()(const PrintStmt &stmt) const;
    void operator()(const VarStmt &stmt) const;
    void operator()(const BlockStmt &stmt) const;
    void operator()(const IfStmt &stmt) const;
    void operator()([[maybe_unused]] const auto & /*unused*/) const;

  private:
    void executeBlock(const std::vector<Stmt> &statements, const std::shared_ptr<Environment> &env) const;

    Interpreter &m_interpreter;
  };

  ExpressionVisitor m_expressionVisitor;
  StatementVisitor m_statementVisitor;

public:
  Interpreter(const std::vector<Stmt> &statements);
  void interpret() const;
};

}
