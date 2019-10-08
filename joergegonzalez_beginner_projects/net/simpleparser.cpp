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
  int Pos;

  static JSON_VAL inplace(JSON_TYPE t, size_t pos=0)
  {
    JSON_VAL tmp;
    tmp.Type = t;
    tmp.Pos = pos;
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
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::begin_array, s.getPos()));
          continue;
          break;
        case '[':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::begin_object, s.getPos()));
          continue;
          break;
        case '}':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::end_array, s.getPos()));
          continue;
          break;
        case ']':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::end_object, s.getPos()));
          continue;
          break;
        case ',':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::value_seperator, s.getPos()));
          continue;
          break;
        case ':':
          AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::name_seperator, s.getPos()));
          continue;
          break;
        default:
          break;
      }
      JSON_TYPE curSym = AST.at(AST.size()-1).Type;
      if(curSym == JSON_TYPE::name_seperator)
      {
        // @TODO: Left should be a ", ' or number, get that. Ignore every escaped character! 
        std::cout << "Got Name Seperator ";
        if(cur == '"')
          std::cout << "and a \"";
        std::cout << "\n";
      }
      //std::cout << "SWITCH done, here must be a value or key\n";
    }

    AST.emplace_back(JSON_VAL::inplace(JSON_TYPE::end_of_input, s.getPos()));
  }

  void PrintAST()
  {
    std::string endStr = ", hurray!\n";
    for(JSON_VAL &c: AST)
    {
      switch(c.Type)
      {
        case JSON_TYPE::literal_true:
          std::cout << "literal_true at " << c.Pos << ", ";
          break;
        case JSON_TYPE::literal_false:
          std::cout << "literal_false at " << c.Pos << ", ";
          break;
        case JSON_TYPE::uninitialised:
          std::cout << "uninitialised at " << c.Pos << ", ";
          break;
        case JSON_TYPE::begin_object:
          std::cout << "begin_object at " << c.Pos << ", ";
          break;
        case JSON_TYPE::begin_array:
          std::cout << "begin_array at " << c.Pos << ", ";
          break;
        case JSON_TYPE::end_array:
          std::cout << "end_array at " << c.Pos << ", ";
          break;
        case JSON_TYPE::end_object:
          std::cout << "end_object at " << c.Pos << ", ";
          break;
        case JSON_TYPE::name_seperator:
          std::cout << "name_seperator at " << c.Pos << ", ";
          break;
        case JSON_TYPE::value_seperator:
          std::cout << "value_seperator at " << c.Pos << ", ";
          break;
        case JSON_TYPE::end_of_input:
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
  b.Type = JSON_TYPE::begin_array;
  Parser s{"./example.json"};
  s.parse();
  s.PrintAST();
  std::cout << std::endl;

}
