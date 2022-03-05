#include <lox/interpreter/interpreter.h>
#include <lox/scanner/scanner.h>
#include <lox/parser/parser.h>
#include <lox/prettyprint/prettyprint.h>

#include <string>
#include <iterator>
#include <fstream>
#include <cassert>
#include <iostream>
#include <vector>

namespace {

void run(const std::string &source) {
  lox::Scanner scanner(source);
  std::vector tokens = scanner.scan();

  lox::Parser parser(tokens);
  std::vector statements = parser.parse();

  lox::Interpreter interpreter(statements);
  interpreter.interpret();
}

void runFile(const std::string &sourceFile) {
  std::ifstream fin;
  fin.open(sourceFile);
  // assert(fin.is_open());

  const auto source = std::string(std::istream_iterator<char>(fin >> std::noskipws >> std::boolalpha), {});

  run(source);
}

void runPrompt() {
  for (;;) {
    std::cout << ">>> ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
      std::cout << ">>> ";
      run(input);
    }
  }
}

}

int main(int argc, const char *const argv[]) {
  if (argc < 2) {
    std::cout << "Usage: lox-cli [script]\n";
    return 1;
  } else if (argc == 2) { // execute file
    runFile(argv[1]);
  } else { // REPL
    runPrompt();
  }

  return 0;
}
