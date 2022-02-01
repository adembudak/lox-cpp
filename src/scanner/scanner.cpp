#include "lox/token.h"
#include "lox/scanner/scanner.h"

#include <magic_enum.hpp>
#include <map>
#include <string>
#include <cassert>
#include <iostream>

/*
 Lox lexical grammar
 -------------------
 NUMBER     -> DIGIT+ ( "." DIGIT+ )? ;
 STRING     -> "\"" <any char except "\"">* "\"" ;
 IDENTIFIER -> ALPHA ( ALPHA | DIGIT )* ;
 ALPHA      -> "a" ... "z" | "A" ... "Z" | "_" ;
 DIGIT      -> "0" ... "9" ;
*/

using namespace lox;

const std::map<std::string, tokenKind> keywords{
    {"and", tokenKind::AND},   {"class", tokenKind::CLASS}, {"else", tokenKind::ELSE},     {"false", tokenKind::FALSE},
    {"for", tokenKind::FOR},   {"fun", tokenKind::FUN},     {"if", tokenKind::IF},         {"nil", tokenKind::NIL},
    {"or", tokenKind::OR},     {"print", tokenKind::PRINT}, {"return", tokenKind::RETURN}, {"super", tokenKind::SUPER},
    {"this", tokenKind::THIS}, {"true", tokenKind::TRUE},   {"var", tokenKind::VAR},       {"while", tokenKind::WHILE}};

Scanner::Scanner(const std::string &source)
    : m_source(source) {
}

std::vector<Token> Scanner::scan() {
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
