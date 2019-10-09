#include <iostream>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <zconf.h>


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

  size_t size()
  {
    return File.size();
  }

  //@TODO Bounds checking File.size();
  //@TODO Also exception
  void setPos(size_t pos)
  {
    Position = pos;
  }
  
  size_t getPos()
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

  bool eof()
  {
    return File.size() <= Position;
  }

  /*
   * HELPERS
   * @TODO: own namespace? Won't use this-> than i think o-o
   */

  bool checkSize(size_t num)
  {
    return (num >= File.size());
  }

  size_t countNewlineUntilPos(size_t pos)
  {
    if(pos >= File.size())
      return -1;
    size_t cursor = 0;
    size_t counter = 0;
    for(; cursor != pos; ++cursor)
    {
      if(File[cursor] == '\n')
        counter++;
    }

    return counter;
  }

  size_t lookForChar(char ch)
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
  std::string getStrFromTo(size_t frompos, size_t topos)
  {
    if(frompos < 0 || topos > File.size())
      return std::string("");
    if(frompos < topos)
      return std::string("");
    std::string ret;

    for(size_t i=frompos; i != topos;  ++i)
    {
      ret += File[i];
    }
    return ret;
  }

  // @TODO: Exception. Now.
  std::string getNumber(size_t FromPos)
  {
    if(FromPos >= File.size())
      return "";

    size_t cursor=FromPos;
    bool runner=true, hasE=false;
    std::string temp;
    for(;runner; ++cursor)
    {
      if(std::isdigit(File[cursor]) || File[cursor] == '.')
        temp+=File[cursor];
      else if(hasE == false && (File[cursor] == 'e' || File[cursor] == 'E'))
      {
        hasE = true;
        temp+=File[cursor];
      }
      else if(hasE == true && (File[cursor] == 'e' || File[cursor] == 'E'))
      {
        std::cout << "ERROR! Multiple expotentials in one number!\n";
        exit(123);
      }
      else if(File[cursor] == '-' || File[cursor] == '+')
      {
        temp+=File[cursor];
      }
      else
        runner=false;
    }
    std::cout << "NUM: "<< temp << "\n";
    return temp;
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
  std::string getStrOnPos(size_t startingPos)
  {
    if(startingPos >= File.size())
    {
      return "";
    }

    size_t oldPos = Position;
    bool escaped = false;
    std::string temp;
    char StrStarter = getChar(startingPos);
    unsigned char cur;
    if(!isQuote(StrStarter))
    {
      return "";
    }
    // looks like a string?

    bool runner = true;
    for(unsigned int i = startingPos+1; runner && !eof(); ++i)
    {
      cur = getChar(static_cast<size_t>(i));
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

  bool isQuote(size_t Pos)
  {
    if(Pos >= File.size())
      return false;

    return isQuote(File[Pos]); 
  }

  static bool isQuote(char ch)
  {
    return (ch == '\'' || ch == '"');
  }

  char getChar(size_t Pos)
  {
    if(Pos >= File.size())
      return '\0';

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
    while(!s.eof())
    {
      unsigned char cur = s.nextTok();
      switch(cur)
      {
        case '{':
          AST.emplace_back(JSON_VAL::inplace(JSON_SYM::begin_object, s.getPos()));
          continue;
        case '[':
          AST.emplace_back(JSON_VAL::inplace(JSON_SYM::begin_array, s.getPos()));
          continue;
        case '}':
          AST.emplace_back(JSON_VAL::inplace(JSON_SYM::end_object, s.getPos()));
          continue;
        case ']':
          AST.emplace_back(JSON_VAL::inplace(JSON_SYM::end_array, s.getPos()));
          continue;
        case ',':
          AST.emplace_back(JSON_VAL::inplace(JSON_SYM::value_seperator, s.getPos()));
          continue;
        case ':':
          AST.emplace_back(JSON_VAL::inplace(JSON_SYM::name_seperator, s.getPos()));
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
      else
      {
        std::string num = s.getNumber(s.getPos()-1);
        if(num.empty())
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
          std::cout <<" NUMBERNUMBER\n";
          s.setPos(s.getPos()+num.size()+1);
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
namespace ParserAndTokenExceptions
{

#include <exception>

class TokenSizeException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "size exceeds File bounds!";
  }
} tse;

class TokenNumberMultipleExponentException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Number has multiple exponents";
  }
} tnmee;

class TokenStringWithoutEnding : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Detected string has no end in file...";
  }
} tswe;

class ParserUnknownCharacterEncountered : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Parser encountered unknown character";
  }
} puce;
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
