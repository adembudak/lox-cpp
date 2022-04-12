#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>
#include <vector>
#include <variant>

#include "lox/primitives/token.h"
#include "lox/scanner/scanner.h"

TEST_CASE("Scanner", "Test individual elements") {
  using namespace lox;

  SECTION("Parenthesis") {
    const std::string input = R"((
                                  ))";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].kind == TokenKind::LeftParen);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::RightParen);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::EndOfFile);
  }

  SECTION("Braces") {
    const std::string input = R"({

                                  })";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].kind == TokenKind::LeftBrace);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::RightBrace);
    REQUIRE(tokens[1].line == 3);

    REQUIRE(tokens[2].kind == TokenKind::EndOfFile);
  }

  SECTION("Dot") {
    const std::string input = ".";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::Dot);
    REQUIRE(tokens[1].kind == TokenKind::EndOfFile);
  }

  SECTION("Comma") {
    const std::string input = ",";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::Comma);
    REQUIRE(tokens[1].kind == TokenKind::EndOfFile);
  }

  SECTION("Minus") {
    const std::string input = "-";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::Minus);
    REQUIRE(tokens[1].kind == TokenKind::EndOfFile);
  }

  SECTION("Plus") {
    const std::string input = "+";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::Plus);
    REQUIRE(tokens[1].kind == TokenKind::EndOfFile);
  }

  SECTION("Semicolon") {
    const std::string input = ";";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::Semicolon);
    REQUIRE(tokens[1].kind == TokenKind::EndOfFile);
  }

  SECTION("Star") {
    const std::string input = "*";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].kind == TokenKind::Star);
    REQUIRE(tokens[1].kind == TokenKind::EndOfFile);
  }

  SECTION("Bang, Bang Equal") {
    const std::string input = R"(!
                                 !=)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].kind == TokenKind::Bang);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::BangEqual);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::EndOfFile);
    REQUIRE(tokens[1].line == 2);
  }

  SECTION("Equal, Equal Equal") {
    const std::string input = R"(=
                                 ==)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].kind == TokenKind::Equal);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::EqualEqual);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::EndOfFile);
    REQUIRE(tokens[1].line == 2);
  }

  SECTION("Less, Less Equal") {
    const std::string input = R"(<
                                 <=)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].kind == TokenKind::Less);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::LessEqual);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::EndOfFile);
    REQUIRE(tokens[1].line == 2);
  }

  SECTION("Greater, Greater Equal") {
    const std::string input = R"(>
                                 >=)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].kind == TokenKind::Greater);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::GreaterEqual);
    REQUIRE(tokens[1].line == 2);

    REQUIRE(tokens[2].kind == TokenKind::EndOfFile);
    REQUIRE(tokens[1].line == 2);
  }

  SECTION("Comment") {
    const std::string input = R"(//comment
                                 // comment
                                )";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].kind == TokenKind::EndOfFile);
  }

  SECTION("White Space") {
    const std::string input = R"( 


                                       )";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].kind == TokenKind::EndOfFile);
    REQUIRE(tokens[0].line == 4);
  }

  SECTION("String") {
    const std::string input = R"( " this is a string " )";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);

    REQUIRE(tokens[0].kind == TokenKind::String);
    REQUIRE(std::get<std::string>(tokens[0].literal.data()) == std::string(" this is a string "));
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::EndOfFile);
  }

  SECTION("Number") {
    using namespace Catch::literals;
    const std::string input = "3.14159";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens.size() == 2);

    REQUIRE(tokens[0].kind == TokenKind::Number);
    REQUIRE(std::get<double>(tokens[0].literal.data()) == 3.14159_a);
    REQUIRE(tokens[0].line == 1);

    REQUIRE(tokens[1].kind == TokenKind::EndOfFile);
  }

  SECTION("for loop") {
    const std::string input = "for (var i = 0; true; i = i+i)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::For);

    REQUIRE(tokens[1].kind == TokenKind::LeftParen);
    REQUIRE(tokens[2].kind == TokenKind::Var);
    REQUIRE(tokens[3].kind == TokenKind::Identifier);
    REQUIRE(tokens[4].kind == TokenKind::Equal);
    REQUIRE(tokens[5].kind == TokenKind::Number);
    REQUIRE(std::get<double>(tokens[5].literal.data()) == Approx(0));
    REQUIRE(tokens[6].kind == TokenKind::Semicolon);

    REQUIRE(tokens[7].kind == TokenKind::True);
    REQUIRE(tokens[8].kind == TokenKind::Semicolon);

    REQUIRE(tokens[9].kind == TokenKind::Identifier);
    REQUIRE(tokens[10].kind == TokenKind::Equal);
    REQUIRE(tokens[11].kind == TokenKind::Identifier);
    REQUIRE(tokens[12].kind == TokenKind::Plus);
    REQUIRE(tokens[13].kind == TokenKind::Identifier);
    REQUIRE(tokens[14].kind == TokenKind::RightParen);
    REQUIRE(tokens[15].kind == TokenKind::EndOfFile);
  }

  SECTION("while loop") {
    const std::string input = "while(true)";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::While);
    REQUIRE(tokens[1].kind == TokenKind::LeftParen);
    REQUIRE(tokens[2].kind == TokenKind::True);
    REQUIRE(tokens[3].kind == TokenKind::RightParen);
    REQUIRE(tokens[4].kind == TokenKind::EndOfFile);
  }

  SECTION("and-or") {
    const std::string input = "(p or (p and q)) == p";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::LeftParen);
    REQUIRE(tokens[1].kind == TokenKind::Identifier);
    REQUIRE(tokens[2].kind == TokenKind::Or);

    REQUIRE(tokens[3].kind == TokenKind::LeftParen);
    REQUIRE(tokens[4].kind == TokenKind::Identifier);
    REQUIRE(tokens[5].kind == TokenKind::And);
    REQUIRE(tokens[6].kind == TokenKind::Identifier);
    REQUIRE(tokens[7].kind == TokenKind::RightParen);
    REQUIRE(tokens[8].kind == TokenKind::RightParen);

    REQUIRE(tokens[9].kind == TokenKind::EqualEqual);
    REQUIRE(tokens[10].kind == TokenKind::Identifier);
    REQUIRE(tokens[11].kind == TokenKind::EndOfFile);
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

    REQUIRE(tokens[0].kind == TokenKind::Fun);
    REQUIRE(tokens[1].kind == TokenKind::Identifier);
    REQUIRE(tokens[2].kind == TokenKind::LeftParen);
    REQUIRE(tokens[3].kind == TokenKind::RightParen);
    REQUIRE(tokens[4].kind == TokenKind::LeftBrace);

    REQUIRE(tokens[5].kind == TokenKind::Print);
    REQUIRE(tokens[6].kind == TokenKind::String);
    REQUIRE(std::get<std::string>(tokens[6].literal.data()) == "Alors on danse x 9");

    REQUIRE(tokens[7].kind == TokenKind::Semicolon);

    REQUIRE(tokens[8].kind == TokenKind::Return);
    REQUIRE(tokens[9].kind == TokenKind::Number);
    REQUIRE(std::get<double>(tokens[9].literal.data()) == 0);
    REQUIRE(tokens[10].kind == TokenKind::Semicolon);

    REQUIRE(tokens[11].kind == TokenKind::RightBrace);

    REQUIRE(tokens[12].kind == TokenKind::EndOfFile);
  }

  SECTION("if-else, true-false") {
    const std::string input =
        R"( if (p) { return true; }
            else   { return false; })";

    Scanner scanner(input);
    const std::vector tokens = scanner.scan();

    REQUIRE(tokens[0].kind == TokenKind::If);
    REQUIRE(tokens[1].kind == TokenKind::LeftParen);
    REQUIRE(tokens[2].kind == TokenKind::Identifier);
    REQUIRE(tokens[2].lexeme == "p");
    REQUIRE(tokens[3].kind == TokenKind::RightParen);

    REQUIRE(tokens[4].kind == TokenKind::LeftBrace);
    REQUIRE(tokens[5].kind == TokenKind::Return);
    REQUIRE(tokens[6].kind == TokenKind::True);
    REQUIRE(tokens[7].kind == TokenKind::Semicolon);
    REQUIRE(tokens[8].kind == TokenKind::RightBrace);

    REQUIRE(tokens[9].kind == TokenKind::Else);
    REQUIRE(tokens[10].kind == TokenKind::LeftBrace);
    REQUIRE(tokens[11].kind == TokenKind::Return);
    REQUIRE(tokens[12].kind == TokenKind::False);
    REQUIRE(tokens[13].kind == TokenKind::Semicolon);
    REQUIRE(tokens[14].kind == TokenKind::RightBrace);
  }
}
