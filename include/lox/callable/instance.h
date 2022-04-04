#pragma once
#include "lox/callable/class.h"

namespace lox {

class Instance {
private:
  Class m_klass;

public:
  explicit Instance(const Class &klass);
};

}
