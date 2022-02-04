#include "lox/prettyprint/prettyprint.h"
#include "lox/ast/ast.h"

#include <boost/variant/multivisitors.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <variant> // to visit Literal type
#include <sstream>

namespace lox {

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

std::string stringifyLexeme(const Literal &t) {
  // clang-format off
  return std::visit(overload {
               [](const std::string &s) { return s; },
               [](const double &d)      { return std::to_string(d); }
             }, t.value());
  // clang-format on
}

std::string parenthesize(const std::string &name, const std::initializer_list<const Expr> &expr);

struct PrintVisitor : public boost::static_visitor<std::string> {

  std::string operator()(const BinaryExpr &e) const {
    return parenthesize(stringifyLexeme(e.op.lexeme), {e.left, e.right});
  }

  std::string operator()(const AssignExpr &e) const {
    return "";
  }

  std::string operator()(const CallExpr &) const {
    return "";
  }

  std::string operator()(const GetExpr &) const {
    return "";
  }

  std::string operator()(const GroupingExpr &e) const {
    return parenthesize("group", {e.expression});
  }

  std::string operator()(const LiteralExpr &e) const {
    if (e.literal.has_value())
      return stringifyLexeme(e.literal);
    else
      return "nil";
  }

  std::string operator()(const LogicalExpr &) const {
    return "";
  }

  std::string operator()(const SetExpr &) const {
    return "";
  }

  std::string operator()(const SuperExpr &) const {
    return "";
  }

  std::string operator()(const ThisExpr &) const {
    return "";
  }

  std::string operator()(const UnaryExpr &e) const {
    return parenthesize(stringifyLexeme(e.op.lexeme), {e.right});
  }

  std::string operator()(const VariableExpr &) const {
    return "";
  }
  ///
};

std::string parenthesize(const std::string &name, const std::initializer_list<const Expr> &expr) {
  std::stringstream sout;

  sout << '(';
  sout << name;

  for (auto e : expr) {
    sout << ' ';
    sout << boost::apply_visitor(PrintVisitor(), e);
  }

  sout << ')';

  return sout.str();
}

void PrettyPrint(std::ostream &os, const Expr &expr) {
  os << boost::apply_visitor(PrintVisitor(), expr);
}
}
