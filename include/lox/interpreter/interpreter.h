#pragma once

#include "lox/ast/stmt.h"
#include "lox/ast/expr.h"
#include "lox/callable/function.h"
#include "lox/environment/environment.h"
#include "lox/literal.h"

#include <boost/variant/static_visitor.hpp>

#include <memory>
#include <vector>
#include <unordered_map>
#include <any>
#include <cstddef>

namespace lox {

class Interpreter {
private:
  std::unordered_map<Expr, std::size_t> m_locals;
  std::vector<Stmt> m_statements;
  Environment m_globals;
  Environment m_environment;

private:
  struct ExpressionVisitor : public boost::static_visitor<std::any> {
    ExpressionVisitor(Interpreter &interpreter);

    std::any evaluate(const Expr &expr) const;

    Literal operator()(const LiteralExpr &expr) const;
    Literal operator()(const LogicalExpr &expr) const;
    Literal operator()(const GroupingExpr &expr) const;
    Literal operator()(const UnaryExpr &expr) const;
    Literal operator()(const BinaryExpr &expr) const;
    std::any operator()(const CallExpr &expr) const;
    std::any operator()(const GetExpr &expr) const;
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
    void operator()(const VariableStmt &stmt) const;
    void operator()(const BlockStmt &stmt) const;
    void operator()(const ClassStmt &stmt) const;
    void operator()(const IfStmt &stmt) const;
    void operator()(const WhileStmt &stmt) const;
    void operator()([[maybe_unused]] const auto & /*unused*/) const;

    void executeBlock(const std::vector<Stmt> &statements, const Environment &env) const;

  private:
    Interpreter &m_interpreter;
  };

private:
  ExpressionVisitor m_expressionVisitor;
  StatementVisitor m_statementVisitor;

public:
  Interpreter(const std::vector<Stmt> &statements);

  void interpret() const;
  void resolve(const Expr &expr, const std::size_t depth);

  ExpressionVisitor &expressionVisitor();
  const ExpressionVisitor &expressionVisitor() const;

  StatementVisitor &statementVisitor();
  const StatementVisitor &statementVisitor() const;

private:
  std::any lookUpVariable(const Token &name, const Expr &expr) const;
};

}
