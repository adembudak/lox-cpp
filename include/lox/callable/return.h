#pragma once

#include "lox/literal.h"
#include <exception>

namespace lox {

class Return : public std::exception {
private:
  Literal m_value;

public:
  explicit Return(const Literal &value);
  Literal value() const;
};

}
