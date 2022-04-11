#include "lox/callable/class/instance.h"
#include "lox/token.h"
#include "lox/error/error.h"

#include <string>

namespace lox {

Instance::Instance(const Class &klass)
    : m_klass(klass){};

std::any Instance::get(const Token &name) {
  if (m_fields.contains(name.lexeme))
    return m_fields.find(name.lexeme)->second;

  if (const auto method = m_klass.findMethod(name.lexeme))
    return *method;

  throw RuntimeError(name, std::string("Undefined property '").append(name.lexeme).append("'."));
}

void Instance::set(const Token &name, const std::any &val) {
  m_fields[name.lexeme] = val;
}

Instance::operator std::string() const {
  return std::string(m_klass).append(" instance");
}

}
