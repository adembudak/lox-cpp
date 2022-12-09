#pragma once

#include "lox/primitives/token.h"
#include "lox/primitives/literal.h"

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

  bool operator==(const AssignExpr &) const = default;
};

struct BinaryExpr {
  Expr left;
  Token op;
  Expr right;

  bool operator==(const BinaryExpr &) const = default;
};

struct CallExpr {
  Expr callee;
  Token paren;
  std::vector<Expr> arguments;

  bool operator==(const CallExpr &) const = default;
};

struct GetExpr {
  Expr object;
  Token name;

  bool operator==(const GetExpr &) const = default;
};

struct GroupingExpr {
  Expr expression;

  bool operator==(const GroupingExpr &) const = default;
};

struct LiteralExpr {
  Literal literal;

  bool operator==(const LiteralExpr &) const = default;
};

struct LogicalExpr {
  Expr left;
  Token op;
  Expr right;

  bool operator==(const LogicalExpr &) const = default;
};

struct SetExpr {
  Expr object;
  Token name;
  Expr value;

  bool operator==(const SetExpr &) const = default;
};

struct SuperExpr {
  Token keyword;
  Token method;

  bool operator==(const SuperExpr &) const = default;
};

struct ThisExpr {
  Token keyword;

  bool operator==(const ThisExpr &) const = default;
};

struct UnaryExpr {
  Token op;
  Expr right;

  bool operator==(const UnaryExpr &) const = default;
};

struct VariableExpr {
  Token name;

  bool operator==(const VariableExpr &) const = default;
};

inline std::size_t hash_value(const AssignExpr &expr) {
  std::size_t h = hash_value(expr.name);
  boost::hash_combine(h, hash_value(expr.value));
  return h;
}

inline std::size_t hash_value(const BinaryExpr &expr) {
  std::size_t h = hash_value(expr.left);
  boost::hash_combine(h, hash_value(expr.op));
  boost::hash_combine(h, hash_value(expr.right));
  return h;
}

inline std::size_t hash_value(const CallExpr &expr) {
  std::size_t h = hash_value(expr.callee);
  boost::hash_combine(h, hash_range(expr.arguments.begin(), expr.arguments.end()));
  boost::hash_combine(h, expr.paren);
  return h;
}

inline std::size_t hash_value(const GetExpr &expr) {
  std::size_t h = hash_value(expr.name);
  boost::hash_combine(h, expr.object);
  return h;
}

inline std::size_t hash_value(const GroupingExpr &expr) {
  return hash_value(expr.expression);
}

inline std::size_t hash_value(const LiteralExpr &expr) {
  return hash_value(expr.literal);
}

inline std::size_t hash_value(const LogicalExpr &expr) {
  std::size_t h = hash_value(expr.left);
  boost::hash_combine(h, hash_value(expr.op));
  boost::hash_combine(h, hash_value(expr.right));
  return h;
}

inline std::size_t hash_value(const SetExpr &expr) {
  std::size_t h = hash_value(expr.name);
  boost::hash_combine(h, hash_value(expr.object));
  boost::hash_combine(h, hash_value(expr.value));
  return h;
}

inline std::size_t hash_value(const SuperExpr &expr) {
  std::size_t h = hash_value(expr.keyword);
  boost::hash_combine(h, hash_value(expr.method));
  return h;
}

inline std::size_t hash_value(const ThisExpr &expr) {
  return hash_value(expr.keyword);
}

inline std::size_t hash_value(const UnaryExpr &expr) {
  std::size_t h = hash_value(expr.op);
  boost::hash_combine(h, hash_value(expr.right));
  return h;
}

inline std::size_t hash_value(const VariableExpr &expr) {
  return hash_value(expr.name);
}

}
