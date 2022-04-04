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

std::any Class::call(const Interpreter &interpreter, std::vector<std::any> &arguments) {
  (void)interpreter;
  (void)arguments;

  return Instance{*this};
}

}
