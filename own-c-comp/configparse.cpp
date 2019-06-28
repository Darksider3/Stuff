#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>

#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
int DeallocateCounter=1;
#else
#define DEBUG_PRINT(fmt, args...)    /* Dont do anything at all pls*/
#endif
#define TABLENAME_MAXLEN 128

namespace {
  struct Values {
    char *Name;
    char *Typename;
    void *value;
    struct Values *next;
  };

  struct Table {
    char Name[TABLENAME_MAXLEN];
    struct Values *valueStart;
  };

/*
 * https://www.daniweb.com/programming/software-development/threads/307115/sort-a-stl-list-of-structs
 */

  bool CompareValuesSizes(const Values &first, const Values &second) {
    if (strcmp(first.Typename, second.Typename) == 0) {
      if (strcmp(first.Typename, "string") == 0) {
        //@TODO String-compare
      } else if (strcmp(first.Typename, "int") == 0) {
        return (first.value > second.value);
      } else if (strcmp(first.Typename, "float") == 0) {
        return ((float *) first.value > (float *) second.value);
      }
      //@TODO: Dates
    } else {
      std::cout << "Not the same types bro \n";
    }

    return true;
  }

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
  return 0;
}
