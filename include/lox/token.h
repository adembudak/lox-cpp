#pragma once

#include "lox/literal.h"

#include <boost/functional/hash.hpp>

#include <cstdint>

namespace lox {
// clang-format off
enum class TokenKind : std::uint8_t {
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,                               // parenthesis
    EQUAL, EQUAL_EQUAL, BANG, BANG_EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, // comparison
    PLUS, MINUS, STAR, SLASH,                                                       // operators
    DOT, COMMA, SEMICOLON,                                                          // dot
    STRING, IDENTIFIER, NUMBER,                                                     // expressions
    AND, OR, TRUE, FALSE,                                                           // boolean
    IF, ELSE,                                                                       // condition
    FOR, WHILE,                                                                     // loop
    CLASS, SUPER, FUN, RETURN, VAR, PRINT, THIS,                                    // keywords
    NIL, END_OF_FILE
  // clang-format on
};

struct Token {
  TokenKind kind;
  std::string lexeme;
  Literal literal;
  std::size_t line;

  bool operator==(const Token &) const = default;
  bool operator!=(const Token &) const = default;
};

inline std::size_t hash_value(const Token &token) {
  std::size_t h = 0;
  boost::hash_combine(h, token.kind);
  boost::hash_combine(h, token.lexeme);
  boost::hash_combine(h, hash_value(token.literal));
  boost::hash_combine(h, token.line);
  return h;
}

}
