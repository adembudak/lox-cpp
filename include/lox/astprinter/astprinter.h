#pragma once

#include "lox/ast/expr.h"
#include "lox/ast/stmt.h"

#include <string>
#include <vector>
#include <iosfwd>
#include <initializer_list>

namespace lox {

class ASTPrinter {
private:
  std::vector<Stmt> m_statements;

public:
  explicit ASTPrinter(const std::vector<Stmt> &statements)
      : m_statements(statements)
      , m_astvisitor(*this) {
  }

  void print(std::ostream &os);

private:
  struct ASTVisitor : public boost::static_visitor<std::string> {

    explicit ASTVisitor(ASTPrinter &astprinter)
        : m_astprinter(astprinter) {
    }

    std::string visit(const Stmt &stmt) const;

    std::string operator()(const BlockStmt &stmt) const;
    std::string operator()(const ClassStmt &stmt) const;
    std::string operator()(const ExpressionStmt &stmt) const;
    std::string operator()(const FunctionStmt &stmt) const;
    std::string operator()(const IfStmt &stmt) const;
    std::string operator()(const PrintStmt &stmt) const;
    std::string operator()(const ReturnStmt &stmt) const;
    std::string operator()(const VarStmt &stmt) const;
    std::string operator()(const WhileStmt &stmt) const;

    std::string visit(const Expr &expr) const;

    std::string operator()(const AssignExpr &expr) const;
    std::string operator()(const BinaryExpr &expr) const;
    std::string operator()(const CallExpr &expr) const;
    std::string operator()(const GetExpr &expr) const;
    std::string operator()(const GroupingExpr &expr) const;
    std::string operator()(const LiteralExpr &expr) const;
    std::string operator()(const LogicalExpr &expr) const;
    std::string operator()(const SetExpr &expr) const;
    std::string operator()(const SuperExpr &expr) const;
    std::string operator()(const ThisExpr &expr) const;
    std::string operator()(const UnaryExpr &expr) const;
    std::string operator()(const VariableExpr &expr) const;

    std::string operator()(const auto & /*unused*/) const {
      return {};
    }

  private:
    ASTPrinter &m_astprinter;
  };

private:
  ASTVisitor m_astvisitor;
};

}
