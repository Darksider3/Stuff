#include <iostream>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <cstring>
//#include <zconf.h>
#define FOR_EACH_SYM \
  SYMBOLNAME (Unknown) \
  SYMBOLNAME (literal_true) \
  SYMBOLNAME (literal_false) \
  SYMBOLNAME (value_string) \
  SYMBOLNAME (value_unsigned) \
  SYMBOLNAME (value_integer) \
  SYMBOLNAME (value_float) \
  SYMBOLNAME (begin_array) \
  SYMBOLNAME (begin_object) \
  SYMBOLNAME (end_array) \
  SYMBOLNAME (end_object) \
  SYMBOLNAME (name_seperator) \
  SYMBOLNAME (value_seperator) \
  SYMBOLNAME (parse_error) \
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
  long startPos = {-1}, EndPos = {-1};
  double Number= {0};
  std::string str = {""};
};


struct StringResult
{
  std::string str;
  long startPos={-1}, endPos={-1};
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
      if(std::isdigit(File[cursor]) || File[cursor] == '.')
      {
        res.hasDot = true;
        temp+=File[cursor];
      }
      else if(res.hasE == false && (File[cursor] == 'e' || File[cursor] == 'E'))
      {
        res.hasE = true;
        temp+=File[cursor];
      }
      else if(res.hasE == true && (File[cursor] == 'e' || File[cursor] == 'E'))
      {
        throw(ParserAndTokenExceptions::TokenNumberMultipleExponentException("Encountered multiple exponents in number in line ", __LINE__));
      }
      else if(File[cursor] == '-' || File[cursor] == '+')
      {
        temp+=File[cursor];
      }
      else
        runner=false;
    }
    res.startPos = static_cast<long>(FromPos);
    res.EndPos = static_cast<long>(cursor);
    res.str = temp;
    return res;
  }
  bool isSeperator(); // Return if Char is a seperator, for json that would be ,(comma)
  bool isChar(const char &c, unsigned int pos);
    //return c == CURRENT_POS_CHAR;
    /*
    while(**cursor == '\t' || **cursor  == '\r' || **cursor == '\n' || **cursor == ' ') ++(*cursor);
    */
    // work on cursor, ignore everything that's a space or newline, exit on nullbyte.
    // return true until nullbyte
    /*
      if(cur == '\'' && cur == '"')
      {
        std::cout << "str start\n";
        StrStarterEncounter = cur;
        bool escaped=false;
        for(bool runner=true; runner && !s.eof();)
        {
          char tcur = s.nextTok();
          if(tcur == '\\')
            escaped=true;
          else if(tcur == StrStarterEncounter)
          {
           std::cout << "str end\n";
            if(!escaped)
              runner=false;
            else
            {
              std::cout << "huh, escaped: " << tcur;
              escaped=false;
            }
          }
        }
      }
     */

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
    returner.startPos = static_cast<long>(startingPos);
    returner.endPos = static_cast<long>(i);
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
          emit(JSON_SYM::value_seperator);
          continue;
        case ':':
          emit(JSON_SYM::name_seperator);
          continue;
        case 't':
          if(s.peek() == 'r' && s.peek(s.getPos()+1) == 'u' && s.peek(s.getPos()+2) == 'e')
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
          if(s.peek() == 'a' && s.peek(s.getPos()+1) == 'l' && s.peek(s.getPos()+2) == 's'  && s.peek(s.getPos()+3) == 'e')
          {
            emit(JSON_SYM::literal_false);
            s.setPos(s.getPos()+4);
          }
          else
          {
            emit(JSON_SYM::parse_error);
          }
          continue;
        case '\0':
          return;
        default:
          break;
      }
      // if string
      // @TODO simplify by putting in a function dafuq is this going to get complex
      if(cur == '"' || cur == '\'')
      {
        JSON_VAL found_str;
        StringResult temp = s.getStrOnPos(s.getPos()-1);
        found_str.Pos = static_cast<int>(s.getPos());
        found_str.Type = JSON_SYM::value_string;
        found_str.value.str = temp;
        std::cout << "getStrOnPos(): '" << found_str.value.str.str << "' <- STR\n";
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

          s.setPos(s.getPos()+num.str.size()+1);
          // ok, it's definitly a number. Detect if it's a floating point or an integer and go add it!
          continue;
        }
        if(cur == '\0')
        {
          AST.emplace_back(JSON_VAL::inplace(JSON_SYM::end_of_input, s.getPos()));
          return;
        }
      }

      std::cout << "ASSERT NOT REACHED ON CHAR " << cur;
      //std::cout << "SWITCH done, here must be a value or key\n";
    }
    AST.emplace_back(JSON_VAL::inplace(JSON_SYM::end_of_input, s.getPos()));
  }

  void PrintAST()
  {
    auto printStringResult = [&](StringResult& res) {
      std::cout << "\t\tStringDump: Str: '"<< res.str << "'; Start: '" << res.startPos << "'; end: '"<<res.endPos<<"'\n";
    };
    std::string endStr = ", hurray!\n";
    for(JSON_VAL &c: AST)
    {
      switch(c.Type)
      {
        case JSON_SYM::value_string:
          std::cout << "Value found.\n";
          printStringResult(c.value.str);
          break;
        case JSON_SYM::literal_true:
          std::cout << "literal_true at " << c.Pos << ", ";
          break;
        case JSON_SYM::literal_false:
          std::cout << "literal_false at " << c.Pos << ", ";
          break;
        case JSON_SYM::Unknown:
          std::cout << "uninitialised at " << c.Pos << ", ";
          break;
        case JSON_SYM::begin_object:
          std::cout << "begin_object at " << c.Pos << ", ";
          break;
        case JSON_SYM::begin_array:
          std::cout << "begin_array at " << c.Pos << ", ";
          break;
        case JSON_SYM::end_array:
          std::cout << "end_array at " << c.Pos << ", ";
          break;
        case JSON_SYM::end_object:
          std::cout << "end_object at " << c.Pos << ", ";
          break;
        case JSON_SYM::name_seperator:
          std::cout << "name_seperator at " << c.Pos << ", ";
          break;
        case JSON_SYM::value_seperator:
          std::cout << "value_seperator at " << c.Pos << ", ";
          break;
        case JSON_SYM::end_of_input:
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
