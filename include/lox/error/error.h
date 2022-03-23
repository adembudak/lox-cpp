#include "lox/token.h"

#include <exception>
#include <string_view>
#include <string>

namespace lox {
class ParseError;
class RuntimeError;

void report(const std::size_t line, const std::string_view where, const std::string_view message);
void error(const std::size_t line, const std::string_view message);
ParseError error(const Token &token, const std::string_view message);
void runtimeError(const RuntimeError &error);

class RuntimeError : public std::exception {
private:
  Token m_token;
  std::string m_error_msg;

public:
  [[nodiscard]] explicit RuntimeError(const Token &token, const std::string_view what_arg);
  virtual const char *what() const noexcept override;
  Token token() const;
};

class ParseError : public std::exception {};
}
