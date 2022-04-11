#pragma once

#include "lox/callable/class/class.h"
#include "lox/callable/function/function.h"

#include <variant>
#include <cstdint>
#include <string>

namespace lox {

class Class;
class Function;

class Callable {
public:
  using callable_t = std::variant<std::monostate, Class, Function>;

private:
  callable_t m_callable;

public:
  Callable() = default;
  Callable(const Class &klass);
  Callable(const Function &function);

  std::any call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const;
  std::size_t arity() const;
  operator std::string() const;
};

}
