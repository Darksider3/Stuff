#include "simpleparser.h"

int main()
{
  JSON_VAL b;
  b.Sym = JSON_SYM::begin_array;
  Parser s{"./example.json"};
  s.parse();
  s.PrintAST();
  s.validate();
  s.statistics();
  std::cout << std::endl;
  s.Turn(s.AST);
}
