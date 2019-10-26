#include <iostream>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <cstring>
//#include <zconf.h>

// @TODO: General Exception as base class?
namespace ParserAndTokenExceptions
{

#include <exception>
class GeneralException : public std::exception
{
private:
  std::string msg;
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

  virtual const char* what() const throw()
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
  uninitialised,
  literal_true,
  literal_false,
  value_string,
  value_unsigned,
  value_integer,
  value_float,
  begin_array,
  begin_object,
  end_array,
  end_object,
  name_seperator, // In JSON it's actually just a .. : lel
  value_seperator, // here it is the , (comma)
  parse_error,
  end_of_input
};

struct JSON_VAL
{
  std::vector<JSON_VAL> ArryObj;
  JSON_SYM Type;
  union VAL
  {
    bool boolean;
    double number;
    char *str;
    char *key;
  } value;
  int Pos;

  static JSON_VAL inplace(JSON_SYM t, size_t pos=0)
  {
    JSON_VAL tmp = {.Type=t, .Pos=static_cast<int>(pos)};
    tmp.Type = t;
    tmp.Pos = pos;
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

  unsigned char nextTok()
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

  bool checkSize(size_t const num) const
  {
    return (num >= File.size());
  }

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

  size_t lookForChar(char const ch)
  {
    size_t ret=0, oldPos=Position;
    while(File[Position] != ch)
    {
      ++Position;
      ++ret;
    }
    Position=oldPos;
    return ret;
  }

  // @TODO: Exception.
  std::string getStrFromTo(size_t const frompos, size_t const topos)
  {
    if(frompos < 0 || topos > File.size())
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
  
  struct NumberResult
  {
    std::string str;
    bool hasDot = false;
    bool hasE = false;
  };

  // @TODO: Exception. Now.
  struct NumberResult getNumber(size_t const FromPos)
  {
    if(FromPos >= File.size())
    {
      throw(ParserAndTokenExceptions::TokenSizeException("Requested starting position is out of bounds in: ", __LINE__));
    }

    size_t cursor=FromPos;
    NumberResult res = {.hasDot=false };
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
    std::cout << "NUM: "<< temp << "\n";
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
  std::string getStrOnPos(size_t const startingPos)
  {
    if(startingPos >= File.size())
    {
      throw(ParserAndTokenExceptions::TokenSizeException("Requested starting position is out of bounds of your given file in line: ", __LINE__));
    }

    size_t oldPos = Position;
    bool escaped = false;
    std::string temp;
    char StrStarter = peek(startingPos);
    unsigned char cur;
    if(!isQuote(StrStarter))
    {
      throw(ParserAndTokenExceptions::ParserUnknownCharacterEncountered("Expected to encouter \' or \", but instead got something else("+std::to_string(StrStarter)+
            ") in line: ", __LINE__));
    }
    // looks like a string?

    bool runner = true;
    for(unsigned int i = startingPos+1; runner && !eof(); ++i)
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
    Position = oldPos;
    return temp;
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

  char peek(size_t const Pos) const
  {
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
    auto emit = [this](auto Type) {
      AST.emplace_back(JSON_VAL::inplace(Type, s.getPos()));
    };
    while(!s.eof())
    {
      unsigned char cur = s.nextTok();
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
        default:
          break;
      }

      // if string
      // @TODO simplify by putting in a function dafuq is this going to get complex
      if(cur == '"' || cur == '\'')
      {
        std::string temp = s.getStrOnPos(s.getPos()-1);
        std::cout << "getStrOnPos(): '" << temp << " ' <- STR\n";
        s.setPos(s.getPos()+temp.size()+1);
      }
      // it should be a number now
      else
      {
        struct Tokenizer::NumberResult num;
        try
        {
          num = s.getNumber(s.getPos()-1);
        }
        catch(ParserAndTokenExceptions::TokenSizeException& e)
        {
          std::cout << "Oh, it seems like our file has an end here! We take that and exit gracefully. \n";
          break;
        }
        if(num.str.empty())
        {
          if(cur == '\0')
          {
            break;
          }
          else
          {
            std::cout << "Parser Error. '" << cur << "' \n";
          }
        }
        else
        {
          s.setPos(s.getPos()+num.str.size()+1);
          // ok, it's definitly a number. Detect if it's a floating point or an integer and go add it!
        }

      }
      //std::cout << "SWITCH done, here must be a value or key\n";
    }

    AST.emplace_back(JSON_VAL::inplace(JSON_SYM::end_of_input, s.getPos()));
  }

  void PrintAST()
  {
    std::string endStr = ", hurray!\n";
    for(JSON_VAL &c: AST)
    {
      switch(c.Type)
      {
        case JSON_SYM::literal_true:
          std::cout << "literal_true at " << c.Pos << ", ";
          break;
        case JSON_SYM::literal_false:
          std::cout << "literal_false at " << c.Pos << ", ";
          break;
        case JSON_SYM::uninitialised:
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
