#include <iostream>
#include <bsd/stdlib.h> // arc4_random

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
public:
  explicit Dice() : ran(6)
  {}

  Dice(size_t upTo) : ran(upTo)
  {}

  size_t roll()
  {
    return ran.rand()+1;
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

  while(run)
  {
    std::cout << "How many rolls do you need? ";
    std::cin >> temp;
    Rolls = std::stoi(temp);
    temp.clear();

    for(size_t i = 0; i != Rolls; ++i)
    {
      DiceRoll = darkImp::Dice(Sides);
      std::cout << DiceRoll.roll() << ", ";
    }
    std::cout << std::endl;
  }
  return 0;
}
