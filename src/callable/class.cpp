#include "lox/literal.h"
#include "lox/callable/class.h"
#include "lox/callable/instance.h"
#include "lox/interpreter/interpreter.h"

#include <string>
#include <vector>
#include <any>

namespace lox {
Class::Class(const std::string &name)
    : m_name(name) {
}

std::any Class::call(const Interpreter &interpreter, const std::vector<Literal> &arguments) {
  (void)interpreter;
  (void)arguments;

  return Instance{*this};
}

std::size_t Class::arity() const {
  return 0;
}

Class::operator std::string() const {
  return m_name;
}

}
