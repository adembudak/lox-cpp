#include "lox/token.h"
#include "lox/ast/stmt.h"

namespace lox {

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

VariableStmt::VariableStmt(const Token &name, const Expr &initializer)
    : name(name)
    , initializer(initializer) {
}

WhileStmt::WhileStmt(const Expr &condition, const Stmt &body)
    : condition(condition)
    , body(body) {
}

}
