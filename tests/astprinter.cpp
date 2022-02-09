#include <lox/scanner/scanner.h>
#include <lox/parser/parser.h>
#include <lox/prettyprint/prettyprint.h>

#include <string>
#include <fstream>
#include <cassert>

int main(int argc, const char *const argv[]) {
  if (argc < 2)
    return 1;

  std::string sourcefile{argv[1]};

  std::ifstream fin;
  fin.open(sourcefile);
  assert(fin.is_open());

  const auto source = std::string(std::istream_iterator<char>(fin >> std::noskipws), {});
  lox::Scanner scanner(source);
  auto tokens = scanner.scan();

  lox::Parser parser(tokens);
  auto expressions = parser.parse();

  std::ofstream fout;
  fout.open(sourcefile + ".ast");

  lox::PrettyPrint(fout, expressions.value());

  return 0;
}
