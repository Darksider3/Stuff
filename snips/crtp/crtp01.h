#ifndef CRTP01_H
#define CRTP01_H
#include <iostream>

template<class T>
class CRTP01
{
public:
  CRTP01()
  {
    someInterface();
    under().blabla();
  }
  void someInterface();

  T& under() {return static_cast<T&>(*this);}
};

class CRTP01impl : public CRTP01<CRTP01impl>
{
public:
  void someInterfaceImpl()
  {
    std::cout << "Huh, somehow this worked lol\n";
  }
  void blabla()
  {
    std::cout << "blabla\n";
  }
};

#endif // CRTP01_H
