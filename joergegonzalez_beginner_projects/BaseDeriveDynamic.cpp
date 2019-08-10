#include <iostream>

class Base
{
  public:
    virtual void print()
    {
      std::cout << "BASE!" << std::endl;
    }
};

class Derive : public Base
{
  public:
  void print()
  {
    std::cout << "DERIVED!" << std::endl;
  }
};

int main()
{
  Base Tester;
  Base *Test = new Derive();
  Test->print();
  Tester.print();
}
