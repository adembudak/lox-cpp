#pragma once

#include "lox/primitives/token.h"
#include "lox/primitives/literal.h"

#include <unordered_map>
#include <memory>
#include <any>

namespace lox {

class Environment : public std::enable_shared_from_this<Environment> {
private:
  std::unordered_map<std::string, std::any> m_values;
  std::shared_ptr<Environment> m_enclosing = nullptr;

public:
  Environment() = default;
  explicit Environment(const std::shared_ptr<Environment> &enclosing);

  void define(const Token &token, const std::any &value);

  std::any get(const Token &token) const;
  std::any getAt(const Token &token, const std::size_t distance) const;

  void assign(const Token &token, const std::any &value);
  void assignAt(const Token &token, const std::size_t distance, const std::any &value);

private:
  bool isGlobalEnvironment() const;
  std::shared_ptr<Environment> ancestor(const std::size_t distance);
  std::shared_ptr<const Environment> ancestor(const std::size_t distance) const;
};

}
