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

  std::string PriceStr(struct Item const &IT)
  {
    std::string r;
    std::ostringstream Obj;
    Obj << std::fixed;
    Obj << std::setprecision(2);
    Obj << IT.price/ItemPriceDivisor;
    r+=Obj.str()+"€";
    return r;
  }

  std::string NameStr(struct Item const &IT)
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

std::vector<size_t> splitNums(std::string const &str)
{
  std::vector<size_t> ret;
  for(char c: str)
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


void output(std::vector<size_t> const &Vec)
{
  /* QT=Quantity
   * P/T=Price per Item
   * T/Q=Total for Item count
   */
  size_t ret = 0;
  FFmt col1(10,2), col2(10,2), col3(12,2);
  struct Item ItemNumerator[9];
  ItemNumerator[0] = MENU.first;
  ItemNumerator[1] = MENU.second;
  ItemNumerator[2] = MENU.third;
  ItemNumerator[3] = MENU.fourth;
  ItemNumerator[4] = MENU.fifth;
  ItemNumerator[5] = MENU.sixth;
  ItemNumerator[6] = MENU.seventh;
  ItemNumerator[7] = MENU.eights;
  ItemNumerator[8] = MENU.nineth;

  size_t Times[9]={0,0,0,0,0,0,0,0,0};
  std::vector<struct Item> ItemList;
  for(size_t N: Vec) {
    switch (N) {
      case 1:
        ret += MENU.first.price;
        ItemList.push_back(ItemNumerator[0]);
        Times[0]++;
        break;
      case 2:
        ret += MENU.second.price;
        ItemList.push_back(ItemNumerator[1]);
        Times[1]++;
        break;
      case 3:
        ret += MENU.third.price;
        ItemList.push_back(ItemNumerator[2]);
        Times[2]++;
        break;
      case 4:
        ret += MENU.fourth.price;
        ItemList.push_back(ItemNumerator[3]);
        Times[3]++;
        break;
      case 5:
        ret += MENU.fifth.price;
        ItemList.push_back(ItemNumerator[4]);
        Times[4]++;
        break;
      case 6:
        ret += MENU.sixth.price;
        ItemList.push_back(ItemNumerator[5]);
        Times[5]++;
        break;
      case 7:
        ret += MENU.seventh.price;
        ItemList.push_back(ItemNumerator[6]);
        Times[6]++;
        break;
      case 8:
        ret += MENU.eights.price;
        ItemList.push_back(ItemNumerator[7]);
        Times[7]++;
        break;
      case 9:
        ret += MENU.nineth.price;
        ItemList.push_back(ItemNumerator[8]);
        Times[8]++;
        break;
      default:
        break;
    }
  }

  std::cout << "YOUR INVOICE: \n";
  std::cout <<
            col1 << "QT" << col2 << "P/T" << col3 << "TIC\n";

  std::cout << std::setw(37) << std::setfill('=') << "\n" << std::setfill(' ');
  for(size_t i = 0; i != 8; ++i)
  {
    if(Times[i] != 0)
    {
      std::cout << "|" <<
            col1 << Times[i] << col2 << ItemNumerator[i].price/MENU.ItemPriceDivisor << "€" << col3 << (Times[i]*ItemNumerator[i].price)/MENU.ItemPriceDivisor << "€" << "|\n";
    }
  }
  std::cout << std::setw(37) << std::setfill('=') << "\n" << std::setfill(' ');
  std::cout << std::setw(40) << std::setfill(' ') << std::setprecision(2) << "TOTAL Items: " << ItemList.size();
  std::cout << std::setw(40) << std::setfill(' ') << std::setprecision(2) << "TOTAL Price: " << ret;
}


int main()
{
  
  populateMenue();
  std::cout.precision(2);
  std::string test = "121";
  std::vector<size_t> TestVec = splitNums(test);
  for(size_t &N: TestVec)
    std::cout << N << " ";
  std::cout << "\n";
  output(TestVec);
  
  return 0;
}
