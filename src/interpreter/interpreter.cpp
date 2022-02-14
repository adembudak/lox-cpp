#include "lox/interpreter/interpreter.h"
#include "lox/token.h"
#include "lox/ast/ast.h"

#include <boost/variant/static_visitor.hpp>
#include <variant>

namespace lox {

bool isTruthy(const Values &values) {
  // clang-format off
  return std::visit(overload {
           [](const std::nullptr_t) { return false; },
           [](const bool ) { return true; },
           [](const double ) { return true; },
           [](const std::string &) { return true; }
      }, values);
  // clang-format on
}

bool operator==(const Values &left, const Values &right) {
  if (left.index() == 0 && right.index() == 0)
    return true;
  if (left.index() == 0 && right.index() != 0)
    return false;
  return left == right;
}

bool operator!=(const Values &left, const Values &right) {
  return !(left == right);
}

struct InterpreterVisitor : public boost::static_visitor<Values> {
  Values operator()(const LiteralExpr &expr) const {
    return to_string(expr.literal);
  }

  Values operator()(const GroupingExpr &expr) const {
    return boost::apply_visitor(InterpreterVisitor(), expr.expression);
  }

  Values operator()(const UnaryExpr &expr) const {
    Values right = boost::apply_visitor(InterpreterVisitor(), expr.right);

    switch (expr.op.kind) {
      using enum TokenKind;

      case MINUS:
        return -std::get<double>(right);

      case BANG:
        return isTruthy(right);

      default:;
    }

    return nullptr;
  }

  Values operator()(const BinaryExpr &expr) const {
    Values left = boost::apply_visitor(InterpreterVisitor(), expr.left);
    Values right = boost::apply_visitor(InterpreterVisitor(), expr.right);

    switch (expr.op.kind) {
      using enum TokenKind;

      case PLUS: {
        if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
          return std::get<double>(left) + std::get<double>(right);

        if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
          return std::get<std::string>(left) + std::get<std::string>(right);
      }

      case MINUS:
        return std::get<double>(left) - std::get<double>(right);

      case SLASH:
        return std::get<double>(left) / std::get<double>(right);

      case STAR:
        return std::get<double>(left) * std::get<double>(right);

      case GREATER:
        return std::get<double>(left) > std::get<double>(right);

      case GREATER_EQUAL:
        return std::get<double>(left) >= std::get<double>(right);

      case LESS:
        return std::get<double>(left) < std::get<double>(right);

      case LESS_EQUAL:
        return std::get<double>(left) <= std::get<double>(right);

      case EQUAL_EQUAL:
        return left == right;

      case BANG_EQUAL:
        return left != right;

      default:
        break;
    }

    return nullptr;
  }

  Values operator()([[maybe_unused]] const auto & /*unused*/) const {
    return {};
  }
};

Interpreter::Interpreter(const Expr &expr)
    : expr(expr) {
}

Values Interpreter::interpret() const {
  return boost::apply_visitor(InterpreterVisitor(), expr);
}

}
