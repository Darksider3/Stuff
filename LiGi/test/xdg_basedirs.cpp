#include "xdg_basedirs.h"
#include <iostream>
#include "Assertions.h"

#define DEBUG
int main()
{
  xdg_basedirs *Ins = xdg_basedirs::instance();
  auto Home = Ins->Home();
  auto CacheHome = Ins->CacheHome();
  auto DataHome = Ins->DataHome();

  std::cout << "Home: " << Home << std::endl;
  std::cout << "CacheHome: " << CacheHome << std::endl;
  std::cout << "DataHome: " << DataHome << std::endl;

  return 1;
}
