#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include "lox/token.h"
#include "lox/scanner/scanner.h"

TEST_CASE("Test individual elements") {
  using namespace lox;

  SECTION("Parenthesis") {
    const std::string braces = R"((
                                  ))";

    Scanner scanner(braces);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].kind == TokenKind::LEFT_PAREN);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::RIGHT_PAREN);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Braces") {
    const std::string braces = R"({

                                  })";

    Scanner scanner(braces);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].kind == TokenKind::LEFT_BRACE);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::RIGHT_BRACE);
    REQUIRE(tokens[1].line == 3);

    REQUIRE(tokens[2].kind == TokenKind::END_OF_FILE);
  }

  ///////////
}
