#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>

#include <variant>

#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)    /* Dont do anything at all please*/
#endif
#define TABLENAME_MAXLEN 128

namespace {
  class Stack
  {
    typedef std::variant<int, double, char> Variant;
    std::vector<Variant> Vec;
    bool insert(void *element)
    {
      Vec.push_back((Variant) 123);
      std::cout << Vec.data();
      return true;
    }

  };

  /*
   *********************************************
   * Type Classes
   *********************************************
   */
  class Types
  {
  public:
    virtual char getType(){return '0';}
    virtual bool cmp(bool a, bool b){return (a&&b);}
  };

  class IntType : public Types
  {
    char getType(){return 'i';}
    static  bool cmp(int a, int b)
    {
      return (a > b);
    }
  };

  class CharType : public Types
  {
    char getType(){return 'c';}
    static bool cmp(char a, char b)
    {
      return (a > b);
    }
  };

  class DoubleType : public Types
  {
    char getType(){return 'd';}
    static bool cmp(double a, double b)
    {
      return(a > b);
    }
  };

  class DateType : public Types
  {
    char getType(){return 'D';}

  };



/*
 * https://www.daniweb.com/programming/software-development/threads/307115/sort-a-stl-list-of-structs
 */

  class AlternativeTokenizer {
    std::ifstream fHandler;
    std::string line;
    size_t curStrIndex;
    size_t curStrSize;
    bool InternEof = false;
  public:
    AlternativeTokenizer(std::string filename) {
      fHandler.open(filename, std::ios::out);
      InternEof = fHandler.fail();
      if (InternEof)
        return;
      curStrIndex = 0;
      std::getline(fHandler, line);
      curStrSize = line.length();
      curStrIndex = 0;
    }

    char next() {
      if (InternEof)
        return -1; // @TODO Exception?
      if (fHandler.eof()) {
        InternEof = true;
        return -1;
      }
      char ret;
      if (curStrIndex > curStrSize - 1) {
        if (!std::getline(fHandler, line))
          return -1;
        curStrIndex = 0;
        curStrSize = line.length();
        return '\n'; // @TODO: Eval if needed.  Actually not, but could be useful when multi-line is necessary.
      }

      return line[curStrIndex++];

    }

    bool eof() { return InternEof; }
  };

  class WholeFileTokenizer
  {
    std::ifstream fHandler;
    std::string Lines;
  public:
    WholeFileTokenizer(std::string Filename)
    {
      fHandler.open(Filename, std::ios::out);
      if(!fHandler)
      {
        DEBUG_PRINT("Couldnt open file");
        return;
      }
      std::basic_ostringstream<char> ss;
      ss = std::ostringstream();
      ss << fHandler.rdbuf();
      Lines = ss.str();
      DEBUG_PRINT("%s", Lines.c_str());
    }
  };
  class Tokenizer {
    std::ifstream fHandler;
    std::string Text;
    bool InDeclareStr = false;
    int InternEof;
  public:

    Tokenizer(std::string filename) {
      fHandler.open(filename, std::ios::out);
      InternEof = fHandler.fail();
#ifdef DEBUG
      if (InternEof)
        DEBUG_PRINT("COULDNT OPEN\n");
#endif
    }

    char next() {
      char ret;
      if (fHandler.eof()) {
        InternEof = true;
        return 0;
      }

      fHandler >> std::noskipws >> ret;
/*
    if(!InDeclareStr && ret == '"')
    {
      DEBUG_PRINT("in declare STR! \n");
      InDeclareStr = true;
    }
    else if(InDeclareStr && ret == '"')
    {
      DEBUG_PRINT("declaration of STR done\n");
      InDeclareStr = false;
    }
    if(!InDeclareStr)
    {
      if(ret == ' ')
      {
        DEBUG_PRINT("\nuseless space! \n");
        return next();
      }
    }
 */
      return ret;
    }

    bool eof() { return InternEof; }
  };

  class Configparse {
  private:
    std::ifstream fHandler;
    std::vector<std::vector<std::string>> Map;
    std::string curLine;
    size_t curCharInLine;
    size_t ProccessedLines;
    std::string Filename;
  public:

    Configparse(std::string Fname = "../test") : Filename{Fname} {
      curCharInLine = 0;
      ProccessedLines = 0;
      fHandler.open(Filename, std::ifstream::out);
    }

    bool getLine() {
      ProccessedLines++;
      curCharInLine = 0;
      return std::getline(fHandler, curLine) && fHandler.good();
    }

    std::string getstrLine() {
      return curLine;
    }

    char getch() {
      if (curLine.length() > curCharInLine || curLine.empty()) {
        curCharInLine = 0;
        getLine();
        return curLine[curCharInLine++];
      } else
        return curLine[curCharInLine++];
    }

    unsigned long int filesize() {
      std::streampos fsize = 0;
      std::ifstream file(Filename, std::ios::binary);
      fsize = file.tellg();
      file.seekg(0, std::ios::end);
      fsize = file.tellg() - fsize;
      file.close();
      return fsize;
    }


    void parse() {
      int fsize = filesize();
      getLine();
      for (size_t i = 0; i < curLine.length(); ++i) {
        if (curLine[i] == '{')
          DEBUG_PRINT("opening bracket!!!\n");
        if (curLine[i] == '}')
          DEBUG_PRINT("closing bracket!!!\n");
      }
      fHandler.seekg(0);
    }

    void test() {
      std::vector<std::vector<std::string>>::iterator row;
      std::vector<std::string>::iterator col;
      for (int i = 0; i < 5; i++) {
        std::vector<std::string> temp;
        temp.push_back("hi" + std::to_string(i));
        Map.push_back(temp);
      }
      for (row = Map.begin(); row != Map.end(); row++) {
        for (col = row->begin(); col != row->end(); col++) {
          std::cout << *col << "\n";
        }
      }
    }
  };
}


int main()
{

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

/*
  Tokenizer b("../test");

  while(!b.eof())
  {
    char cur=b.next();
    if(cur == '}')
      std::cout << "Curly close!\n";
    else if (cur == '{')
      std::cout << "Curly open!\n";
    else
      std::cout << cur;
  }

 */
  std::cout << "Here!\n";
  AlternativeTokenizer c("../test");
  while(!c.eof())
  {
    char cur=c.next();
    if(cur == -1)
      continue;
    else
      std::cout << cur;
  }
  WholeFileTokenizer d("../test");

  std::cout << std::endl;
  typedef std::variant<int, double, char, std::string> Variant;

  std::vector<Variant> Vec;
  Vec.push_back((Variant) 123);
  Vec.push_back((Variant) 'a');
  Vec.push_back((Variant) 1.1);
  Vec.push_back((Variant) "Hallo Welt");
  for(auto &v: Vec)
  {
    std::visit([](auto&& arg){std::cout << arg << " ";}, v);
  }
  std::cout << std::endl;
  // display each type
  for (auto& v: Vec){
    std::visit([](auto&& arg){std::cout << typeid(arg).name() << " ";}, v); // 3
  }
  std::cout << std::endl;


  Types *t;
  IntType b;
  t=&b;
  std::cout << t->getType();
  return 0;
}
