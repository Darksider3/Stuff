#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <cstring>

#define TABLENAME_MAXLEN 128


struct Values
{
  char *Name;
  char *Typename;
  void *value;
  struct Values *next;
};

struct Table
{
  char Name[TABLENAME_MAXLEN];
  struct Values *valueStart;
};

/*
 * https://www.daniweb.com/programming/software-development/threads/307115/sort-a-stl-list-of-structs
 */

bool CompareValues(const Values& first, const Values& second)
{
  if(strcmp(first.Typename, second.Typename) == 0)
  {
    if(strcmp(first.Typename, "string") == 0)
    {
      return strcmp((char *) first.value, (char *) second.value) == 0;
    }
    else if(strcmp(first.Typename, "int") == 0)
    {
      return (first.value == second.value);
    }
    //@TODO: Dates
  }
  else
  {
    std::cout << "Not the same types bro \n";
  }
}
class Configparse
{
private:
  std::ifstream fHandler;
  std::vector<std::vector<std::string>> Map;
  std::string curLine;
  unsigned int curCharInLine;
  unsigned int ProccessedLines;
  std::string Filename;
public:

  Configparse(std::string Fname="../test") : Filename{Fname}
  {
    curCharInLine=0;
    ProccessedLines=0;
    fHandler.open(Filename, std::ifstream::out);
  }

  bool getLine()
  {
    ProccessedLines++;
    curCharInLine = 0;
    return std::getline(fHandler, curLine) && fHandler.good();
  }

  std::string getstrLine()
  {
    return curLine;
  }

  char getch()
  {
    if(curLine.length() > curCharInLine || curLine.empty())
    {
      curCharInLine=0;
      getLine();
      return curLine[curCharInLine++];
    }
    else
      return curLine[curCharInLine++];
  }

  unsigned long int filesize()
  {
    std::streampos fsize=0;
    std::ifstream file(Filename, std::ios::binary);
    fsize=file.tellg();
    file.seekg(0, std::ios::end);
    fsize = file.tellg() - fsize;
    file.close();
    return fsize;
  }


  void parse()
  {
    int fsize = filesize();
    getLine();
    for(int i=0; i < curLine.length(); ++i)
    {
      if(curLine[i] == '{')
        std::cout << "opening bracket!!!\n";
      if(curLine[i] == '}')
        std::cout << "closing bracket!!!\n";
    }
    fHandler.seekg(0);
  }

  void test()
  {
    std::vector<std::vector<std::string>>::iterator row;
    std::vector<std::string>::iterator col;
    for(int i=0; i < 5; i++)
    {
      std::vector<std::string> temp;
      temp.push_back("hi" + std::to_string(i));
      Map.push_back(temp);
    }
    for(row = Map.begin(); row != Map.end(); row++)
    {
      for (col = row->begin(); col != row->end(); col++)
      {
        std::cout << *col << "\n";
      }
    }
  }
};

int main()
{

  std::cout << "Hi! \n";
  Configparse b("../test");
  b.test();
  b.parse();
  if(b.getLine())
    std::cout << b.getstrLine() << std::endl;
  if(b.getLine())
    std::cout << b.getstrLine() << std::endl;
  if(b.getLine())
    std::cout << b.getstrLine() << std::endl;
  if(b.getLine())
    std::cout << b.getstrLine() << std::endl;
  else
    std::cout << "End!";
  return 0;
}
