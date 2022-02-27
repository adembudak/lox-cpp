#pragma once

#include "lox/token.h"

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

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

  AssignExpr(const Token &name, const Expr &val);
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

  CallExpr(const Expr &callee, const Token &paren, const std::vector<Expr> &arguments);
};

struct GetExpr {
  Expr object;
  Token name;

  GetExpr(const Expr &object, const Token &name);
};

struct GroupingExpr {
  Expr expression;

  explicit GroupingExpr(const Expr &expression);
};

struct LiteralExpr {
  Literal literal;

  explicit LiteralExpr(const Literal &literal);
};

struct LogicalExpr {
  Expr left;
  Token op;
  Expr right;

  LogicalExpr(const Expr &left, const Token &op, const Expr &right);
};

struct SetExpr {
  Expr obj;
  Token name;
  Expr val;

  SetExpr(const Expr &obj, const Token &name, const Expr &val);
};

struct SuperExpr {
  Token keyword;
  Token method;

  SuperExpr(const Token &keyword, const Token &method);
};

struct ThisExpr {
  Token keyword;

  explicit ThisExpr(const Token &keyword);
};

struct UnaryExpr {
  Token op;
  Expr right;

  UnaryExpr(const Token &op, const Expr &right);
};

struct VariableExpr {
  Token name;

  explicit VariableExpr(const Token &name);
};

}
