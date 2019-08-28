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
  Random(size_t from, size_t to) : FromNum{from}, ToNum{to}
  {}

  Random(size_t to) : ToNum{to}
  {
    FromNum = 0;
  }

  size_t rand()
  {
    return arc4random_uniform(ToNum) - FromNum;
  }
};

class Dice
{
protected:
  Random ran;
public:
  explicit Dice() : ran(6)
  {}

  size_t roll()
  {
    return ran.rand();
  }

};

}

int main()
{
  darkImp::Dice DiceRoll;
  std::cout << DiceRoll.roll();
  return 0;
}
