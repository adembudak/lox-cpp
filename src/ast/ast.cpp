#include "lox/token.h"
#include "lox/ast/ast.h"

#include <vector>

using namespace lox;

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

///////////////

BlockStmt::BlockStmt(const std::vector<Stmt> &statements)
    : statements(statements) {
}

ClassStmt::ClassStmt(const Token &name, const VariableExpr &superClass, const std::vector<FunctionStmt> &methods)
    : name(name)
    , superClass(superClass)
    , methods(methods) {
}

ExpressionStmt::ExpressionStmt(const Expr &expression)
    : expression(expression) {
}

FunctionStmt::FunctionStmt(const Token &name, const std::vector<Token> &params, const std::vector<Stmt> &body)
    : name(name)
    , params(params)
    , body(body) {
}

IfStmt::IfStmt(const Expr &condition, const Stmt &thenBranch, const Stmt &elseBranch)
    : condition(condition)
    , thenBranch(thenBranch)
    , elseBranch(elseBranch) {
}

PrintStmt::PrintStmt(const Expr &expression)
    : expression(expression) {
}

ReturnStmt::ReturnStmt(const Token &keyword, const Expr &value)
    : keyword(keyword)
    , value(value) {
}

VarStmt::VarStmt(const Token &name, const Expr &initializer)
    : name(name)
    , initializer(initializer) {
}

WhileStmt::WhileStmt(const Expr &condition, const Stmt &body)
    : condition(condition)
    , body(body) {
}
