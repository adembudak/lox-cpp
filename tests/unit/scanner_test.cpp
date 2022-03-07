#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>
#include <variant>

#include "lox/token.h"
#include "lox/scanner/scanner.h"

TEST_CASE("Scanner", "Test individual elements") {
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

  SECTION("String") {
    const std::string input = R"( " this is a string " )";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);

    REQUIRE(tokens[0].kind == TokenKind::STRING);
    REQUIRE(std::get<std::string>(tokens[0].literal) == std::string(" this is a string "));
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::END_OF_FILE);
  }

  SECTION("Number") {
    using namespace Catch::literals;
    const std::string input = "3.14159";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);

    REQUIRE(tokens[0].kind == TokenKind::NUMBER);
    REQUIRE(std::get<double>(tokens[0].literal) == 3.14159_a);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::END_OF_FILE);
  }

  SECTION("for loop") {
    const std::string input = "for (var i = 0; true; i = i+i)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::FOR);

    REQUIRE(tokens[1].kind == TokenKind::LEFT_PAREN);
    REQUIRE(tokens[2].kind == TokenKind::VAR);
    REQUIRE(tokens[3].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[4].kind == TokenKind::EQUAL);
    REQUIRE(tokens[5].kind == TokenKind::NUMBER);
    REQUIRE(std::get<double>(tokens[5].literal) == Approx(0));
    REQUIRE(tokens[6].kind == TokenKind::SEMICOLON);

    REQUIRE(tokens[7].kind == TokenKind::TRUE);
    REQUIRE(tokens[8].kind == TokenKind::SEMICOLON);

    REQUIRE(tokens[9].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[10].kind == TokenKind::EQUAL);
    REQUIRE(tokens[11].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[12].kind == TokenKind::PLUS);
    REQUIRE(tokens[13].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[14].kind == TokenKind::RIGHT_PAREN);
    REQUIRE(tokens[15].kind == TokenKind::END_OF_FILE);
  }

  SECTION("while loop") {
    const std::string input = "while(true)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::WHILE);
    REQUIRE(tokens[1].kind == TokenKind::LEFT_PAREN);
    REQUIRE(tokens[2].kind == TokenKind::TRUE);
    REQUIRE(tokens[3].kind == TokenKind::RIGHT_PAREN);
    REQUIRE(tokens[4].kind == TokenKind::END_OF_FILE);
  }

  SECTION("and-or") {
    const std::string input = "(p or (p and q)) == p";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::LEFT_PAREN);
    REQUIRE(tokens[1].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[2].kind == TokenKind::OR);

    REQUIRE(tokens[3].kind == TokenKind::LEFT_PAREN);
    REQUIRE(tokens[4].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[5].kind == TokenKind::AND);
    REQUIRE(tokens[6].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[7].kind == TokenKind::RIGHT_PAREN);
    REQUIRE(tokens[8].kind == TokenKind::RIGHT_PAREN);

    REQUIRE(tokens[9].kind == TokenKind::EQUAL_EQUAL);
    REQUIRE(tokens[10].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[11].kind == TokenKind::END_OF_FILE);
  }

  SECTION("function") {

    const std::string input = R"(
      fun someFunc() {
        print "Alors on danse x 9";
        return 0;
      }
    )";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::FUN);
    REQUIRE(tokens[1].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[2].kind == TokenKind::LEFT_PAREN);
    REQUIRE(tokens[3].kind == TokenKind::RIGHT_PAREN);
    REQUIRE(tokens[4].kind == TokenKind::LEFT_BRACE);

    REQUIRE(tokens[5].kind == TokenKind::PRINT);
    REQUIRE(tokens[6].kind == TokenKind::STRING);
    REQUIRE(std::get<std::string>(tokens[6].literal) == "Alors on danse x 9");

    REQUIRE(tokens[7].kind == TokenKind::SEMICOLON);

    REQUIRE(tokens[8].kind == TokenKind::RETURN);
    REQUIRE(tokens[9].kind == TokenKind::NUMBER);
    REQUIRE(std::get<double>(tokens[9].literal) == 0);
    REQUIRE(tokens[10].kind == TokenKind::SEMICOLON);

    REQUIRE(tokens[11].kind == TokenKind::RIGHT_BRACE);

    REQUIRE(tokens[12].kind == TokenKind::END_OF_FILE);
  }

  SECTION("if-else, true-false") {
    const std::string input =
        R"( if (p) { return true; }
            else   { return false; })";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::IF);
    REQUIRE(tokens[1].kind == TokenKind::LEFT_PAREN);
    REQUIRE(tokens[2].kind == TokenKind::IDENTIFIER);
    REQUIRE(tokens[2].lexeme == "p");
    REQUIRE(tokens[3].kind == TokenKind::RIGHT_PAREN);

    REQUIRE(tokens[4].kind == TokenKind::LEFT_BRACE);
    REQUIRE(tokens[5].kind == TokenKind::RETURN);
    REQUIRE(tokens[6].kind == TokenKind::TRUE);
    REQUIRE(tokens[7].kind == TokenKind::SEMICOLON);
    REQUIRE(tokens[8].kind == TokenKind::RIGHT_BRACE);

    REQUIRE(tokens[9].kind == TokenKind::ELSE);
    REQUIRE(tokens[10].kind == TokenKind::LEFT_BRACE);
    REQUIRE(tokens[11].kind == TokenKind::RETURN);
    REQUIRE(tokens[12].kind == TokenKind::FALSE);
    REQUIRE(tokens[13].kind == TokenKind::SEMICOLON);
    REQUIRE(tokens[14].kind == TokenKind::RIGHT_BRACE);
  }
}
