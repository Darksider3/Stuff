#include <iostream>
#include <cstdio>
#include <cerrno>
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

enum JSON_TYPE {
  TYPE_NULL,
  TYPE_BOOL,
  TYPE_NUMBER,
  TYPE_OBJECT,
  TYPE_ARRAY,
  TYPE_STRING,
  TYPE_KEY
};

struct json_val
{
  int T;
  union
  {
    bool boolean;
    double number;
    std::string str;
    std::string key;
    //std::vector array;
    //std::vector object;
  } value;
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
  
public:
  Parser(const std::string &c) : s{c}
  {
    s = Tokenizer(c);
  }

  void parse()
  {
    while(s.eof())
    {
      std::cout << s.nextTok();
    }
  }
};


int main()
{
  Parser s{"./example.json"};
  s.parse();
  std::cout << std::endl;
}
