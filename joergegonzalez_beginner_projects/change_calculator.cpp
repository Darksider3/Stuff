#include <iostream>
namespace darkImpl
{
struct Result
{
  int dollars;
  int quarters;
  int dimes;
  int nickels;
  int pennies;
};


struct Result &calcReturn(int change, struct Result &ret)
{
  ret.dollars = change/100;
  ret.quarters = change%100/25;
  ret.dimes  = change%100%25/10;
  ret.nickels = change%100%25%10/5;
  ret.pennies = change%100%25%10%5;
  return ret;
}

void dumpResult(struct Result const re)
{
  std::cout << "Dollars: " << re.dollars << "\nQuartes: " << re.quarters << "\nDimes: "<< re.dimes << "\nNickels: " << re.nickels << "\n";
  std::cout << "Pennies: " << re.pennies << "\n";
}
}

int main()
{
  std::string inputbuf;
  struct darkImpl::Result change;
  bool runcondition = true;
  

  std::cout << "Press q to exit \n\n";
  while(runcondition)
  {
    std::cout << "Enter your change: ";
    std::cin >> inputbuf;
    if(inputbuf == "q")
    {
      runcondition = false;
      continue;
    }
    change = darkImpl::calcReturn(std::stof(inputbuf)*100, change);
    darkImpl::dumpResult(change);
    std::cin.clear();
  }
}
