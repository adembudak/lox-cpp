#pragma once

#include "lox/primitives/token.h"
#include "lox/ast/expr.h"

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

namespace lox {

struct BlockStmt;
struct ClassStmt;
struct ExpressionStmt;
struct FunctionStmt;
struct IfStmt;
struct PrintStmt;
struct ReturnStmt;
struct VariableStmt;
struct WhileStmt;

using boost::blank;
using boost::recursive_wrapper;
using boost::variant;

// clang-format off
using Stmt = variant<blank,
                     recursive_wrapper<BlockStmt>, 
                     recursive_wrapper<ClassStmt>, 
                     recursive_wrapper<ExpressionStmt>, 
                     recursive_wrapper<FunctionStmt>,
                     recursive_wrapper<IfStmt>, 
                     recursive_wrapper<PrintStmt>, 
                     recursive_wrapper<ReturnStmt>, 
                     recursive_wrapper<VariableStmt>,
                     recursive_wrapper<WhileStmt>>;
// clang-format on

struct BlockStmt {
  std::vector<Stmt> statements;

  explicit BlockStmt(const std::vector<Stmt> &statements);
};

struct ClassStmt {
  Token name;
  //  VariableExpr superClass;
  std::vector<FunctionStmt> methods;

  ClassStmt(const Token &name, /*const VariableExpr &superClass,*/ const std::vector<FunctionStmt> &methods);
};

struct ExpressionStmt {
  Expr expression;

  explicit ExpressionStmt(const Expr &expression);
};

struct FunctionStmt {
  Token name;
  std::vector<Token> params;
  std::vector<Stmt> body;

  FunctionStmt(const Token &name, const std::vector<Token> &params, const std::vector<Stmt> &body);
};

struct IfStmt {
  Expr condition;
  Stmt thenBranch;
  Stmt elseBranch;

  IfStmt(const Expr &condition, const Stmt &thenBranch, const Stmt &elseBranch);
};

struct PrintStmt {
  Expr expression;

  explicit PrintStmt(const Expr &expression);
};

struct ReturnStmt {
  Token keyword;
  Expr value;

  ReturnStmt(const Token &keyword, const Expr &value);
};

struct VariableStmt {
  Token name;
  Expr initializer;

  VariableStmt(const Token &name, const Expr &initializer);
};

struct WhileStmt {
  Expr condition;
  Stmt body;

  WhileStmt(const Expr &condition, const Stmt &body);
};

}
