#include <iostream>
#include <vector>

size_t range(std::vector<float> R)
{
  return R[0];
}

void delSpace(std::string &Source)
{
  std::string ret;
  for(char &i: Source)
  {
    if(!std::isspace(i))
    {
      ret+=i;
    }
  }
}

std::vector<int> seperateByDelimiter(std::string const str, const char delimiter)
{
  std::vector<int> Ret;
  return Ret;
}

int main()
{
  std::string inputbuf;
  std::cout << "Insert a set of numbers seperated by commata. Will output mean, median, mode and Range! \n";
  std::cout << "Numbers: ";
  
  std::cin >> inputbuf;

  return 0;
}
