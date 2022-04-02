#pragma once

#include "lox/ast/stmt.h"
#include "lox/interpreter/interpreter.h"

#include <boost/variant/static_visitor.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace lox {

class Resolver : public boost::static_visitor<void> {
private:
  enum class FunctionKind : std::uint8_t { None, Function, Initializer, Method };
  enum class ClassKind : std::uint8_t { None, Class, Subclass };

  FunctionKind m_currentFunction = FunctionKind::None;
  ClassKind m_currentClass = ClassKind::None;

  std::vector<Stmt> m_statements;
  std::vector<std::unordered_map<std::string, bool>> m_scopes;

  Interpreter m_interpreter;

public:
  explicit Resolver(const Interpreter &interpreter);
  void resolve(const std::vector<Stmt> &statements);

  void resolve(const Stmt &stmt);

  void operator()(const BlockStmt &stmt);
  void operator()(const ExpressionStmt &stmt);
  void operator()(const FunctionStmt &stmt);
  void operator()(const IfStmt &stmt);
  void operator()(const PrintStmt &stmt);
  void operator()(const ReturnStmt &stmt);
  void operator()(const VariableStmt &stmt);
  void operator()(const WhileStmt &stmt);

  void resolve(const Expr &expr);

  void operator()(const AssignExpr &expr);
  void operator()(const BinaryExpr &expr);
  void operator()(const CallExpr &expr);
  void operator()(const GroupingExpr &expr);
  void operator()(const LiteralExpr &expr);
  void operator()(const LogicalExpr &expr);
  void operator()(const UnaryExpr &expr);
  void operator()(const VariableExpr &expr);

  void operator()(const auto & /*unused*/);

private:
  void beginScope();
  void endScope();

  void declare(const Token &token);
  void define(const Token &token);

  void resolveLocal(const Expr &expr, const Token &name);
  void resolveFunction(const FunctionStmt &function);
};

}
