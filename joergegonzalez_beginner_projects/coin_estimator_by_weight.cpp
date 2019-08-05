#include <iostream>
#include <cmath> //round

struct Coin
{
  std::string name;
  size_t value = 1; // Value in cents
  float weight = 2;
  size_t wrapperSize = 50;
};


int estimateWrappers(struct Coin const c, size_t totalWeight)
{
  /* Pennies. 5g each, 50 per wrapper. Total 500g
   * (500/5)=100/50=2
   */
  return (totalWeight / c.weight) / c.wrapperSize;
}

int main()
{
  std::string inputbuf;
  while(true)
  {
    struct Coin CoinType;
    size_t fullWeight;
   
    std::cout << "Which type you want an estimate from? Answer: ";
    
    std::cin >> inputbuf;
    CoinType.name = inputbuf;
    std::cin.clear();
    inputbuf = "";
    
    std::cout << "Which value does it have: ";

    std::cin >> inputbuf;
    CoinType.value = std::stoi(inputbuf);
    std::cin.clear();
    inputbuf = "";

    std::cout << "Weight of a single Coin: ";
    
    std::cin >> inputbuf;
    CoinType.weight = std::stof(inputbuf);
    std::cin.clear();
    inputbuf = "";

    std::cout << "Total weight of your coins: ";

    std::cin >> inputbuf;
    fullWeight = std::stof(inputbuf);
    std::cin.clear();
    inputbuf = "";

    std::cout << "\n\nYou could fill " << estimateWrappers(CoinType, fullWeight) << " wrappers with your " << CoinType.name << 
      ", and got a total of " << round(fullWeight/CoinType.weight) << " Coins, which have a total value of " << 
      (float)( ((fullWeight/CoinType.weight)*CoinType.value) / 100) <<" $Currency.\n\n";
    
    std::fflush(stdout);
  }
}
