#include "simpleparser.h"

int main()
{
  JSON_VAL b;
  b.Type = JSON_SYM::begin_array;
  Parser s{"./example.json"};
  s.parse();
  s.PrintAST();
  std::cout << std::endl;
  s.Turn(s.AST);
}
