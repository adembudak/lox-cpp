#include "lox/token.h"
#include "lox/ast/ast.h"
#include "lox/prettyprint/prettyprint.h"

#include <boost/variant/apply_visitor.hpp>

#include <variant>
#include <string>
#include <vector>
#include <sstream>

namespace lox {

using Parts = std::variant<Expr, std::vector<Expr>, Token, Literal, Stmt>;

std::string parenthesize(const std::string &name, const std::initializer_list<const Expr> &expr);
std::string parenthesize(const std::string &name, const std::initializer_list<const Parts> &parts);

struct ExprVisitor : public boost::static_visitor<std::string> {

  std::string operator()(const BinaryExpr &e) const {
    return parenthesize(to_string(e.op.lexeme), {e.left, e.right});
  }

  std::string operator()(const AssignExpr &e) const {
    return parenthesize("=", {e.name.lexeme, e.value});
  }

  std::string operator()(const CallExpr &e) const {
    return parenthesize("call", {e.callee, e.arguments});
  }

  std::string operator()(const GetExpr &e) const {
    return parenthesize(".", {e.object, e.name.lexeme});
  }

  std::string operator()(const GroupingExpr &e) const {
    return parenthesize("group", {e.expression});
  }

  std::string operator()(const LiteralExpr &e) const {
    if (e.literal.index() != 0)
      return to_string(e.literal);
    else
      return "nil";
  }

  std::string operator()(const LogicalExpr &e) const {
    return parenthesize(to_string(e.op.lexeme), {e.left, e.right});
  }

  std::string operator()(const SetExpr &e) const {
    return parenthesize("=", {e.obj, e.name.lexeme, e.val});
  }

  std::string operator()(const SuperExpr &e) const {
    return parenthesize("super", {e.method});
  }

  std::string operator()([[maybe_unused]] const ThisExpr &e) const {
    return "this";
  }

  std::string operator()(const UnaryExpr &e) const {
    return parenthesize(to_string(e.op.lexeme), {e.right});
  }

  std::string operator()(const VariableExpr &e) const {
    return to_string(e.name.lexeme);
  }

  std::string operator()(const boost::blank &s) const {
    return std::string{};
  }
};

struct StmtVisitor : public boost::static_visitor<std::string> {

  std::string operator()(const BlockStmt &stmt) const {
    std::ostringstream sout;
    sout << "(block ";

    for (const auto &stmt : stmt.statements) {
      sout << boost::apply_visitor(StmtVisitor(), stmt);
    }

    sout << ")";
    return sout.str();
  }

  std::string operator()(const ClassStmt &stmt) const {
    std::ostringstream sout;
    sout << "(class " << to_string(stmt.name.lexeme);

    if (stmt.superClass.name.lexeme.index() != 0) {
      sout << " < " << to_string(stmt.superClass.name.lexeme);
    }

    // revisit: why `const auto &` doesn't work here?
    for (const Stmt method : stmt.methods) {
      sout << ' ';
      sout << boost::apply_visitor(StmtVisitor(), method);
    }

    sout << ")";
    return sout.str();
  }

  std::string operator()(const ExpressionStmt &stmt) const {
    return parenthesize(";", {stmt.expression});
  }

  std::string operator()(const FunctionStmt &stmt) const {
    std::ostringstream sout;

    sout << "(fun ";
    sout << to_string(stmt.name.lexeme);
    sout << "( ";

    for (const Token &param : stmt.params) {
      sout << to_string(param.lexeme);
    }

    sout << ") ";

    for (const Stmt &body : stmt.body) {
      sout << boost::apply_visitor(StmtVisitor(), body);
    }
    sout << ')';

    return sout.str();
  }

  std::string operator()(const IfStmt &stmt) const {
    if (stmt.elseBranch.empty()) {
      return parenthesize("if", {stmt.condition, stmt.thenBranch});
    }
    return parenthesize("if-else", {stmt.condition, stmt.thenBranch, stmt.thenBranch});
  }

  std::string operator()(const PrintStmt &stmt) const {
    return parenthesize("print", {stmt.expression});
  }

  std::string operator()(const ReturnStmt &stmt) const {
    if (stmt.value.empty()) {
      return "(return)";
    }
    return parenthesize("return", {stmt.value});
  }

  std::string operator()(const VarStmt &stmt) const {
    if (stmt.initializer.empty()) {
      return parenthesize("var", {stmt.name});
    }
    return parenthesize("var", {stmt.name, "=", stmt.initializer});
  }

  std::string operator()(const WhileStmt &stmt) const {
    return parenthesize("while", {stmt.condition, stmt.body});
  }

  std::string operator()(const boost::blank &s) const {
    return std::string{};
  }
};

std::string parenthesize(const std::string &name, const std::initializer_list<const Expr> &expr) {
  std::stringstream sout;

  sout << '(';
  sout << name;

  for (const auto &e : expr) {
    sout << ' ';
    sout << boost::apply_visitor(ExprVisitor(), e);
  }

  sout << ')';

  return sout.str();
}

std::string parenthesize(const std::string &name, const std::initializer_list<const Parts> &parts) {
  std::stringstream sout;

  sout << '(';
  sout << name;

  for (const auto &part : parts) {
    sout << ' ';

    if (const auto *pE = std::get_if<Expr>(&part)) {
      sout << boost::apply_visitor(ExprVisitor(), *pE);
    }

    else if (const auto *pS = std::get_if<Stmt>(&part)) {
      sout << boost::apply_visitor(StmtVisitor(), *pS);
    }

    else if (const auto *pEV = std::get_if<std::vector<Expr>>(&part)) {
      for (const auto &expr : *pEV) {
        sout << boost::apply_visitor(ExprVisitor(), expr);
      }
    }

    else if (const auto *pT = std::get_if<Token>(&part)) {
      sout << to_string(pT->lexeme);
    }

    else if (const auto *pL = std::get_if<Literal>(&part)) {
    }
  }

  sout << ')';

  return sout.str();
}

void PrettyPrint(std::ostream &os, const Expr &expr) {
  os << boost::apply_visitor(ExprVisitor(), expr);
}

}
