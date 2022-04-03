#pragma once

#include <string>

namespace lox {

class Class {
private:
  std::string m_name;

public:
  explicit Class(const std::string &name);
};

}
