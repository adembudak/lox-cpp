#include <lox/scanner/scanner.h>
#include <lox/parser/parser.h>
#include <lox/resolver/resolver.h>
#include <lox/interpreter/interpreter.h>
#include <lox/astprinter/astprinter.h>

#include <string>
#include <iterator>
#include <fstream>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>

namespace {

bool prettyprint = false;

void run(const std::string &source) {
  lox::Scanner scanner{source};
  std::vector<lox::Token> tokens = scanner.scan();

  lox::Parser parser{tokens};
  std::vector<lox::Stmt> statements = parser.parse();

  if (prettyprint) {
    lox::ASTPrinter astprinter{statements};
    astprinter.print(std::cout);
  } else {
    lox::Interpreter interpreter{statements};
    lox::Resolver resolver{interpreter};
    resolver.resolve(statements);
    interpreter.interpret();
  }
}

void runFile(const std::string &sourceFile) {
  std::ifstream fin;
  fin.open(sourceFile);
  // assert(fin.is_open());

  const auto source = std::string(std::istream_iterator<char>(fin >> std::noskipws >> std::boolalpha), {});
  run(source);
}

void runREPL() {
  const std::string prompt = ">>> ";
  const std::vector<std::string> exit_words{"exit", "quit", "wq"};

  std::cout << prompt;
  std::string input;
  for (;;) {
    std::getline(std::cin, input);
    if (std::find(cbegin(exit_words), cend(exit_words), input) != end(exit_words))
      break;

    run(input);
    std::cout << prompt;
  }
}
}

int main(int argc, const char *const argv[]) {
  const std::vector<std::string> arguments(argv + 1, argv + argc);

  if (arguments.empty()) {
    runREPL();
  }

  else {
    const std::string menu = R"(usage: lox-cli [option] [file]
Options:
-p     : pretty print and exit
)";

    const bool prinMenuAndExit =
        std::any_of(cbegin(arguments), cend(arguments), [](const std::string &arg) { return arg == "--help" || arg == "-h"; });

    if (prinMenuAndExit) {
      std::cout << menu;
      return 0;
    }

    if (std::find(cbegin(arguments), cend(arguments), "-p") != cend(arguments)) {
      prettyprint = true;
    }

    runFile(arguments.back());
  }

  return 0;
}
