#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

class Configparse
{
private:
  std::ofstream f;
  std::vector<std::vector<std::string>> Map;
public:
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
  Configparse b;
  b.test();
  std::cout << std::endl;
  return 0;
}
