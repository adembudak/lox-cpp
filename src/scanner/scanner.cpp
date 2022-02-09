#include "lox/token.h"
#include "lox/scanner/scanner.h"

#include <map>
#include <string>
#include <string_view>
#include <iostream>
#include <cstddef>
#include <variant>
#include <vector>

/*
 Lox lexical grammar
 -------------------
 NUMBER     -> DIGIT+ ( "." DIGIT+ )? ;
 STRING     -> "\"" <any char except "\"">* "\"" ;
 IDENTIFIER -> ALPHA ( ALPHA | DIGIT )* ;
 ALPHA      -> "a" ... "z" | "A" ... "Z" | "_" ;
 DIGIT      -> "0" ... "9" ;
*/

namespace {

void report(std::size_t line, const std::string_view where, const std::string_view message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
}

void error(const std::size_t line, const std::string_view message) {
  report(line, "", message);
}

}

namespace lox {

const std::map<std::string, TokenKind> keywords{
    {"and", TokenKind::AND},   {"class", TokenKind::CLASS}, {"else", TokenKind::ELSE},     {"false", TokenKind::FALSE},
    {"for", TokenKind::FOR},   {"fun", TokenKind::FUN},     {"if", TokenKind::IF},         {"nil", TokenKind::NIL},
    {"or", TokenKind::OR},     {"print", TokenKind::PRINT}, {"return", TokenKind::RETURN}, {"super", TokenKind::SUPER},
    {"this", TokenKind::THIS}, {"true", TokenKind::TRUE},   {"var", TokenKind::VAR},       {"while", TokenKind::WHILE}};

Scanner::Scanner(const std::string &source)
    : m_source(source) {
}

std::vector<Token> Scanner::scan() {
  while (!isAtEnd()) {
    m_start = m_current;

    switch (const char c = advance(); c) {
      case '(':
        m_tokens.push_back(Token{TokenKind::LEFT_PAREN, noopt, m_line});
        break;

      case ')':
        m_tokens.push_back(Token{TokenKind::RIGHT_PAREN, noopt, m_line});
        break;

      case '{':
        m_tokens.push_back(Token{TokenKind::LEFT_BRACE, noopt, m_line});
        break;

      case '}':
        m_tokens.push_back(Token{TokenKind::RIGHT_BRACE, noopt, m_line});
        break;

      case '.':
        m_tokens.push_back(Token{TokenKind::DOT, noopt, m_line});
        break;

      case ',':
        m_tokens.push_back(Token{TokenKind::COMMA, noopt, m_line});
        break;

      case '-':
        m_tokens.push_back(Token{TokenKind::MINUS, noopt, m_line});
        break;

      case '+':
        m_tokens.push_back(Token{TokenKind::PLUS, noopt, m_line});
        break;

      case ';':
        m_tokens.push_back(Token{TokenKind::SEMICOLON, noopt, m_line});
        break;

      case '*':
        m_tokens.push_back(Token{TokenKind::STAR, noopt, m_line});
        break;

      case '!':
        m_tokens.push_back(Token{match('=') ? TokenKind::BANG_EQUAL : TokenKind::BANG, noopt, m_line});
        break;

      case '=':
        m_tokens.push_back(Token{match('=') ? TokenKind::EQUAL_EQUAL : TokenKind::EQUAL, noopt, m_line});
        break;

      case '<':
        m_tokens.push_back(Token{match('=') ? TokenKind::LESS_EQUAL : TokenKind::LESS, noopt, m_line});
        break;

      case '>':
        m_tokens.push_back(Token{match('=') ? TokenKind::GREATER_EQUAL : TokenKind::GREATER, noopt, m_line});
        break;

      case '/':
        if (match('/'))
          while (peek() != '\n' && !isAtEnd())
            advance();
        else
          m_tokens.push_back(Token{TokenKind::SLASH, noopt, m_line});
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

        if (isAtEnd()) {
          error(m_line, "unterminated string");
        }

        advance();
        m_tokens.push_back(Token{TokenKind::STRING, m_source.substr(m_start + 1, m_current - m_start - 2), m_line});
        break;

      default:
        if (isDigit(c)) {
          while (isDigit(peek()))
            advance();

          if (peek() == '.' && isDigit(peekNext()))
            advance();

          while (isDigit(peek()))
            advance();

          m_tokens.push_back(Token{TokenKind::NUMBER, std::stod(m_source.substr(m_start, m_current - m_start)), m_line});
        }

        else if (isAlpha(c)) {
          while (isAlphaNumeric(peek()))
            advance();
          const std::string identifier = m_source.substr(m_start, m_current - m_start);
          TokenKind type = keywords.at(identifier);
          m_tokens.push_back(Token{TokenKind::IDENTIFIER, identifier, m_line});
        } else {
          error(m_line, "unexpected character");
        }
        break;
    }
  }

  m_tokens.push_back(Token{TokenKind::END_OF_FILE, noopt, m_line});
  return m_tokens;
}

char Scanner::peek() const {
  return m_source.at(m_current);
}

char Scanner::peekNext() const {
  return m_current + 1 >= m_source.size() ? '\0' : m_source.at(m_current + 1);
}

bool Scanner::isAtEnd() const {
  return m_current + 1 >= m_source.size();
}

char Scanner::advance() {
  return m_source.at(m_current++);
}

bool Scanner::match(const char expected) {
  if (isAtEnd())
    return false;
  if (m_source.at(m_current) != expected)
    return false;

  ++m_current;
  return true;
}

bool Scanner::isAlpha(const char c) const {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isDigit(const char c) const {
  return c >= '0' && c <= '9';
}

bool Scanner::isAlphaNumeric(const char c) const {
  return isAlpha(c) || isDigit(c);
}

}
