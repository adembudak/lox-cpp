#include "lox/token.h"
#include "lox/ast/expr.h"

#include <vector>

namespace lox {

AssignExpr::AssignExpr(const Token &name, const Expr &val)
    : name(name)
    , value(val) {
}

BinaryExpr::BinaryExpr(const Expr &left, const Token &op, const Expr &right)
    : left(left)
    , op(op)
    , right(right) {
}

CallExpr::CallExpr(const Expr &callee, const Token &paren, const std::vector<Expr> &arguments)
    : callee(callee)
    , paren(paren)
    , arguments(arguments) {
}

GetExpr::GetExpr(const Expr &object, const Token &name)
    : object(object)
    , name(name) {
}

GroupingExpr::GroupingExpr(const Expr &expression)
    : expression(expression) {
}

LiteralExpr::LiteralExpr(const Literal &literal)
    : literal(literal) {
}

LogicalExpr::LogicalExpr(const Expr &left, const Token &op, const Expr &right)
    : left(left)
    , op(op)
    , right(right) {
}

SetExpr::SetExpr(const Expr &obj, const Token &name, const Expr &val)
    : obj(obj)
    , name(name)
    , val(val) {
}

SuperExpr::SuperExpr(const Token &keyword, const Token &method)
    : keyword(keyword)
    , method(method) {
}

ThisExpr::ThisExpr(const Token &keyword)
    : keyword(keyword) {
}

UnaryExpr::UnaryExpr(const Token &op, const Expr &right)
    : op(op)
    , right(right) {
}

VariableExpr::VariableExpr(const Token &name)
    : name(name) {
}

}
