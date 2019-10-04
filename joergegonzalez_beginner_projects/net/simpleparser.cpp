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
  Tokenizer(std::string &F)
  {
    File=get_file_contents(F.c_str());
    Position = 0;
  }
  char next()
  {
    if(Position > File.size())
      return -1;

    return File[Position++];
  }
  bool isSeperator(); // Return if Char is a seperator, for json that would be ,(comma)
  bool isChar(const char &c, unsigned int pos);
    //return c == CURRENT_POS_CHAR;
  bool ignoreWhitespace();
    /*
    while(**cursor == '\t' || **cursor  == '\r' || **cursor == '\n' || **cursor == ' ') ++(*cursor);
    */
    // work on cursor, ignore everything that's a space or newline, exit on nullbyte.
    // return true until nullbyte
};
