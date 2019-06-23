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

bool CompareValuesSizes(const Values& first, const Values& second)
{
  if(strcmp(first.Typename, second.Typename) == 0)
  {
    if(strcmp(first.Typename, "string") == 0)
    {
      //@TODO String-compare
    }
    else if(strcmp(first.Typename, "int") == 0)
    {
      return (first.value > second.value);
    }
    else if(strcmp(first.Typename, "float") == 0)
    {
      return ((float*)first.value > (float*)second.value);
    }
    //@TODO: Dates
  }
  else
  {
    std::cout << "Not the same types bro \n";
  }

  return true;
}

class Tokenizer
{
  std::ifstream fHandler;
  std::string Text;
  int lineLength;
  int curIndex;
  int InternEof;
public:

  Tokenizer(std::string filename)
  {
    fHandler.open(filename, std::ios::out);
    curIndex = 0;
    lineLength = Text.length();
    InternEof = false;

  }

  char next()
  {
    char ret;
    if(fHandler.eof())
    {
      InternEof = true;
      return 0;
    }

    fHandler >> std::noskipws >> ret;
    return ret;
  }

  bool Eof(){return InternEof;}
};
class Configparse
{
private:
  std::ifstream fHandler;
  std::vector<std::vector<std::string>> Map;
  std::string curLine;
  size_t curCharInLine;
  size_t ProccessedLines;
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
    for(size_t i=0; i < curLine.length(); ++i)
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
  /*Configparse b("../test");
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
    std::cout << "End!";*/

  Tokenizer b("../test");
  std::cout << "Here!";
  while(!b.Eof())
  {
    char cur=b.next();
    if(cur == '}')
      std::cout << "Curly close!\n";
    else if (cur == '{')
      std::cout << "Curly open!\n";
    else
      std::cout << cur;
  }

  std::cout << "Here!";
  return 0;
}
