#pragma once
#include <unordered_map>
#include <any>

#include "lox/callable/class.h"

namespace lox {
struct Token;

class Instance {
private:
  std::unordered_map<std::string, std::any> m_fields;
  Class m_klass;

public:
  explicit Instance(const Class &klass);

  std::any get(const Token &name);
  void set(const Token &name, const std::any &val);
  operator std::string() const;
};

}
