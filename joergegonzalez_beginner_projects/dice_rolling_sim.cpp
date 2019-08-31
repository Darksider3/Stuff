#include <iostream>
#include <ios> //std:: floating point output manipulation(fixed, scientific..)
#include <iomanip> //setprecision
#include <bsd/stdlib.h> // arc4_random
#include <map>
#include <vector>

namespace darkImp
{

class Random
{
protected:
  size_t FromNum;
  size_t ToNum;
public:

  Random(size_t to) : ToNum{to}
  {
    FromNum = 0;
  }

  size_t rand()
  {
    return arc4random_uniform(ToNum);
  }
};


class Dice
{
protected:
  Random ran;
  struct Statistics
  {
    std::map<size_t, size_t> Nums;
    size_t count = 0;
  } STATS;

public:
  explicit Dice() : ran(6)
  {}

  Dice(size_t upTo) : ran(upTo)
  {}

  size_t roll()
  {
    size_t Num = ran.rand()+1;
    STATS.Nums[Num]++;
    STATS.count++;
    return Num;
  }

  void print_stats(size_t precision = 4)
  {
    double percent = 0;
    std::setprecision(precision);
    for(std::pair<size_t, size_t> elem : STATS.Nums)
    {
      percent = ((float)elem.second*100)/STATS.count;
      std::cout << "Number: " << elem.first << " occured " << elem.second << " times! " <<
        "Thats " << std::fixed << percent <<"\% of the total!\n";
    }
  }

};

}

int main()
{
  bool run = true;
  size_t Sides = 0;
  size_t Rolls = 0;
  std::string temp;
  darkImp::Dice DiceRoll;
  std::cout << "How many numbers/sides has the dice? ";
  std::cin >> temp;
  Sides = std::stoi(temp);
  //@TODO: Keep asking if temp isnt a number!
  while(run)
  {
    std::vector<int> RanNums;
    std::cout << "How many rolls do you need? ";
    std::cin >> temp;
    Rolls = std::stoi(temp);
    temp.clear();
    DiceRoll = darkImp::Dice(Sides);
    for(size_t i = 0; i != Rolls; ++i)
    {
      RanNums.push_back(DiceRoll.roll());
    }
    for(int &N: RanNums)
    {
      std::cout << N << ", ";
    }
    std::cout << std::endl;
    DiceRoll.print_stats();
  }
  return 0;
}
