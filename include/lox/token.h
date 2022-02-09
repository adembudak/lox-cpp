#pragma once

#include <cstdint>
#include <optional>
#include <variant>
#include <string>

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

using Literal = std::variant<std::monostate, std::string, double>;
constexpr const std::monostate noopt;

struct Token {
  TokenKind kind;
  Literal lexeme;
  std::size_t line;
};

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

inline std::string to_string(const lox::Literal &t) {
  // clang-format off
  return std::visit(overload {
               [](const std::monostate) { return std::string{}; },
               [](const std::string &s) { return s; },
               [](const double &d)      { return std::to_string(d); }
             }, t);
  // clang-format on
}

}
