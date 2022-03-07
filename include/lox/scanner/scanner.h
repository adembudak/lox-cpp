#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "lox/token.h"

namespace lox {

class Scanner {
private:
  std::size_t m_line = 1;
  std::size_t m_start = 0;
  std::size_t m_current = 0;

  std::string m_source;
  std::vector<Token> m_tokens;

public:
  Scanner(const std::string &source);

  std::vector<Token> scan();

private:
  void addToken(const TokenKind kind);
  void addToken(const TokenKind kind, const Literal &literal);

  char peek() const;
  char peekNext() const;
  bool isAtEnd() const;
  char advance();
  bool match(const char expected);
  bool isAlpha(const char c) const;
  bool isDigit(const char c) const;
  bool isAlphaNumeric(const char c) const;
};

}
