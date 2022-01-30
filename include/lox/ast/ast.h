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

  BinaryExpr(Expr &left, Token &op, Expr &right);
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

  GroupingExpr(Expr &expression);
};

struct LiteralExpr {
  literal_t literal;

  LiteralExpr(literal_t &literal);
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

  UnaryExpr(Token &op, Expr &right);
};

struct VariableExpr {
  Token name;

  VariableExpr(Token &name);
};

}
