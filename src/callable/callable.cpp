#include "lox/callable/callable.h"
#include "lox/environment/environment.h"
#include "lox/interpreter/interpreter.h"

#include <variant>
#include <cstdint>
#include <string>

namespace {

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

}

namespace lox {

Callable::Callable(const Class &klass)
    : m_callable(klass) {}

Callable::Callable(const Function &function)
    : m_callable(function) {}

std::any Callable::call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const {
  return std::visit(overload{
                        [&](const std::monostate) { return std::any{}; },                               //
                        [&](const Class &klass) { return klass.call(interpreter, arguments); },         //
                        [&](const Function &function) { return function.call(interpreter, arguments); } //
                    },
                    m_callable);
}

std::size_t Callable::arity() const {
  return std::visit(overload{
                        [&](const std::monostate) { return std::size_t{0}; },      //
                        [&](const Class &klass) { return klass.arity(); },         //
                        [&](const Function &function) { return function.arity(); } //
                    },
                    m_callable);
}

Callable::operator std::string() const {
  return std::visit(overload{
                        [&](const std::monostate) { return std::string{}; },            //
                        [&](const Class &klass) { return std::string(klass); },         //
                        [&](const Function &function) { return std::string(function); } //
                    },
                    m_callable);
}

}
