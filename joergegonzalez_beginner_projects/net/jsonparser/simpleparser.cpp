#include <iostream>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <cstring>
//#include <zconf.h>
#define FOR_EACH_SYM \
  SYMBOLNAME (Unknown)          \
  SYMBOLNAME (literal_true)     \
  SYMBOLNAME (literal_false)    \
  SYMBOLNAME (literal_null)     \
  SYMBOLNAME (value_string)     \
  SYMBOLNAME (value_number)    \
  SYMBOLNAME (value_float)      \
  SYMBOLNAME (begin_array)      \
  SYMBOLNAME (begin_object)     \
  SYMBOLNAME (end_array)        \
  SYMBOLNAME (end_object)       \
  SYMBOLNAME (name_seperator)   \
  SYMBOLNAME (member_seperator) \
  SYMBOLNAME (parse_error)      \
  SYMBOLNAME (end_of_input)

// @TODO: General Exception as base class?
namespace ParserAndTokenExceptions
{

#include <exception>
class GeneralException : public std::exception
{
private:
  std::string msg = {""};
  int line;

public:
  GeneralException(std::string Msg, const int LNo)
  {
    line=LNo;
    msg=Msg;
  }

  GeneralException(GeneralException const &p)
  {
    line=p.line;
    msg=p.msg;
  }

  virtual const char* what() const noexcept
  {
    std::string tmp=msg+std::to_string(line);
    return tmp.c_str();
  }

};

class TokenSizeException : public GeneralException
{
  using GeneralException::GeneralException;
};

class TokenNumberMultipleExponentException : public GeneralException
{
  using GeneralException::GeneralException;
};

class TokenStringWithoutEnding : public GeneralException
{
  using GeneralException::GeneralException;
};

class ParserUnknownCharacterEncountered : public GeneralException
{
  using GeneralException::GeneralException;
};
}
/*
 * HELPER
 */
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
  throw(errno);
}

enum class JSON_SYM {
  #define SYMBOLNAME(x) x,
  FOR_EACH_SYM
  #undef SYMBOLNAME
};
struct NumberResult
{
  bool hasDot = {false};
  bool hasE = {false};
  long StartPos = {-1}, EndPos = {-1};
  double Number= {0};
  std::string str = {""};
};


struct StringResult
{
  std::string str;
  long StartPos={-1}, EndPos={-1};
};

struct JSON_VAL
{
  std::vector<JSON_VAL> ArryObj;
  struct VAL
  {
    bool boolean;
    NumberResult number = {false, false, 0,0,0,""};
    StringResult str;
  } value;
  JSON_SYM Type;

  int Pos;
  static JSON_VAL inplace(JSON_SYM t, size_t pos=0)
  {
    JSON_VAL tmp;
    tmp.Type = t;
    tmp.Pos = static_cast<int>(pos);
    return tmp;
  }
};


