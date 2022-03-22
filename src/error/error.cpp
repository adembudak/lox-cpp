#include "lox/error/error.h"
#include "lox/token.h"

#include <exception>
#include <string_view>
#include <iostream>

namespace lox {

void report(const std::size_t line, const std::string_view where, const std::string_view message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
}

ParseError error(const Token &token, const std::string_view message) {
  if (token.kind == TokenKind::END_OF_FILE) {
    report(token.line, " at end", message);
  } else {
    report(token.line, " at '" + token.lexeme + "'", message);
  }

  return ParseError{};
}

void runtimeError(const RuntimeError &error) {
  std::cerr << error.what() << "\n[line " << error.token().line << "]\n";
}

const char *RuntimeError::what() const noexcept {
  return m_error_msg.c_str();
}

RuntimeError::RuntimeError(const Token &token, const std::string_view what_arg)
    : m_token{token}
    , m_error_msg(what_arg) {
}

Token RuntimeError::token() const {
  return m_token;
}

}
