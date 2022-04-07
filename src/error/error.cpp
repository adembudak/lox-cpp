#include "lox/error/error.h"
#include "lox/token.h"

#include <fmt/core.h>
#include <fmt/color.h>

#include <exception>
#include <string_view>

namespace lox {

void report(const std::size_t line, const std::string_view message) {
  const std::string line_message = fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "[line {}]:", line);
  fmt::print(stderr, fg(fmt::color::red), "{0} Error: {1}\n", line_message, message);
}

void report(const std::size_t line, const std::string_view where, const std::string_view message) {
  const std::string line_message = fmt::format(fmt::emphasis::bold | fg(fmt::color::red), "[line {}]:", line);
  const std::string where_message = fmt::format(fmt::emphasis::bold | fg(fmt::color::ghost_white), "{}", where);
  fmt::print(stderr, fg(fmt::color::red), "{0} Error at `{1}`: {2}\n", line_message, where_message, message);
}

void error(const std::size_t line, const std::string_view message) {
  report(line, message);
}

ParseError error(const Token &token, const std::string_view message) {
  if (token.kind == TokenKind::EndOfFile) {
    report(token.line, " at end", message);
  } else {
    report(token.line, token.lexeme, message);
  }

  return ParseError{};
}

void runtimeError(const RuntimeError &error) {
  report(error.token().line, error.what());
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