// @TODO: __PLEASE__ use exceptions..... -.- 
class Tokenizer
{
private:
  std::string File;
  size_t Position;

public:
  explicit Tokenizer(const std::string &F)
  {
    try
    {
      File = get_file_contents(F.c_str());
      Position = 0;
    }
    catch(int &e)
    {
      std::cout << "Sorry, something went wrong here! Your file doesn't exist.\n";
      std::exit(e);
    }
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

  char nextTok()
  {
    if(!ignoreWhitespace())
      return 0;
    return File[Position++];
  }

  bool eof() const
  {
    return File.size() <= Position;
  }

  /*
   * HELPERS
   * @TODO: own namespace? Won't use this-> than i think o-o
   */

  size_t countNewlineUntilPos(size_t const pos)
  {
    if(pos >= File.size())
    {
      throw(ParserAndTokenExceptions::TokenSizeException("Size bounds error", __LINE__));
    }
    size_t cursor = 0;
    size_t counter = 0;
    for(; cursor != pos; ++cursor)
    {
      if(File[cursor] == '\n')
        counter++;
    }

    return counter;
  }

  // @TODO: Exception.
  std::string getStrFromTo(size_t const frompos, size_t const topos)
  {
    if(topos > File.size())
    {
      throw(ParserAndTokenExceptions::TokenSizeException("Requested Positions exit file bounds, in: ", __LINE__));
    }
    if(frompos < topos)
    {
      throw(ParserAndTokenExceptions::TokenSizeException("given Position to start is bigger then the one requested to end! In: ", __LINE__));
    }
    std::string ret;

    for(size_t i=frompos; i != topos;  ++i)
    {
      ret += File[i];
    }
    return ret;
  }

  // @TODO: Exception. Now.
  struct NumberResult getNumber(size_t FromPos = 0)
  {
    if(FromPos == 0)
      FromPos = this->getPos()-1;

    if(FromPos >= File.size())
    {
      throw(ParserAndTokenExceptions::TokenSizeException("Requested starting position is out of bounds in: ", __LINE__));
    }

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
        throw(ParserAndTokenExceptions::TokenNumberMultipleExponentException("Encountered multiple exponents in number in line ", __LINE__));
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
    res.str = temp;
    return res;
  }

  StringResult getStrOnPos(size_t startingPos = 0)
  {
    if(startingPos == 0)
    {
      startingPos = getPos();
    }
    if(startingPos >= File.size())
    {
      throw(ParserAndTokenExceptions::TokenSizeException("Requested starting position is out of bounds of your given file in line: ", __LINE__));
    }
    StringResult returner;
    bool escaped = false;
    std::string temp;
    char StrStarter = peek(startingPos);
    char cur;
    if(!isQuote(StrStarter))
    {
      throw(ParserAndTokenExceptions::ParserUnknownCharacterEncountered("Expected to encouter \' or \", but instead got something else("+std::to_string(StrStarter)+
            ") in line: ", __LINE__));
    }
    // looks like a string?

    bool runner = true;
    unsigned long i = startingPos+1;
    for(; runner && !eof(); ++i)
    {
      cur = peek(static_cast<size_t>(i));
      if(cur == '\\')
      {
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
    returner.str = temp;
    returner.StartPos = static_cast<long>(startingPos);
    returner.EndPos = static_cast<long>(i);
    return returner;
  }

  bool isQuote(size_t const Pos) const
  {
    if(Pos >= File.size())
      throw(ParserAndTokenExceptions::TokenSizeException("Reqeusted position is out of bounds of the given file. Thrown in line: ", __LINE__));

    return isQuote(File[Pos]); 
  }

  static bool isQuote(char const ch)
  {
    return (ch == '\'' || ch == '"');
  }

  char peek(size_t Pos=0) const
  {
    if(Pos == 0)
      Pos = this->getPos();
    if(Pos >= File.size())
      throw(ParserAndTokenExceptions::TokenSizeException("Requested position is out of bounds of the given file. Thrown in line: ", __LINE__));

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

class Parser
{
private:
  Tokenizer s;
  std::vector<JSON_VAL> AST;
public:
  explicit Parser(const std::string &c) : s{c}
  {
    s = Tokenizer(c);
    // @TODO: check for syntax - { at start, } at end required.
  }

  void parse()
  {
    auto emit = [&](auto Type) {
      AST.emplace_back(JSON_VAL::inplace(Type, s.getPos()));
    };

//    auto strCopy = [](auto str, auto ptr) {
//      for(unsigned long i = 0; i != str.length()+1; ++i)
//      {
//        ptr[i] = str[i];
//      }
//    };
    while(!s.eof())
    {
      char cur = s.nextTok();
      switch(cur)
      {
        case '{':
          emit(JSON_SYM::begin_object);
          continue;
        case '[':
          emit(JSON_SYM::begin_array);
          continue;
        case '}':
          emit(JSON_SYM::end_object);
          continue;
        case ']':
          emit(JSON_SYM::end_array);
          continue;
        case ',':
          emit(JSON_SYM::member_seperator);
          continue;
        case ':':
          emit(JSON_SYM::name_seperator);
          continue;
        case 't':
          if(s.peek("rue"))
          {
            emit(JSON_SYM::literal_true);
            s.setPos(s.getPos()+3);
          }
          else
          {
            emit(JSON_SYM::parse_error);
          }
          continue;
        case 'f':
          if(s.peek("alse"))
          {
            emit(JSON_SYM::literal_false);
            s.setPos(s.getPos()+4);
          }
          else
          {
            emit(JSON_SYM::parse_error);
          }
          continue;
        case 'n':
          if(s.peek("ull"))
          {
            emit(JSON_SYM::literal_null);
            s.setPos(s.getPos()+3);
          }
          else
          {
            emit(JSON_SYM::parse_error);
          }
          continue;
        case '\0':
          emit(JSON_SYM::end_of_input);
          return;
        default:
          break;
      }
      // if string
      // @TODO ' => CHARACTER POINT! __Not__ strings!
      if(cur == '"' || cur == '\'')
      {
        JSON_VAL found_str;
        StringResult temp = s.getStrOnPos(s.getPos()-1);
        found_str.Pos = static_cast<int>(s.getPos()-1);
        found_str.Type = JSON_SYM::value_string;
        found_str.value.str = temp;
        AST.push_back(found_str);
        s.setPos(s.getPos()+temp.str.size()+1);
        continue;
      }
      // it should be a number now
      else
      {
        struct NumberResult num;
        try
        {
          num = s.getNumber();
        }
        catch(ParserAndTokenExceptions::TokenSizeException&)
        {
          std::cout << "Oh, it seems like our file has an end here! We take that and exit gracefully. \n";
          break;
        }
        if(num.str.empty())
        {
          std::cout << "Error! Couldn't really parse number?\n";
        }
        else
        {
          JSON_VAL found_num;
          found_num.Pos = static_cast<int>(s.getPos()-1);
          found_num.value.number = num;
          if(found_num.value.number.hasDot)
          {
            found_num.Type = JSON_SYM::value_float;
          }
          else
          {
            found_num.Type = JSON_SYM::value_number;
          }
          AST.emplace_back(found_num);
          s.setPos(s.getPos()+num.str.size()-1);
          // ok, it's definitly a number. Detect if it's a floating point or an integer and go add it!
          continue;
        }
        if(cur == '\0')
        {
          emit(JSON_SYM::end_of_input);
          return;
        }
      }

      std::cout << "ASSERT NOT REACHED ON CHAR " << cur;
      emit(JSON_SYM::Unknown);
      //std::cout << "SWITCH done, here must be a value or key\n";
    }
    AST.emplace_back(JSON_VAL::inplace(JSON_SYM::end_of_input, s.getPos()));
  }

  void PrintAST(void (*function_hook)(std::vector<JSON_VAL>&) = nullptr)
  {
    int indent = 0;
    auto indenter=[&indent]()
    {
      for(int j = indent; j != 0; --j)
        std::cout <<'\t';
    };
    auto printStringResult = [&](const StringResult& res) {
      indenter();
      std::cout << "StringDump: Str: '" << res.str << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
    };
    auto printNumberResult = [&](const NumberResult& res) {
      indenter();
      std::cout << "NumberDump: Str: '" << res.str << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
    };
    auto printFloatResult = [&](const NumberResult& res) {
      indenter();
      std::cout << "FloatDump: Str: '" << res.str << "'; Start: '" << res.StartPos << "'; end: '"<< res.EndPos <<".'\n";
    };

    std::string endStr = ", hurray!\n";
    std::vector<JSON_VAL> ASTCop(AST);
    if(function_hook != nullptr)
    {
      // @TODO Functor! Filters for example!
      function_hook(ASTCop);
    }

    for(JSON_VAL &c: ASTCop)
    {
      switch(c.Type)
      {
        case JSON_SYM::value_string:
          printStringResult(c.value.str);
          break;
        case JSON_SYM::value_number:
          printNumberResult(c.value.number);
          break;
        case JSON_SYM::value_float:
          printFloatResult(c.value.number);
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
        case JSON_SYM::name_seperator:
          indenter();
          std::cout << "name_seperator at " << c.Pos << ": ";
          break;
        case JSON_SYM::member_seperator:
          indenter();
          std::cout << "member_seperator at " << c.Pos << ",\n";
          break;
        case JSON_SYM::Unknown:
          indenter();
          std::cout << "uninitialised at " << c.Pos << ". \n";
          break;
        case JSON_SYM::end_of_input:
          if(indent!=0)
            std::cout << "huh....\n";
          std::cout << "End at "<< c.Pos << ".\n";
          break;
        default:
          std::cout << "something else!" << std::endl;
      }
    }
    std::cout << std::endl;
  }
};


int main()
{
  JSON_VAL b;
  b.Type = JSON_SYM::begin_array;
  Parser s{"./example.json"};
  s.parse();
  s.PrintAST();
  std::cout << std::endl;

}
