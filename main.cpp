#include "magic_enum.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <variant>
#include <vector>

class scanner_t {
private:
  // clang-format off
  enum class tokenType : std::uint8_t {
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
  };
  // clang-format on

  const std::map<std::string, tokenType> keywords{
      {"and", tokenType::AND},   {"class", tokenType::CLASS}, {"else", tokenType::ELSE},     {"false", tokenType::FALSE},
      {"for", tokenType::FOR},   {"fun", tokenType::FUN},     {"if", tokenType::IF},         {"nil", tokenType::NIL},
      {"or", tokenType::OR},     {"print", tokenType::PRINT}, {"return", tokenType::RETURN}, {"super", tokenType::SUPER},
      {"this", tokenType::THIS}, {"true", tokenType::TRUE},   {"var", tokenType::VAR},       {"while", tokenType::WHILE}};

public:
  struct token_t {
    using literal_t = std::variant<std::string, double>;

    tokenType m_type;
    literal_t m_lexeme;
    int m_line;
  };

  std::vector<token_t> m_tokens;
  int m_line = 1;
  std::size_t m_start = 0;
  std::size_t m_current = 0;
  std::string m_source;

public:
  scanner_t(const std::string source) : m_source(source) {}

  std::vector<token_t> scan() {

    // clang-format off
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

    auto isDigit = [&](const char c) 
    { return c >= '0' && c <= '9'; };

    auto isAlphaNumeric = [&](const char c) -> bool
    { return isAlpha(c)  || isDigit(c); };

    // clang-format on
    while (!isAtEnd()) {
      m_start = m_current;

      switch (const char c = advance(); c) {
        case '(':
          m_tokens.push_back(token_t{tokenType::LEFT_PAREN, {}, m_line});
          break;

        case ')':
          m_tokens.push_back(token_t{tokenType::RIGHT_PAREN, {}, m_line});
          break;

        case '{':
          m_tokens.push_back(token_t{tokenType::LEFT_BRACE, {}, m_line});
          break;

        case '}':
          m_tokens.push_back(token_t{tokenType::RIGHT_BRACE, {}, m_line});
          break;

        case '.':
          m_tokens.push_back(token_t{tokenType::DOT, {}, m_line});
          break;

        case ',':
          m_tokens.push_back(token_t{tokenType::COMMA, {}, m_line});
          break;

        case '-':
          m_tokens.push_back(token_t{tokenType::MINUS, {}, m_line});
          break;

        case '+':
          m_tokens.push_back(token_t{tokenType::PLUS, {}, m_line});
          break;

        case ';':
          m_tokens.push_back(token_t{tokenType::SEMICOLON, {}, m_line});
          break;

        case '*':
          m_tokens.push_back(token_t{tokenType::STAR, {}, m_line});
          break;

        case '!':
          m_tokens.push_back(token_t{match('=') ? tokenType::BANG_EQUAL : tokenType::BANG, {}, m_line});
          break;

        case '=':
          m_tokens.push_back(token_t{match('=') ? tokenType::EQUAL_EQUAL : tokenType::EQUAL, {}, m_line});
          break;

        case '<':
          m_tokens.push_back(token_t{match('=') ? tokenType::LESS_EQUAL : tokenType::LESS, {}, m_line});
          break;

        case '>':
          m_tokens.push_back(token_t{match('=') ? tokenType::GREATER_EQUAL : tokenType::GREATER, {}, m_line});
          break;

        case '/':
          if (match('/'))
            while (peek() != '\n' && !isAtEnd())
              advance();
          else
            m_tokens.push_back(token_t{tokenType::SLASH, {}, m_line});
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
          m_tokens.push_back(token_t{tokenType::STRING, m_source.substr(m_start + 1, m_current - m_start - 2), m_line});
          break;

        default:
          if (isDigit(c)) {
            while (isDigit(peek()))
              advance();

            if (peek() == '.' && isDigit(peekNext()))
              advance();

            while (isDigit(peek()))
              advance();

            m_tokens.push_back(token_t{tokenType::NUMBER, std::stod(m_source.substr(m_start, m_current - m_start)), m_line});
          }

          else if (isAlpha(c)) {
            while (isAlphaNumeric(peek()))
              advance();
            const std::string identifier = m_source.substr(m_start, m_current - m_start);
            tokenType type = keywords.at(identifier);
            m_tokens.push_back(token_t{tokenType::IDENTIFIER, identifier, m_line});
          } else {
            assert(false && "unexpected character");
          }
          break;
      }
    }

    m_tokens.push_back(token_t{tokenType::END_OF_FILE, {}, m_line});
    return m_tokens;
  }
};

int main(int argc, const char *argv[]) {
  const auto source = std::string(std::istream_iterator<char>(std::ifstream{argv[1]} >> std::noskipws), {});
  scanner_t scanner(source);
  auto tokens = scanner.scan();

  for (auto token : tokens) {
    std::cout << magic_enum::enum_name(token.m_type) << ' ';
    if (auto ret = std::get_if<std::string>(&token.m_lexeme))
      std::cout << *ret << ' ';
    else if (auto ret = std::get_if<double>(&token.m_lexeme))
      std::cout << *ret << ' ';

    std::cout << token.m_line << '\n';
  }
  std::cout << '\n';

  return 0;
}
