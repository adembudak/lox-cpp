#include "lox/primitives/literal.h"
#include "lox/callable/class/class.h"
#include "lox/callable/class/instance.h"
#include "lox/interpreter/interpreter.h"

#include <string>
#include <vector>
#include <any>

namespace lox {

Class::Class(const std::string &name, const std::unordered_map<std::string, Function> &methods)
    : m_name(name)
    , m_methods(methods) {}

std::optional<Function> Class::findMethod(const std::string &name) const {
  if (m_methods.contains(name))
    return m_methods.find(name)->second;
  return std::nullopt;
}

std::any Class::call(const Interpreter &interpreter, const std::vector<Literal> &arguments) const {
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
