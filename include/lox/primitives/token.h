#pragma once

#include "lox/primitives/literal.h"

#include <boost/functional/hash.hpp>

#include <cstdint>

namespace lox {
// clang-format off
enum class TokenKind : std::uint8_t {
    LeftParen, RightParen, LeftBrace, RightBrace,                               // parenthesis
    Equal, EqualEqual, Bang, BangEqual, Less, LessEqual, Greater, GreaterEqual, // comparison
    Plus, Minus, Star, Slash,                                                   // operators
    Dot, Comma, Semicolon,                                                      // dot
    String, Identifier, Number,                                                 // expressions
    And, Or, True, False,                                                       // boolean
    If, Else,                                                                   // condition
    For, While,                                                                 // loop
    Class, Super, Fun, Return, Var, Print, This,                                // keywords
    Nil, EndOfFile
  // clang-format on
};

struct Token {
  TokenKind kind;
  std::string lexeme;
  Literal literal;
  std::size_t line;

  bool operator==(const Token &) const = default;
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
