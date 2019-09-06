#include <iostream>
#include <vector>
#include <ios> // std::fixed

#include <iomanip> //width

class FFmt
{
public:
  const int mWidth;
  const int mPrecision;
  FFmt(const int width, const int precision):mWidth(width), mPrecision(precision)
  {
  }

  int getPrecision()
  {
    return mPrecision;
  }

  int getWidth()
  {
    return mWidth;
  }

  friend std::ostream &operator<< (std::ostream& dest, FFmt & fmt)
  {
    dest.precision(fmt.getPrecision());
    dest.width(fmt.getWidth());
    dest.setf(std::ios_base::right, std::ios_base::uppercase);
    dest.flags(std::ios_base::uppercase);
    return dest;
  }

};

class FFmt2
{
  int Width;
  int Precision;

public:
  FFmt2(int width, int precision) : Width(width), Precision(precision)
  {}

  int getWidth()
  { return Width; }
  int getPrecision()
  { return Precision; }
template<typename T>
  void format(std::ostream& dest, T arg)
  {
    dest.precision(getPrecision());
    dest.setf(std::ios_base::right, std::ios_base::uppercase);
    dest.flags(std::ios_base::uppercase);
    dest.width(getWidth());
    dest << arg;
  }
};

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

  std::string PriceStr(struct Item IT)
  {
    std::string r;
    std::ostringstream Obj;
    Obj << std::fixed;
    Obj << std::setprecision(2);
    Obj << IT.price/ItemPriceDivisor;
    r+=Obj.str()+"€";
    return r;
  }

  std::string NameStr(struct Item IT)
  {
    std::string r;
    r = IT.name;
    return r;
  }

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

void output(std::vector<size_t> Vec)
{
  /* QT=Quantity
   * P/T=Price per Item
   * T/Q=Total for Item count
   */
  FFmt col1(10,2), col2(10,2), col3(10,2);
  std::cout << "YOUR INVOICE: \n";
  std::cout << 
    col1 << "QT" << col2 << "P/T" << col3 << "TIC\n" <<
    col1 << "TO" << col2 << "DO:" << col3 << "HER" <<
    "\n";
  std::cout << std::setw(40) << std::setfill(' ') << std::setprecision(2) << "TOTAL: " << "TODO\n";
}


void formatTest()
{
  FFmt b1(10, 2), b2(10, 2);
  std::cout << 
    b1 << "Hi!" << b2 << "World!" << "\n" <<
    b1 << "No!" << b2 << "Wrarr!" << "\n";
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
  FFmt col1(10, 1);
  FFmt col2(11, 1);
  output(TestVec);
  
  formatTest();
  return 0;
}
