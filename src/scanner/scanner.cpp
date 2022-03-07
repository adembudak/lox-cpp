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

void report(const std::size_t line, const std::string_view where, const std::string_view message) {
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
        addToken(TokenKind::LEFT_PAREN);
        break;

      case ')':
        addToken(TokenKind::RIGHT_PAREN);
        break;

      case '{':
        addToken(TokenKind::LEFT_BRACE);
        break;

      case '}':
        addToken(TokenKind::RIGHT_BRACE);
        break;

      case '.':
        addToken(TokenKind::DOT);
        break;

      case ',':
        addToken(TokenKind::COMMA);
        break;

      case '-':
        addToken(TokenKind::MINUS);
        break;

      case '+':
        addToken(TokenKind::PLUS);
        break;

      case ';':
        addToken(TokenKind::SEMICOLON);
        break;

      case '*':
        addToken(TokenKind::STAR);
        break;

      case '!':
        addToken(match('=') ? TokenKind::BANG_EQUAL : TokenKind::BANG);
        break;

      case '=':
        addToken(match('=') ? TokenKind::EQUAL_EQUAL : TokenKind::EQUAL);
        break;

      case '<':
        addToken(match('=') ? TokenKind::LESS_EQUAL : TokenKind::LESS);
        break;

      case '>':
        addToken(match('=') ? TokenKind::GREATER_EQUAL : TokenKind::GREATER);
        break;

      case '/':
        if (match('/'))
          while (peek() != '\n' && !isAtEnd())
            advance();
        else
          addToken(TokenKind::SLASH);
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

      case '"': {
        while (peek() != '"' && !isAtEnd()) {
          if (peek() == '\n')
            ++m_line;
          advance();
        }

        if (isAtEnd()) {
          error(m_line, "unterminated string");
        }

        advance();

        const std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
        addToken(TokenKind::STRING, value);
      } break;

      default:
        if (isDigit(c)) {
          while (isDigit(peek()))
            advance();

          if (peek() == '.' && isDigit(peekNext()))
            advance();

          while (isDigit(peek()))
            advance();

          const double value = std::stod(m_source.substr(m_start, m_current - m_start));
          addToken(TokenKind::NUMBER, value);
        }

        else if (isAlpha(c)) {
          while (isAlphaNumeric(peek())) {
            advance();
          }

          const std::string text = m_source.substr(m_start, m_current - m_start);
          const TokenKind kind = keywords.contains(text) ? keywords.find(text)->second : TokenKind::IDENTIFIER;
          addToken(kind);
        }

        else {
          error(m_line, "unexpected character");
        }
        break;
    }
  }

  addToken(TokenKind::END_OF_FILE);
  return m_tokens;
}

void Scanner::addToken(const TokenKind kind) {
  addToken(kind, nullptr);
}

void Scanner::addToken(const TokenKind kind, const Literal &literal) {
  const std::string text = m_source.substr(m_start, m_current - m_start);
  m_tokens.push_back(Token{kind, text, literal, m_line});
}

char Scanner::peek() const {
  return m_source[m_current];
}

char Scanner::peekNext() const {
  return m_current + 1 >= m_source.size() ? '\0' : m_source[m_current + 1];
}

bool Scanner::isAtEnd() const {
  return m_current + 1 > m_source.size();
}

char Scanner::advance() {
  return m_source[m_current++];
}

bool Scanner::match(const char expected) {
  if (isAtEnd())
    return false;
  if (m_source[m_current] != expected)
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
