#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

class Configparse
{
private:
  std::ifstream f;
  std::vector<std::vector<std::string>> Map;
  std::string strLine;
  unsigned int CharInLine;
  unsigned int ProccessedLines;
  std::string filename;
public:

  Configparse(std::string Fname="../test") : filename{Fname}
  {
    CharInLine=0;
    ProccessedLines=0;
    f.open(filename, std::ifstream::out);
  }
  bool getLine()
  {
    CharInLine = 0;
    return std::getline(f, strLine) && f.good();
  }

  std::string getstrLine()
  {
    return strLine;
  }

  char getch()
  {
    if(strLine.length() > CharInLine)
    {
      CharInLine=0;
      getLine();
      return strLine[CharInLine++];
    }
    else
      return strLine[CharInLine++];
  }

  unsigned long int filesize()
  {
    std::streampos fsize=0;
    std::ifstream file(filename, std::ios::binary);
    fsize=file.tellg();
    file.seekg(0, std::ios::end);
    fsize = file.tellg() - fsize;
    file.close();
    return fsize;
  }
  void parse()
  {
    int fsize = filesize();
    getLine();


  }

  void test()
  {
    std::vector<std::vector<std::string>>::iterator row;
    std::vector<std::string>::iterator col;
    for(int i=0; i < 5; i++)
    {
      std::vector<std::string> temp;
      temp.push_back("hi" + std::to_string(i));
      Map.push_back(temp);
    }
    for(row = Map.begin(); row != Map.end(); row++)
    {
      for (col = row->begin(); col != row->end(); col++)
      {
        std::cout << *col << "\n";
      }
    }
  }
};

int main()
{

  std::cout << "Hi! \n";
  Configparse b("../test");
  b.test();
  if(b.getLine())
    std::cout << b.getstrLine() << std::endl;
  if(b.getLine())
    std::cout << b.getstrLine() << std::endl;
  if(b.getLine())
    std::cout << b.getstrLine() << std::endl;
  else
    std::cout << "End!";
  return 0;
}
