#include "crtp01.h"

template<class T>
void CRTP01<T>::someInterface()
{
  static_cast<T*>(this)->someInterfaceImpl();
}


int main()
{
  CRTP01<CRTP01impl> B;
  B.someInterface();
}
