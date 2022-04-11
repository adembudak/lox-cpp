#pragma once

#include <variant>
#include "lox/callable/class.h"
#include "lox/callable/function.h"
#include "lox/environment/environment.h"
#include "lox/interpreter/interpreter.h"

namespace lox {
template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

class Callable {
public:
  using callable_t = std::variant<std::monostate, Class, Function>;

private:
  callable_t m_callable;

public:
  Callable() = default;

  Callable(const Function &function)
      : m_callable(function) {
  }

  Callable(const Class &klass)
      : m_callable(klass) {
  }

  std::any call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const {
    return std::visit(overload{
                          [&](const std::monostate) { return std::any{nullptr}; },                        //
                          [&](const Class &klass) { return klass.call(interpreter, arguments); },         //
                          [&](const Function &function) { return function.call(interpreter, arguments); } //
                      },
                      m_callable);
  }

  std::size_t arity() const {
    return std::visit(overload{
                          [&](const std::monostate) { return std::size_t{0}; },      //
                          [&](const Class &klass) { return klass.arity(); },         //
                          [&](const Function &function) { return function.arity(); } //
                      },
                      m_callable);
  }

  operator std::string() const {
    return std::visit(overload{
                          [&](const std::monostate) { return std::string{}; },            //
                          [&](const Class &klass) { return std::string(klass); },         //
                          [&](const Function &function) { return std::string(function); } //
                      },
                      m_callable);
  }
};

}
