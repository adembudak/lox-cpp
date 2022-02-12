#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include "lox/token.h"
#include "lox/scanner/scanner.h"

TEST_CASE("Test individual elements") {
  using namespace lox;

  SECTION("Parenthesis") {
    const std::string input = R"((
                                  ))";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].kind == TokenKind::LEFT_PAREN);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::RIGHT_PAREN);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Braces") {
    const std::string input = R"({

                                  })";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].kind == TokenKind::LEFT_BRACE);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::RIGHT_BRACE);
    REQUIRE(tokens[1].line == 3);

    REQUIRE(tokens[2].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Dot") {
    const std::string input = ".";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::DOT);
    REQUIRE(tokens[1].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Comma") {
    const std::string input = ",";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::COMMA);
    REQUIRE(tokens[1].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Minus") {
    const std::string input = "-";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::MINUS);
    REQUIRE(tokens[1].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Plus") {
    const std::string input = "+";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::PLUS);
    REQUIRE(tokens[1].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Semicolon") {
    const std::string input = ";";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::SEMICOLON);
    REQUIRE(tokens[1].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Star") {
    const std::string input = "*";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::STAR);
    REQUIRE(tokens[1].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Bang, Bang Equal") {
    const std::string input = R"(!
                                 !=)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].kind == TokenKind::BANG);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::BANG_EQUAL);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::END_OF_FILE);
    REQUIRE(tokens[1].line == 2);
  }

  SECTION("Equal, Equal Equal") {
    const std::string input = R"(=
                                 ==)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].kind == TokenKind::EQUAL);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::EQUAL_EQUAL);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::END_OF_FILE);
    REQUIRE(tokens[1].line == 2);
  }

  SECTION("Less, Less Equal") {
    const std::string input = R"(<
                                 <=)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].kind == TokenKind::LESS);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::LESS_EQUAL);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::END_OF_FILE);
    REQUIRE(tokens[1].line == 2);
  }

  SECTION("Greater, Greater Equal") {
    const std::string input = R"(>
                                 >=)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].kind == TokenKind::GREATER);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::GREATER_EQUAL);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::END_OF_FILE);
    REQUIRE(tokens[1].line == 2);
  }

  SECTION("Comment") {
    const std::string input = R"(//comment
                                 // comment
                                )";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].kind == TokenKind::END_OF_FILE);
  }

  SECTION("White Space") {
    const std::string input = R"( 


                                       )";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].kind == TokenKind::END_OF_FILE);
    REQUIRE(tokens[0].line == 4);
  }
}
