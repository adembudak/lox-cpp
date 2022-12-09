#include "lox/primitives/token.h"
#include "lox/primitives/literal.h"
#include "lox/ast/expr.h"

#include <boost/functional/hash.hpp>
#include <vector>

namespace lox {

AssignExpr::AssignExpr(const Token &name, const Expr &val)
    : name(name)
    , value(val) {}

BinaryExpr::BinaryExpr(const Expr &left, const Token &op, const Expr &right)
    : left(left)
    , op(op)
    , right(right) {}

CallExpr::CallExpr(const Expr &callee, const Token &paren, const std::vector<Expr> &arguments)
    : callee(callee)
    , paren(paren)
    , arguments(arguments) {}

GetExpr::GetExpr(const Expr &object, const Token &name)
    : object(object)
    , name(name) {}

GroupingExpr::GroupingExpr(const Expr &expression)
    : expression(expression) {}

LiteralExpr::LiteralExpr(const Literal &literal)
    : literal(literal) {}

LogicalExpr::LogicalExpr(const Expr &left, const Token &op, const Expr &right)
    : left(left)
    , op(op)
    , right(right) {}

SetExpr::SetExpr(const Expr &object, const Token &name, const Expr &val)
    : object(object)
    , name(name)
    , value(val) {}

SuperExpr::SuperExpr(const Token &keyword, const Token &method)
    : keyword(keyword)
    , method(method) {}

ThisExpr::ThisExpr(const Token &keyword)
    : keyword(keyword) {}

UnaryExpr::UnaryExpr(const Token &op, const Expr &right)
    : op(op)
    , right(right) {}

VariableExpr::VariableExpr(const Token &name)
    : name(name) {}

std::size_t hash_value(const AssignExpr &expr) {
  std::size_t h = hash_value(expr.name);
  boost::hash_combine(h, hash_value(expr.value));
  return h;
}

std::size_t hash_value(const BinaryExpr &expr) {
  std::size_t h = hash_value(expr.left);
  boost::hash_combine(h, hash_value(expr.op));
  boost::hash_combine(h, hash_value(expr.right));
  return h;
}

std::size_t hash_value(const CallExpr &expr) {
  std::size_t h = hash_value(expr.callee);
  boost::hash_combine(h, hash_range(expr.arguments.begin(), expr.arguments.end()));
  boost::hash_combine(h, expr.paren);
  return h;
}

std::size_t hash_value(const GetExpr &expr) {
  std::size_t h = hash_value(expr.name);
  boost::hash_combine(h, expr.object);
  return h;
}

std::size_t hash_value(const GroupingExpr &expr) {
  return hash_value(expr.expression);
}

std::size_t hash_value(const LiteralExpr &expr) {
  return hash_value(expr.literal);
}

std::size_t hash_value(const LogicalExpr &expr) {
  std::size_t h = hash_value(expr.left);
  boost::hash_combine(h, hash_value(expr.op));
  boost::hash_combine(h, hash_value(expr.right));
  return h;
}

std::size_t hash_value(const SetExpr &expr) {
  std::size_t h = hash_value(expr.name);
  boost::hash_combine(h, hash_value(expr.object));
  boost::hash_combine(h, hash_value(expr.value));
  return h;
}

std::size_t hash_value(const SuperExpr &expr) {
  std::size_t h = hash_value(expr.keyword);
  boost::hash_combine(h, hash_value(expr.method));
  return h;
}

std::size_t hash_value(const ThisExpr &expr) {
  return hash_value(expr.keyword);
}

std::size_t hash_value(const UnaryExpr &expr) {
  std::size_t h = hash_value(expr.op);
  boost::hash_combine(h, hash_value(expr.right));
  return h;
}

std::size_t hash_value(const VariableExpr &expr) {
  return hash_value(expr.name);
}

}
