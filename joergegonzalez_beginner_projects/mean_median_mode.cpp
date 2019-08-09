#include <iostream>
#include <vector>

size_t range(std::vector<float> R)
{
  return R[0];
}

void trimChar(std::string &Source, char trim)
{
  std::string ret;
  for(size_t i = 0; i != Source.length(); ++i)
  {
    if(Source[i] != trim)
      ret+=Source[i];
  }
  Source = ret;
}


std::vector<int> seperateByDelimiter(std::string str, const char delimiter)
{
  std::vector<int> Ret;
  trimChar(str, ' ');
  std::cout << "str after trimChar: " << str << std::endl;
  std::string buf;
  for(char &i: str)
  {
    if(std::isdigit(i) && !std::isalpha(i))
    {
      buf+=i;
    }
    if(i == delimiter)
    {
      Ret.push_back(std::stoi(buf));
      buf = "";
    }
  }
  
  Ret.push_back(std::stoi(buf));
  return Ret;
}

double mean(std::vector<int> N)
{
  int Mean = 0;
  int i=0;
  for(int &Num: N)
  {
    Mean+=Num;
    i++;
  }
  //std::cout << Mean << "/" << i << " = " << Mean/i << '\n';
  return Mean/i;
}

int main()
{
  std::string inputbuf;
  std::cout << "Insert a set of numbers seperated by commata. Will output mean, median, mode and Range! \n";
  std::cout << "Numbers: ";
  
  std::getline(std::cin, inputbuf);
  trimChar(inputbuf, ' ');
  std::cout << inputbuf << std::endl;
  std::vector<int> A = seperateByDelimiter(inputbuf, ',');
  std::cout << "Mean: " << mean(A) << std::endl;
  return 0;
}
