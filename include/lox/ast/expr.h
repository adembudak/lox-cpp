#pragma once

#include "lox/token.h"
#include "lox/literal.h"

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

namespace boost {
inline std::size_t hash_value(const blank /*unused*/) {
  return 0;
}
}

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

using boost::blank;
using boost::recursive_wrapper;
using boost::variant;

// clang-format off
using Expr = variant<blank,
                     recursive_wrapper<AssignExpr>,   
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

  bool operator==(const AssignExpr &) const = default;
  bool operator!=(const AssignExpr &) const = default;
};

struct BinaryExpr {
  Expr left;
  Token op;
  Expr right;

  BinaryExpr(const Expr &left, const Token &op, const Expr &right);

  bool operator==(const BinaryExpr &) const = default;
  bool operator!=(const BinaryExpr &) const = default;
};

struct CallExpr {
  Expr callee;
  Token paren;
  std::vector<Expr> arguments;

  CallExpr(const Expr &callee, const Token &paren, const std::vector<Expr> &arguments);

  bool operator==(const CallExpr &) const = default;
  bool operator!=(const CallExpr &) const = default;
};

struct GetExpr {
  Expr object;
  Token name;

  GetExpr(const Expr &object, const Token &name);

  bool operator==(const GetExpr &) const = default;
  bool operator!=(const GetExpr &) const = default;
};

struct GroupingExpr {
  Expr expression;

  explicit GroupingExpr(const Expr &expression);

  bool operator==(const GroupingExpr &) const = default;
  bool operator!=(const GroupingExpr &) const = default;
};

struct LiteralExpr {
  Literal literal;

  explicit LiteralExpr(const Literal &literal);

  bool operator==(const LiteralExpr &) const = default;
  bool operator!=(const LiteralExpr &) const = default;
};

struct LogicalExpr {
  Expr left;
  Token op;
  Expr right;

  LogicalExpr(const Expr &left, const Token &op, const Expr &right);

  bool operator==(const LogicalExpr &) const = default;
  bool operator!=(const LogicalExpr &) const = default;
};

struct SetExpr {
  Expr object;
  Token name;
  Expr value;

  SetExpr(const Expr &object, const Token &name, const Expr &val);

  bool operator==(const SetExpr &) const = default;
  bool operator!=(const SetExpr &) const = default;
};

struct SuperExpr {
  Token keyword;
  Token method;

  SuperExpr(const Token &keyword, const Token &method);

  bool operator==(const SuperExpr &) const = default;
  bool operator!=(const SuperExpr &) const = default;
};

struct ThisExpr {
  Token keyword;

  explicit ThisExpr(const Token &keyword);

  bool operator==(const ThisExpr &) const = default;
  bool operator!=(const ThisExpr &) const = default;
};

struct UnaryExpr {
  Token op;
  Expr right;

  UnaryExpr(const Token &op, const Expr &right);

  bool operator==(const UnaryExpr &) const = default;
  bool operator!=(const UnaryExpr &) const = default;
};

struct VariableExpr {
  Token name;

  explicit VariableExpr(const Token &name);

  bool operator==(const VariableExpr &) const = default;
  bool operator!=(const VariableExpr &) const = default;
};

std::size_t hash_value(const AssignExpr &expr);
std::size_t hash_value(const BinaryExpr &expr);
std::size_t hash_value(const CallExpr &expr);
std::size_t hash_value(const GetExpr &expr);
std::size_t hash_value(const GroupingExpr &expr);
std::size_t hash_value(const LiteralExpr &expr);
std::size_t hash_value(const LogicalExpr &expr);
std::size_t hash_value(const SetExpr &expr);
std::size_t hash_value(const SuperExpr &expr);
std::size_t hash_value(const ThisExpr &expr);
std::size_t hash_value(const UnaryExpr &expr);
std::size_t hash_value(const VariableExpr &expr);

}
