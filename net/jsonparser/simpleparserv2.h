#ifndef SIMPLEPARSERV2_H
#define SIMPLEPARSERV2_H

#include <iostream>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <cstring>
#include <cassert>
#include <memory>
//#include <zconf.h>
#define FOR_EACH_SYM \
  SYMBOLNAME (Unknown)          \
  SYMBOLNAME (literal_true)     \
  SYMBOLNAME (literal_false)    \
  SYMBOLNAME (literal_null)     \
  SYMBOLNAME (value_string)     \
  SYMBOLNAME (value_number)     \
  SYMBOLNAME (value_float)      \
  SYMBOLNAME (begin_array)      \
  SYMBOLNAME (begin_object)     \
  SYMBOLNAME (end_array)        \
  SYMBOLNAME (end_object)       \
  SYMBOLNAME (value_separator)  \
  SYMBOLNAME (member_seperator) \
  SYMBOLNAME (parse_error)      \
  SYMBOLNAME (end_of_input)
std::string get_file_contents(const char *filename)
{
  std::FILE *fp = std::fopen(filename, "rb");
  if (fp)
  {
    std::string contents;
    std::fseek(fp, 0, SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&contents[0], 1, contents.size(), fp);
    std::fclose(fp);
    return(contents);
  }
  std::cout << (errno);
  std::abort();
}

enum class JSON_SYM {
  #define SYMBOLNAME(x) x,
  FOR_EACH_SYM
  #undef SYMBOLNAME
};

struct GenericResult
{
  long StartPos=-1, EndPos=-1;
  std::string String = {""};
};

struct NumberResult : public GenericResult
{
  bool hasDot = {false};
  bool hasE = {false};
  double Number = {0};

  NumberResult()
  {
    return;
  }
  NumberResult(const NumberResult &p)
  {
    hasDot = p.hasDot;
    hasE = p.hasE;
    StartPos = p.StartPos;
    EndPos = p.EndPos;
    Number = p.Number;
    String = p.String;
  }
  NumberResult(const NumberResult &&p)
  {
    hasDot = p.hasDot;
    hasE = p.hasE;
    StartPos = p.StartPos;
    EndPos = p.EndPos;
    Number = p.Number;
    String = p.String;
  }
};


struct StringResult : GenericResult
{
  StringResult()
  {
    return;
  }
  StringResult(const StringResult &p)
  {
    String = p.String;
    StartPos = p.StartPos;
    EndPos = p.EndPos;
  }
  StringResult(const StringResult &&p)
  {
    StartPos = p.StartPos;
    EndPos = p.EndPos;
    String = p.String;
  }
};


struct JSON_Object
{
public:
  struct KeyNameOrNumber
  {
    std::string str;
    int i;
  } Key;
  enum class Typename
  {
    Number,
    String,
    Boolean,
    Null,
    None
  };
  GenericResult *value = {nullptr};
  int Pos;
  Typename Type;
  JSON_SYM Sym;
  std::vector<JSON_Object> ArryObj;
};

class Tokenizer
{
private:
  std::string File;
  size_t Position;

public:
  explicit Tokenizer(const std::string &F)
  {
    File = get_file_contents(F.c_str());
    Position = 0;
  }

  size_t size() const
  {
    return File.size();
  }

  //@TODO Bounds checking File.size();
  //@TODO Also exception
  void setPos(size_t const pos)
  {
    Position = pos;
  }

  size_t getPos() const
  {
    return Position;
  }

  bool ignoreWhitespace()
  {
    while(File[Position] == '\t' || File[Position]  == '\r' || File[Position] == '\n' || File[Position] == ' ')
    {
      if(Position > File.size())
        return false;
      ++Position;
    }
    return true;
  }

  char next()
  {
    if(!ignoreWhitespace())
      return 0;
    return File[Position++];
  }

  bool eof() const
  {
    return File.size() <= Position;
  }

  // @TODO: Exception. Now.
  struct NumberResult getNumber(size_t FromPos = 0)
  {
    if(FromPos == 0)
      FromPos = this->getPos()-1;

    assert(FromPos <= File.size() && "First argument is bigger than the file size!");

    size_t cursor=FromPos;
    NumberResult res;
    res.hasDot = false;
    bool runner=true;
    std::string temp;
    for(;runner; ++cursor)
    {

      if(std::isdigit(File[cursor]))
      {
        temp+=File[cursor];
        continue;
      }
      else if(File[cursor] == '.' && std::isdigit(File[cursor+1]))
      {
        res.hasDot = true;
        temp+=File[cursor];
        continue;
      }
      else if(res.hasE == false && (File[cursor] == 'e' || File[cursor] == 'E'))
      {
        res.hasE = true;
        temp+=File[cursor];
        continue;
      }
      else if(res.hasE == true && (File[cursor] == 'e' || File[cursor] == 'E'))
      {
        assert(false && "multiple exponents");
      }
      else if(File[cursor] == '-' || File[cursor] == '+')
      {
        temp+=File[cursor];
        continue;
      }
      else
        break;
    }
    res.StartPos = static_cast<long>(FromPos);
    res.EndPos = static_cast<long>(--cursor);
    res.String = temp;
    return res;
  }

