#include "lox/literal.h"
#include "lox/callable/return.h"

namespace lox {

Return::Return(const Literal &value)
    : m_value(value) {
}

Literal Return::value() const {
  return m_value;
}

}
