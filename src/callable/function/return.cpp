#include "lox/primitives/literal.h"
#include "lox/callable/function/return.h"

namespace lox {

Return::Return(const Literal &value)
    : m_value(value) {
}

Literal Return::value() const {
  return m_value;
}

}
