#include <iostream>
#include <vector>
#include <algorithm> //std::sort
#include <map> //mode

struct VectorSort
{
  bool operator() (int i, int j) {return (i<j); }
} SortObj;

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
class Statistical
{}; //@TODO: Add everything down below
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

int median(std::vector<int> N)
{
  std::sort(N.begin(), N.end(), SortObj);
  N.erase( std::unique(N.begin(), N.end() ), N.end());
  return N[N.size()/2];
}


int mode(std::vector<int> N)
{
  std::sort(N.begin(), N.end(), SortObj);
  std::map<int, int> Modes;
  
  for(size_t i = 0; i != N.size(); ++i)
  {
    Modes[N[i]]++;
  }

  int mode = 0;
  int mode_freq = 0;

  for(auto it = Modes.begin(); it != Modes.end(); it++)
  {
    if(it->second > mode_freq)
    {
      mode = it->first;
      mode_freq = it->second;
    }
  }

  return mode;
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
  std::cout << "Median: " << median(A) << std::endl;
  std::cout << "Mode: " << mode(A) << std::endl;
  return 0;
}
