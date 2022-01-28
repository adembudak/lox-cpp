#include "lox/token.h"
#include "lox/scanner/scanner.h"
#include "lox/ast/ast.h"
#include "lox/parser/parser.h"

#include <fstream>
#include <iterator>
#include <string>

int main(int argc, const char *const argv[]) {
  if (argc < 2)
    return 1;

  const auto source = std::string(std::istream_iterator<char>(std::ifstream{argv[1]} >> std::noskipws), {});

  lox::Scanner scanner(source);
  const auto tokens = scanner.scan();
  scanner.trace();

  return 0;
}
