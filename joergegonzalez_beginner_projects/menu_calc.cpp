#include <iostream>
#include <vector>
struct Item
{
  std::string name;
  size_t price;
};

struct Menu
{
  struct Item first;
  struct Item second;
  struct Item third;
  struct Item fourth;
  struct Item fifth;
  struct Item sixth;
  struct Item seventh;
  struct Item eights;
  struct Item nineth;
  double ItemPriceDivisor = 100.0;
} MENU;

void populateMenue()
{
  MENU.first.name = "Chicken Stripes";
  MENU.first.price = 350;
  
  MENU.second.name = "French Fries";
  MENU.second.price = 250;

  MENU.third.name = "Hamburger";
  MENU.third.price = 400;

  MENU.fourth.name = "Hotdog";
  MENU.fourth.price = 350;

  MENU.fifth.name = "Large Drink";
  MENU.fifth.price = 175;

  MENU.sixth.name = "Medium Drink";
  MENU.sixth.price = 150;

  MENU.seventh.name = "Milk Shake";
  MENU.seventh.price = 225;

  MENU.eights.name = "Salad";
  MENU.eights.price = 375;

  MENU.nineth.name = "Small Drink";
  MENU.nineth.price = 125;
}

std::vector<size_t> splitNums(std::string str)
{
  std::vector<size_t> ret;
  for(char &c: str)
  {
    if(std::isdigit(c))
      ret.push_back(c-'0');
    else
      continue; // current char is invalid
  }
  return ret;
}

int main()
{
  populateMenue();
  std::string test = "123123123";
  std::vector<size_t> TestVec = splitNums(test);
  for(size_t &N: TestVec)
    std::cout << N << " ";
  std::cout << "\n";
  return 0;
}
