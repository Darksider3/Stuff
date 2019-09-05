#include <iostream>
#include <vector>
#include <ios> // std::fixed
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

void buildVisualInvoice()
{
  //@TODO: Build invoice with a table (Quantity, Product_Name, sing cost, total cost)
  //https://stackoverflow.com/questions/11226143/formatting-output-in-c
}

size_t calc(std::vector<size_t> Numbers)
{
  size_t ret=0;
  for(size_t &N: Numbers)
  {
    switch(N)
    {
      case 1:
        ret+=MENU.first.price;
        break;
      case 2:
        ret+=MENU.second.price;
        break;
      case 3:
        ret+=MENU.third.price;
        break;
      case 4:
        ret+=MENU.fourth.price;
        break;
      case 5:
        ret+=MENU.fifth.price;
        break;
      case 6:
        ret+=MENU.sixth.price;
        break;
      case 7:
        ret+=MENU.seventh.price;
        break;
      case 8:
        ret+=MENU.eights.price;
        break;
      case 9:
        ret+=MENU.nineth.price;
        break;
      default:
        break;
    }
  }

  return ret;
}
int main()
{
  
  populateMenue();
  std::cout.precision(2);
  std::string test = "111";
  std::vector<size_t> TestVec = splitNums(test);
  for(size_t &N: TestVec)
    std::cout << N << " ";
  std::cout << "\n";
  std::cout << "Calc: " << std::fixed << calc(TestVec)/MENU.ItemPriceDivisor;
  std::cout << "\n";
  return 0;
}
