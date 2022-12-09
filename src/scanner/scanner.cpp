#include "lox/primitives/token.h"
#include "lox/scanner/scanner.h"
#include "lox/error/error.h"

#include <map>
#include <string>
#include <string_view>
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

namespace lox {

const std::map<std::string, TokenKind> keywords{
    {"and", TokenKind::And},   {"class", TokenKind::Class}, {"else", TokenKind::Else},     {"false", TokenKind::False},
    {"for", TokenKind::For},   {"fun", TokenKind::Fun},     {"if", TokenKind::If},         {"nil", TokenKind::Nil},
    {"or", TokenKind::Or},     {"print", TokenKind::Print}, {"return", TokenKind::Return}, {"super", TokenKind::Super},
    {"this", TokenKind::This}, {"true", TokenKind::True},   {"var", TokenKind::Var},       {"while", TokenKind::While}};

Scanner::Scanner(const std::string &source)
    : m_source(source) {}

std::vector<Token> Scanner::scan() {
  while (!isAtEnd()) {
    m_start = m_current;

    switch (const char c = advance(); c) {
      case '(':
        addToken(TokenKind::LeftParen);
        break;

      case ')':
        addToken(TokenKind::RightParen);
        break;

      case '{':
        addToken(TokenKind::LeftBrace);
        break;

      case '}':
        addToken(TokenKind::RightBrace);
        break;

      case '.':
        addToken(TokenKind::Dot);
        break;

      case ',':
        addToken(TokenKind::Comma);
        break;

      case '-':
        addToken(TokenKind::Minus);
        break;

      case '+':
        addToken(TokenKind::Plus);
        break;

      case ';':
        addToken(TokenKind::Semicolon);
        break;

      case '*':
        addToken(TokenKind::Star);
        break;

      case '!':
        addToken(match('=') ? TokenKind::BangEqual : TokenKind::Bang);
        break;

      case '=':
        addToken(match('=') ? TokenKind::EqualEqual : TokenKind::Equal);
        break;

      case '<':
        addToken(match('=') ? TokenKind::LessEqual : TokenKind::Less);
        break;

      case '>':
        addToken(match('=') ? TokenKind::GreaterEqual : TokenKind::Greater);
        break;

      case '/':
        if (match('/'))
          while (peek() != '\n' && !isAtEnd())
            advance();
        else
          addToken(TokenKind::Slash);
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
        addToken(TokenKind::String, value);
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
          addToken(TokenKind::Number, value);
        }

        else if (isAlpha(c)) {
          while (isAlphaNumeric(peek())) {
            advance();
          }

          const std::string text = m_source.substr(m_start, m_current - m_start);
          const TokenKind kind = keywords.contains(text) ? keywords.find(text)->second : TokenKind::Identifier;
          addToken(kind);
        }

        else {
          error(m_line, "unexpected character");
        }
        break;
    }
  }

  addToken(TokenKind::EndOfFile);
  return m_tokens;
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
