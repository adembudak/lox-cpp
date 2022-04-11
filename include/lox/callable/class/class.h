#pragma once

#include "lox/callable/function/function.h"
#include "lox/literal.h"

#include <string>
#include <vector>
#include <any>
#include <cstdint> // for std::size_t
#include <unordered_map>
#include <optional>

namespace lox {
class Interpreter;

class Class {
private:
  std::string m_name;
  std::unordered_map<std::string, Function> m_methods;

public:
  explicit Class(const std::string &name, const std::unordered_map<std::string, Function> &methods);

  std::optional<Function> findMethod(const std::string &name) const;
  std::any call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const;
  std::size_t arity() const;
  operator std::string() const;
};

}
