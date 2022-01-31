#include "lox/parser/parser.h"
#include "lox/token.h"

#include <initializer_list>
#include <algorithm>

using namespace lox;

Token Parser::peek() const {
  return m_tokens[m_current];
}

bool Parser::isAtEnd() const {
  return peek().m_kind == tokenKind::END_OF_FILE;
}

Token Parser::previous() const {
  return m_tokens[m_current - 1];
}

Token Parser::advance() {
  if (!isAtEnd()) {
    ++m_current;
  }
  return previous();
}

bool Parser::check(const tokenKind t) const {
  if (isAtEnd())
    return false;
  return peek().m_kind == t;
}

bool Parser::match(std::initializer_list<tokenKind> tokens) {
  bool isThereAnMatch = std::any_of(tokens.begin(), tokens.end(), [&](const auto token) { return check(token); });

  if (isThereAnMatch) {
    advance();
  }
  return isThereAnMatch;
}
