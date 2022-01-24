#include "magic_enum.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace Lox {

// clang-format off
enum class tokenKind : std::uint8_t {
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

using literal_t = std::optional<std::variant<std::string, double>>;

struct Token {
  tokenKind m_kind;
  literal_t m_lexeme;
  std::size_t m_line;
};

class Scanner {
private:
  const std::map<std::string, tokenKind> keywords{
      {"and", tokenKind::AND},   {"class", tokenKind::CLASS}, {"else", tokenKind::ELSE},     {"false", tokenKind::FALSE},
      {"for", tokenKind::FOR},   {"fun", tokenKind::FUN},     {"if", tokenKind::IF},         {"nil", tokenKind::NIL},
      {"or", tokenKind::OR},     {"print", tokenKind::PRINT}, {"return", tokenKind::RETURN}, {"super", tokenKind::SUPER},
      {"this", tokenKind::THIS}, {"true", tokenKind::TRUE},   {"var", tokenKind::VAR},       {"while", tokenKind::WHILE}};

public:
  std::size_t m_line = 1;
  std::size_t m_start = 0;
  std::size_t m_current = 0;

  std::string m_source;
  std::vector<Token> m_tokens;

public:
  Scanner(const std::string &source)
      : m_source(source) {}

  // clang-format off
  std::vector<Token> scan() {

    auto peek = [&]() -> char 
    { return m_source.at(m_current); };

    auto peekNext = [&]() -> char 
    { return m_current + 1 >= m_source.size() ? '\0' : m_source.at(m_current + 1); };

    auto isAtEnd = [&]() -> bool 
    { return m_current + 1 >= m_source.size(); };

    auto advance = [&]() -> char 
    { return m_source.at(m_current++); };

    auto match = [&](const char expected) -> bool 
    { if (isAtEnd()) return false;
      if (m_source.at(m_current) != expected) return false;

      ++m_current;
      return true; };

    auto isAlpha = [&](const char c) -> bool 
    { return (c >= 'a' && c <= 'z') || 
             (c >= 'A' && c <= 'Z') || 
              c == '_'; };

    auto isDigit = [&](const char c) -> bool
    { return c >= '0' && c <= '9'; };

    auto isAlphaNumeric = [&](const char c) -> bool
    { return isAlpha(c) || isDigit(c); };

    // clang-format on
    while (!isAtEnd()) {
      m_start = m_current;

      switch (const char c = advance(); c) {
        case '(':
          m_tokens.push_back(Token{tokenKind::LEFT_PAREN, std::nullopt, m_line});
          break;

        case ')':
          m_tokens.push_back(Token{tokenKind::RIGHT_PAREN, std::nullopt, m_line});
          break;

        case '{':
          m_tokens.push_back(Token{tokenKind::LEFT_BRACE, std::nullopt, m_line});
          break;

        case '}':
          m_tokens.push_back(Token{tokenKind::RIGHT_BRACE, std::nullopt, m_line});
          break;

        case '.':
          m_tokens.push_back(Token{tokenKind::DOT, std::nullopt, m_line});
          break;

        case ',':
          m_tokens.push_back(Token{tokenKind::COMMA, std::nullopt, m_line});
          break;

        case '-':
          m_tokens.push_back(Token{tokenKind::MINUS, std::nullopt, m_line});
          break;

        case '+':
          m_tokens.push_back(Token{tokenKind::PLUS, std::nullopt, m_line});
          break;

        case ';':
          m_tokens.push_back(Token{tokenKind::SEMICOLON, std::nullopt, m_line});
          break;

        case '*':
          m_tokens.push_back(Token{tokenKind::STAR, std::nullopt, m_line});
          break;

        case '!':
          m_tokens.push_back(Token{match('=') ? tokenKind::BANG_EQUAL : tokenKind::BANG, std::nullopt, m_line});
          break;

        case '=':
          m_tokens.push_back(Token{match('=') ? tokenKind::EQUAL_EQUAL : tokenKind::EQUAL, std::nullopt, m_line});
          break;

        case '<':
          m_tokens.push_back(Token{match('=') ? tokenKind::LESS_EQUAL : tokenKind::LESS, std::nullopt, m_line});
          break;

        case '>':
          m_tokens.push_back(Token{match('=') ? tokenKind::GREATER_EQUAL : tokenKind::GREATER, std::nullopt, m_line});
          break;

        case '/':
          if (match('/'))
            while (peek() != '\n' && !isAtEnd())
              advance();
          else
            m_tokens.push_back(Token{tokenKind::SLASH, std::nullopt, m_line});
          break;

        case ' ':
          [[fallthrough]];
        case '\r':
          [[fallthrough]];
        case '\t':
          break;

        case '\n':
          ++m_line;
          break;

        case '"':
          while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n')
              ++m_line;
            advance();
          }

          if (isAtEnd())
            assert(false && "unterminated string");

          advance();
          m_tokens.push_back(Token{tokenKind::STRING, m_source.substr(m_start + 1, m_current - m_start - 2), m_line});
          break;

        default:
          if (isDigit(c)) {
            while (isDigit(peek()))
              advance();

            if (peek() == '.' && isDigit(peekNext()))
              advance();

            while (isDigit(peek()))
              advance();

            m_tokens.push_back(Token{tokenKind::NUMBER, std::stod(m_source.substr(m_start, m_current - m_start)), m_line});
          }

          else if (isAlpha(c)) {
            while (isAlphaNumeric(peek()))
              advance();
            const std::string identifier = m_source.substr(m_start, m_current - m_start);
            tokenKind type = keywords.at(identifier);
            m_tokens.push_back(Token{tokenKind::IDENTIFIER, identifier, m_line});
          } else {
            assert(false && "unexpected character");
          }
          break;
      }
    }

    m_tokens.push_back(Token{tokenKind::END_OF_FILE, std::nullopt, m_line});
    return m_tokens;
  }

  void trace() {
    if (m_tokens.empty())
      return;

    for (auto token : m_tokens) {
      std::cout << token.m_line << '\t';
      std::cout << magic_enum::enum_name(token.m_kind) << ' ';

      if (token.m_lexeme.has_value()) {
        if (auto ret = std::get_if<std::string>(&token.m_lexeme.value()))
          std::cout << *ret << ' ';
        else if (auto ret = std::get_if<double>(&token.m_lexeme.value()))
          std::cout << *ret << ' ';
      }

      std::cout << '\n';
    }
  }
};

} // end namespace Lox

int main(int argc, const char *argv[]) {
  if (argc < 2)
    return 1;

  const auto source = std::string(std::istream_iterator<char>(std::ifstream{argv[1]} >> std::noskipws), {});

  Lox::Scanner scanner(source);
  const auto tokens = scanner.scan();

  scanner.trace();

  return 0;
}
