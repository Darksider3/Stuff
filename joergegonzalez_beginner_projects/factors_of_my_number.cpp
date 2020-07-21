#include <iostream>
#include <vector>
#include <concepts>
#include <cstddef>
#include <sstream>

using namespace std::literals;
template<typename T>
concept Integral = std::is_integral<T>::value;

std::vector<double> Factors(Integral auto Number, std::vector<double> &Results)
{
  for(int i = 1; i <= Number; ++i)
  {
    if(Number % i == 0)
    {
      Results.push_back(i);
    }
  }

  return Results;
}

// C++20, Math not neccessary
// g++ -std=2a -lm



int main()
{
  int Number = 20;

  std::cout << "Number to find factors from: ";
  std::cin >> Number;

  std::stringstream Result;
  Result << "Factors of " << Number << " are: ";
  std::vector<double> Facts = std::vector<double>();
  Factors(Number, Facts);


  for(double &Num: Facts)
  {
    if(Num == Number)
      break;
    Result << Num;
    Result << ", ";
  }

  std::cout << Result.str() << Number << "!" << std::endl;

  return 0;
}