  StringResult getStrOnPos(size_t startingPos = 0)
  {
    auto isQuote = [](char ch)
    {
      return (ch == '\'' || ch == '"');
    };
    auto isViableHexChar=[](char s)
    {
      return (std::isdigit(s) ||
         s == 'A' || s == 'a' ||
         s == 'B' || s == 'b' ||
         s == 'C' || s == 'c' ||
         s == 'D' || s == 'd' ||
         s == 'E' || s == 'e');
    };
    if(startingPos == 0)
    {
      startingPos = getPos()-1;
    }
    assert(startingPos <= File.size() && "Requested startingPosition is bigger then the actual file size.");
    StringResult returner;
    bool escaped = false;
    std::string temp;
    char StrStarter = peek(startingPos);
    char cur;
    if(!isQuote(StrStarter))
    {
      assert(false && "Current character isn't a quote. Valid are ' and \".");
    }
    // looks like a string?

    bool runner = true;
    unsigned long i = startingPos+1;
    for(; runner && !eof(); ++i)
    {
      cur = peek(static_cast<size_t>(i));
      if(cur == '\\')
      {
        if(StrStarter == '\'' &&
           (peek(static_cast<size_t>(i+1)) == 'u' || peek(static_cast<size_t>(i+1)) == 'U') &&
           isViableHexChar(peek(static_cast<size_t>(i+1))) &&
           isViableHexChar(peek(static_cast<size_t>(i+2))) &&
           isViableHexChar(peek(static_cast<size_t>(i+3))) &&
           isViableHexChar(peek(static_cast<size_t>(i+4)))) // @TODO OTHERWISE escaped == True bla
        {
          // @TODO it's a valid codepoint. Process it...
        }
        temp+=cur;
        escaped=true;
      }
      else if (cur == StrStarter)
      {
        //str end!
        if(!escaped)
          runner=false;
        else
        {
          temp+=cur;
          escaped=false;
        }
      }
      else
      {
        temp+=cur;
      }
    }
    returner.String = temp;
    returner.StartPos = static_cast<long>(startingPos);
    returner.EndPos = static_cast<long>(i);
    return returner;
  }

  char peek(size_t Pos=0) const
  {
    if(Pos == 0)
      Pos = this->getPos();
    assert(Pos <= File.size() && "delivered position is actually bigger then the file size!");

    return File[Pos];
  }
  bool peek(const std::string &strToPeekFor) const
  {
    bool peeker = true;
    for(size_t i = 0; i != strToPeekFor.size(); ++i)
    {
      if(strToPeekFor[i] != peek(getPos()+i))
      {
        peeker = false;
        break;
      }
    }
    return peeker;
  }
};

class simpleparserv2
{
public:
  Tokenizer tok;
  std::vector<JSON_Object> H;

  simpleparserv2(const std::string&);
  void parse();
  void PrintAST(void (*function_hook)(std::vector<JSON_Object>&) = nullptr)
  {
    std::cout << "AST Size: " << H.size() << "\n";
    int indent = 0;
    auto indenter=[&indent]()
    {
      for(int j = indent; j != 0; --j)
        std::cout <<'\t';
    };
    auto printStringResult = [&](const GenericResult& res) {
      indenter();
      std::cout << "StringDump: Str: '" << res.String << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
    };
    auto printNumberResult = [&](const GenericResult& res) {
      indenter();
      std::cout << "NumberDump: Str: '" << res.String << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
    };
    auto printFloatResult = [&](const GenericResult& res) {
      indenter();
      std::cout << "FloatDump: Str: '" << res.String << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
    };

    std::string endStr = ", hurray!\n";
    std::vector<JSON_Object> ASTCop(H);
    if(function_hook != nullptr)
    {
      // @TODO Functor! Filters for example!
      (*function_hook)(ASTCop);
    }
    size_t i = 0;
    for(JSON_Object &c: ASTCop)
    {
      ++i;
      switch(c.Sym)
      {
        case JSON_SYM::value_string:
          printStringResult(*(c).value);
          break;
        case JSON_SYM::value_number:
          printNumberResult(*(c).value);
          break;
        case JSON_SYM::value_float:
          printFloatResult(*(c).value);
          break;
        case JSON_SYM::literal_true:
          indenter();
          std::cout << "literal_true at " << c.Pos << ". \n";
          break;
        case JSON_SYM::literal_false:
          indenter();
          std::cout << "literal_false at " << c.Pos << ". \n";
          break;
        case JSON_SYM::literal_null:
          indenter();
          std::cout << "literal_null at " << c.Pos << ". \n";
          break;
        case JSON_SYM::begin_object:
          indenter();
          ++indent;
          std::cout << "begin_object at " << c.Pos << ". \n";
          break;
        case JSON_SYM::begin_array:
          indenter();
          ++indent;
          std::cout << "begin_array at " << c.Pos << ". \n";
          break;
        case JSON_SYM::end_array:
          --indent;
          indenter();
          std::cout << "end_array at " << c.Pos << ". \n";
          break;
        case JSON_SYM::end_object:
          --indent;
          indenter();
          std::cout << "end_object at " << c.Pos << ". \n";
          break;
        case JSON_SYM::value_separator:
          indenter();
          std::cout << "name_seperator at " << c.Pos << ": ";
          break;
        case JSON_SYM::member_seperator:
          indenter();
          std::cout << "member_seperator at " << c.Pos << ",\n";
          break;
        case JSON_SYM::parse_error:
        case JSON_SYM::Unknown:
          indenter();
          std::cout << "UNKNOWN at " << c.Pos << ". \n";
          break;
        case JSON_SYM::end_of_input:
          if(indent!=0)
            std::cout << "huh....\n";
          std::cout << "End at "<< c.Pos << ".\n";
          break;
      }
    }
    std::cout << "Saw " << i << " objects " << std::endl;
  }
  virtual ~simpleparserv2();
};

#endif // SIMPLEPARSERV2_H
