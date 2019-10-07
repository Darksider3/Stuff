#include <iostream>
#include <cstdio>
#include <cerrno>
#include <vector>
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

enum class JSON_TYPE {
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
  JSON_TYPE Type;
  union VAL
  {
    bool boolean;
    double number;
    char *str;
    char *key;
  } value;

  static JSON_VAL inplace(JSON_TYPE t)
  {
    JSON_VAL tmp;
    tmp.Type=t;
    return tmp;
  }
};


class Tokenizer
{
private:
  std::string File;
  size_t Position;
public: 
  Tokenizer(const std::string &F)
  {
    File=get_file_contents(F.c_str());
    Position = 0;
  }

  size_t lookForChar(char ch)
  {
    size_t ret=0, oldPos=Position;
    while(File[Position] != ch)
    {
      Position++;
      ret++;
    }
    Position=oldPos;
    return ret;
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

  void setPos(size_t pos)
  {
    Position = pos;
  }
  
  size_t getPos()
  {
    return Position;
  }

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
  unsigned char nextTok()
  {
    if(!ignoreWhitespace())
      return 0;
    return File[Position++];
  }

  bool eof()
  {
    return File.size() > Position;
  }
  bool isSeperator(); // Return if Char is a seperator, for json that would be ,(comma)
  bool isChar(const char &c, unsigned int pos);
    //return c == CURRENT_POS_CHAR;
    /*
    while(**cursor == '\t' || **cursor  == '\r' || **cursor == '\n' || **cursor == ' ') ++(*cursor);
    */
    // work on cursor, ignore everything that's a space or newline, exit on nullbyte.
    // return true until nullbyte
};

class Parser
{
private:
  Tokenizer s;
  std::vector<JSON_VAL> AST;
public:
  Parser(const std::string &c) : s{c}
  {
    s = Tokenizer(c);
    // @TODO: check for syntax - { at start, } at end required.
  }

  void parse()
  {
    while(s.eof())
    {
      char cur = s.nextTok();
      switch(cur)
      {
        case '{':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::begin_array));
          break;
        case '[':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::begin_object));
          break;
        case '}':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::end_array));
          break;
        case ']':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::end_object));
          break;
        default:
          break;
      }
    }
  }

  void PrintAST()
  {
    std::string endStr = ", hurray!\n";
    for(JSON_VAL &c: AST)
    {
      switch(c.Type)
      {
        case JSON_TYPE::literal_true:
          break;
        case JSON_TYPE::literal_false:
          break;
        case JSON_TYPE::uninitialised:
          break;
        case JSON_TYPE::begin_object:
        case JSON_TYPE::begin_array:
          std::cout << "Array/Object starts"+endStr;
          break;
        case JSON_TYPE::end_array:
        case JSON_TYPE::end_object:
          std::cout << "Array/Object ends"+endStr;
          break;
        default:
          std::cout << "something else!" << std::endl;
      }
    }
  }
};


int main()
{
  JSON_VAL b;
  b.Type = JSON_TYPE::begin_array;
  Parser s{"./example.json"};
  s.parse();
  s.PrintAST();
  std::cout << std::endl;

}
