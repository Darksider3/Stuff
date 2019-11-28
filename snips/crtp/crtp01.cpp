#include "crtp01.h"

template<class T>
void CRTP01<T>::someInterface()
{
  under().someInterfaceImpl();
}


int main()
{
  CRTP01impl B;
}
