#include "lox/token.h"
#include "lox/ast/ast.h"

#include <vector>

using namespace lox;

AssignExpr::AssignExpr(Token &name, Expr &val)
    : name(name)
    , value(val) {
}

BinaryExpr::BinaryExpr(Expr &left, Token &op, Expr &right)
    : left(left)
    , op(op)
    , right(right) {
}

CallExpr::CallExpr(Expr &callee, Token &paren, std::vector<Expr> &arguments)
    : callee(callee)
    , paren(paren)
    , arguments(arguments) {
}

GetExpr::GetExpr(Expr &object, Token &name)
    : object(object)
    , name(name) {
}

GroupingExpr::GroupingExpr(Expr &expression)
    : expression(expression) {
}

LiteralExpr::LiteralExpr(literal_t &literal)
    : literal(literal) {
}

LogicalExpr::LogicalExpr(Expr &left, Token &op, Expr &right)
    : left(left)
    , op(op)
    , right(right) {
}

SetExpr::SetExpr(Expr &obj, Token &name, Expr &val)
    : obj(obj)
    , name(name)
    , val(val) {
}

SuperExpr::SuperExpr(Token &keyword, Token &method)
    : keyword(keyword)
    , method(method) {
}

ThisExpr::ThisExpr(Token &keyword)
    : keyword(keyword) {
}

UnaryExpr::UnaryExpr(Token &op, Expr &right)
    : op(op)
    , right(right) {
}

VariableExpr::VariableExpr(Token &name)
    : name(name) {
}
