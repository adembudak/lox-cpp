#pragma once

#include "lox/token.h"
#include "lox/literal.h"

#include <unordered_map>
#include <memory>
#include <any>

namespace lox {

class Environment : public std::enable_shared_from_this<Environment> {
private:
  std::unordered_map<std::string, std::any> m_values;

private:
  std::shared_ptr<Environment> m_enclosing = nullptr;

public:
  Environment() = default;
  explicit Environment(const std::shared_ptr<Environment> &enclosing);

  bool isGlobalEnvironment() const {
    return m_enclosing == nullptr;
  }

  void define(const std::string &name, const std::any &val);
  std::any get(const Token &token) const;
  void assign(const Token &token, const std::any &value);

private:
  std::shared_ptr<Environment> ancestor(const int distance);
  void assignAt(const int distance, const Token &token, const std::any &value);
};

}
