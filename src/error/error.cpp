#include "lox/error/error.h"
#include "lox/token.h"

#include <fmt/core.h>
#include <fmt/color.h>

#include <exception>
#include <string_view>

namespace lox {

void report(const std::size_t line, const std::string_view where, const std::string_view message) {
  if (!where.empty())
    fmt::print(stderr, fg(fmt::color::red), "[line {0}] Error{1}: {2}\n", line, where, message);
  else
    fmt::print(stderr, fg(fmt::color::red), "[line {0}] Error: {1}\n", line, message);
}

void error(const std::size_t line, const std::string_view message) {
  report(line, std::string{}, message);
}

ParseError error(const Token &token, const std::string_view message) {
  if (token.kind == TokenKind::EndOfFile) {
    report(token.line, " at end", message);
  } else {
    report(token.line, " at '" + token.lexeme + "'", message);
  }

  return ParseError{};
}

void runtimeError(const RuntimeError &error) {
  fmt::print(stderr, fg(fmt::color::red), "{0}\n[line {1}]\n", error.what(), error.token().line);
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
