#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
enum {
  nullsym, intsym, tablesym, timesym, strsym, lparen, rparen, eofsym
};

std::vector<int> table;
std::vector<std::string> strings;
class Scanner
{
public:
  int ch;
  FILE *f;
  bool init(const char fn[])
  {
    if((f=fopen(fn, "r")) == NULL)
    {
      printf("Couldnt open file %s.", fn);
      return false;
    }
    ch = ' ';
    readch();
    return true;
  }

  void readch()
  {
    ch = fgetc(f);
    /*if(ch != EOF)
      putchar((char)ch);
    else
      ch=EOF;*/
  }

  void loop_ch()
  {
    while(ch != EOF)
    {
      if(ch == '\n')
        printf("ch: Newline\n");
      else if(ch == ' ')
        printf("ch: Space\n");
      else
        printf("ch: '%c'\n", ch);
      readch();
    }
  }
};

int main() {
  Scanner Test;
  Test.init("../main.cpp");
  Test.loop_ch();
  std::cout << "Hello, World!" << std::endl;
  return 0;
}
