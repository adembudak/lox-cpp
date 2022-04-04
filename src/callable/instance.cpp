#include "lox/callable/instance.h"

namespace lox {

Instance::Instance(const Class &klass)
    : m_klass(klass){};

}
