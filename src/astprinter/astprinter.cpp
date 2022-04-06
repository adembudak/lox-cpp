#include "lox/astprinter/astprinter.h"

#include <boost/variant/apply_visitor.hpp>

#include <sstream>
#include <string>
#include <tuple> // for std::ignore

namespace lox {

void ASTPrinter::print(std::ostream &os) {
  for (const auto &stmt : m_statements)
    os << visit(stmt);
  os << '\n';
}

std::string ASTPrinter::visit(const Stmt &stmt) const {
  return boost::apply_visitor(ASTPrinter(), stmt);
}

std::string ASTPrinter::operator()(const BlockStmt &stmt) const {
  std::ostringstream sout;
  sout << '(';

  for (sout << "block "; const auto &statement : stmt.statements) {
    sout << visit(statement);
  }

  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const VariableStmt &stmt) const {
  std::ostringstream sout;
  sout << '(';

  if (sout << "var "; stmt.initializer.which() == 0)
    sout << stmt.name.lexeme;
  else
    sout << stmt.name.lexeme << '=' << visit(stmt.initializer);

  sout << ')';
  return sout.str();
}

std::string ASTPrinter::visit(const Expr &expr) const {
  return boost::apply_visitor(ASTPrinter(), expr);
}

std::string ASTPrinter::operator()(const ClassStmt &stmt) const {
  std::ostringstream sout;
  sout << '(';
  sout << "class ";

  //  if (stmt.superClass.name.literal != nullptr)
  //    sout << " < " + visit(stmt.superClass);

  for (const FunctionStmt &method : stmt.methods)
    sout << ' ' << visit(method);

  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const ExpressionStmt &stmt) const {
  std::ostringstream sout;
  sout << '(' << "; " << visit(stmt.expression) << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const FunctionStmt &stmt) const {
  std::ostringstream sout;
  sout << '(';
  sout << "fun " << stmt.name.lexeme;

  sout << '(';
  for (const Token &param : stmt.params)
    sout << ' ' << param.lexeme;
  sout << ')';

  for (sout << ' '; const Stmt &body : stmt.body)
    sout << visit(body);

  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const IfStmt &stmt) const {
  std::ostringstream sout;
  sout << '(';

  if (stmt.elseBranch.which() == 0)
    sout << "if " << visit(stmt.condition) << ' ' << visit(stmt.thenBranch);
  else
    sout << "if-else " << visit(stmt.condition) << ' ' << visit(stmt.thenBranch) << ' ' << visit(stmt.elseBranch);

  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const PrintStmt &stmt) const {
  std::ostringstream sout;
  sout << '(';
  sout << "print " << visit(stmt.expression);
  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const ReturnStmt &stmt) const {
  if (stmt.value.which() == 0)
    return "(return)";

  std::ostringstream sout;
  sout << '(';
  sout << "return " << visit(stmt.value);
  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const WhileStmt &stmt) const {
  std::ostringstream sout;
  sout << '(';
  sout << "while " << visit(stmt.condition) << ' ' << visit(stmt.body);
  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const AssignExpr &expr) const {
  std::ostringstream sout;
  sout << '(';
  sout << "= " << expr.name.lexeme << ' ' << visit(expr.value);
  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const BinaryExpr &expr) const {
  std::ostringstream sout;
  sout << '(';
  sout << expr.op.lexeme << ' ' << visit(expr.left) << ' ' << visit(expr.right);
  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const CallExpr &expr) const {
  std::ostringstream sout;
  sout << '(';
  sout << "call " << visit(expr.callee);

  for (sout << ' '; const Expr &arg : expr.arguments)
    sout << visit(arg) << ' ';

  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const GetExpr &expr) const {
  std::ostringstream sout;
  sout << '(' << ". " << visit(expr.object) << ' ' << expr.name.lexeme << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const GroupingExpr &expr) const {
  std::ostringstream sout;
  sout << '(' << "group " << visit(expr.expression) << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const LiteralExpr &expr) const {
  return std::string(expr.literal);
}

std::string ASTPrinter::operator()(const LogicalExpr &expr) const {
  std::ostringstream sout;
  sout << '(';
  sout << expr.op.lexeme << ' ' << visit(expr.left) << ' ' << visit(expr.right);
  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const SetExpr &expr) const {
  std::ostringstream sout;
  sout << '(' << "= " << visit(expr.object) << ' ' << expr.name.lexeme << ' ' << visit(expr.value) << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const SuperExpr &expr) const {
  std::ostringstream sout;
  sout << '(' << "super " << expr.method.lexeme << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const ThisExpr &expr) const {
  std::ignore = expr;
  return "this";
}

std::string ASTPrinter::operator()(const UnaryExpr &expr) const {
  std::ostringstream sout;
  sout << '(';
  sout << expr.op.lexeme << ' ' << visit(expr.right);
  sout << ')';
  return sout.str();
}

std::string ASTPrinter::operator()(const VariableExpr &expr) const {
  return expr.name.lexeme;
}

std::string ASTPrinter::operator()(const auto & /*unused*/) const {
  // "sink" function. First member of the Stmt and Expr variants is boost::blank, this makes good default when
  // declared and assigned later. Visitors must be exhaustive and operator()() must be implemented for
  // every variant member. This member is for completeness.
  return {};
}

}
