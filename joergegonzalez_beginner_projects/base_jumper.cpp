#include <iostream>


//....
void getAsBase(int Number, int destBase, std::string &Holder)
{
  if ( Number == 0 )
    return;
  int x = Number % destBase;
  Number /= destBase;
  if(x < 0)
    Number += 1;
  getAsBase(Number, destBase, Holder);
  Holder += x < 0 ? x + (destBase * -1) : x;
}

int main()
{
  //@TODO actual work pls
  std::string T;
  getAsBase(2, 2, T);
  std::cout << "Number" << T << std::endl;
  return 0;
}
