#pragma once

#include "lox/literal.h"

#include <string>
#include <vector>
#include <any>

namespace lox {
class Interpreter;

class Class {
private:
  std::string m_name;

public:
  explicit Class(const std::string &name);

  std::any call(const Interpreter &interpreter, std::vector<Literal> &arguments);

  std::size_t arity() const;

  operator std::string() const;
};

}
