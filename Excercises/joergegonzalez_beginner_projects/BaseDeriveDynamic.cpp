#include <iostream>

namespace darkImpl
{
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
  void print() final
  {
    std::cout << "DERIVED!" << std::endl;
  }
};
}
int main()
{
  darkImpl::Base Tester;
  darkImpl::Base *Test = new darkImpl::Derive();
  Test->print();
  Tester.print();
}
