#include "lox/callable/instance.h"
#include <string>

namespace lox {

Instance::Instance(const Class &klass)
    : m_klass(klass){};

Instance::operator std::string() const {
  return std::string(m_klass).append(" instance");
}

}
