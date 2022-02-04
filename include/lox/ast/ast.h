#pragma once

#include "lox/token.h"

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

// https://craftinginterpreters.com/appendix-i.html

namespace lox {

struct AssignExpr;
struct BinaryExpr;
struct CallExpr;
struct GetExpr;
struct GroupingExpr;
struct LiteralExpr;
struct LogicalExpr;
struct SetExpr;
struct SuperExpr;
struct ThisExpr;
struct UnaryExpr;
struct VariableExpr;

using boost::recursive_wrapper;
using boost::variant;

// clang-format off
using Expr = variant<recursive_wrapper<AssignExpr>,   
                     recursive_wrapper<BinaryExpr>, 
                     recursive_wrapper<CallExpr>,     
                     recursive_wrapper<GetExpr>,
                     recursive_wrapper<GroupingExpr>, 
                     recursive_wrapper<LiteralExpr>, 
                     recursive_wrapper<LogicalExpr>,  
                     recursive_wrapper<SetExpr>,
                     recursive_wrapper<SuperExpr>,    
                     recursive_wrapper<ThisExpr>, 
                     recursive_wrapper<UnaryExpr>,    
                     recursive_wrapper<VariableExpr>>;
// clang-format on
struct AssignExpr {
  Token name;
  Expr value;

  AssignExpr(Token &name, Expr &val);
};

struct BinaryExpr {
  Expr left;
  Token op;
  Expr right;

  BinaryExpr(const Expr &left, const Token &op, const Expr &right);
};

struct CallExpr {
  Expr callee;
  Token paren;
  std::vector<Expr> arguments;

  CallExpr(Expr &callee, Token &paren, std::vector<Expr> &arguments);
};

struct GetExpr {
  Expr object;
  Token name;

  GetExpr(Expr &object, Token &name);
};

struct GroupingExpr {
  Expr expression;

  GroupingExpr(const Expr &expression);
};

struct LiteralExpr {
  Literal literal;

  LiteralExpr(const Literal &literal);
};

struct LogicalExpr {
  Expr left;
  Token op;
  Expr right;

  LogicalExpr(Expr &left, Token &op, Expr &right);
};

struct SetExpr {
  Expr obj;
  Token name;
  Expr val;

  SetExpr(Expr &obj, Token &name, Expr &val);
};

struct SuperExpr {
  Token keyword;
  Token method;

  SuperExpr(Token &keyword, Token &method);
};

struct ThisExpr {
  Token keyword;

  ThisExpr(Token &keyword);
};

struct UnaryExpr {
  Token op;
  Expr right;

  UnaryExpr(const Token &op, const Expr &right);
};

struct VariableExpr {
  Token name;

  VariableExpr(Token &name);
};

/////////////////////////////////////

struct BlockStmt;
struct ClassStmt;
struct ExpressionStmt;
struct FunctionStmt;
struct IfStmt;
struct PrintStmt;
struct ReturnStmt;
struct VarStmt;
struct WhileStmt;

// clang-format off
using Stmt = variant<recursive_wrapper<BlockStmt>, 
                     recursive_wrapper<ClassStmt>, 
                     recursive_wrapper<ExpressionStmt>, 
                     recursive_wrapper<FunctionStmt>,
                     recursive_wrapper<IfStmt>, 
                     recursive_wrapper<PrintStmt>, 
                     recursive_wrapper<ReturnStmt>, 
                     recursive_wrapper<VarStmt>,
                     recursive_wrapper<WhileStmt>>;
// clang-format on

struct BlockStmt {
  std::vector<Stmt> statements;

  BlockStmt(std::vector<Stmt> &statements);
};

struct ClassStmt {
  Token name;
  VariableExpr superClass;
  std::vector<FunctionStmt> methods;

  ClassStmt(Token &name, VariableExpr &superClass, std::vector<FunctionStmt> &methods);
};

struct ExpressionStmt {
  Expr expression;

  ExpressionStmt(Expr &expression);
};

struct FunctionStmt {
  Token name;
  std::vector<Token> params;
  std::vector<Stmt> body;

  FunctionStmt(Token &name, std::vector<Token> &params, std::vector<Stmt> &body);
};

struct IfStmt {
  Expr condition;
  Stmt thenBranch;
  Stmt elseBranch;

  IfStmt(Expr &condition, Stmt &thenBranch, Stmt &elseBranch);
};

struct PrintStmt {
  Expr expression;

  PrintStmt(Expr &expression);
};

struct ReturnStmt {
  Token keyword;
  Expr value;

  ReturnStmt(Token &keyword, Expr &value);
};

struct VarStmt {
  Token name;
  Expr initializer;

  VarStmt(Token &name, Expr &initializer);
};

struct WhileStmt {
  Expr condition;
  Stmt body;

  WhileStmt(Expr &condition, Stmt &body);
};

}
