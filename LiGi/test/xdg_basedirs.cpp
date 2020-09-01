#include "xdg_basedirs.h"

#include <iostream>

int main()
{
  xdg_basedirs *Ins = xdg_basedirs::instance();
  std::string bl = "/home/darksider3";
  if(Ins->valid_xdg_var(bl))
  {
    std::cout << "Valid";
  }
  else
  {
    std::cout << "invalid!";
  }

  auto vectorthing = Ins->DataDirs(xdg_basedirs::Dirs_List());
  std::cout << Ins->DataHome() << "\n";
  std::cout << Ins->DataDirs() << "\n";
  std::cout << "DataDirs: " << "\n";
  for(auto &ele: vectorthing)
    std::cout << "-> " << ele << std::endl;
  vectorthing = Ins->ConfigDirs(xdg_basedirs::Dirs_List());

  std::cout << "\nConfigDirs: " << std::endl;
  for(auto &ele: vectorthing)
    std::cout << "-> " << ele << std::endl;
  std::cout << "\n" << Ins->M_home << std::endl;

  return 1;
}
