#include <iostream>
#include <bsd/stdlib.h> // arc4random_uniform ----- -lbsd

class Higher_Lower
{
  int Val;
  public:
  Higher_Lower()
  {
    newNumber();
  }

  void newNumber()
  {
    Val = arc4random_uniform(10);
  }

  int getNum()
  {
    return Val;
  }
};

int main()
{
  std::string Intro = "Hi! This game is about higher/lower guessing! The computer will generate a number between 1 and 10 and u have to guess it by inputing" 
    "another number. The computer will tell you if it's higher or lower than his number. Rinse and repeat till your number is found. Press q to exit.\n\n";

  std::string WinMsg = "Yay you won! \n\n";
  size_t guessesTaken = 0;
  std::string inputbuf = "";
  int guess;
  Higher_Lower Game{};
  std::cout << Intro;
  Game.newNumber();
  while(true)
  {
    std::cout << "Your guess: ";
    std::cin >> inputbuf;
    
    if(inputbuf == "q")
      break;

    guess = std::stoi(inputbuf);
    std::cin.clear();
    inputbuf = "";
    
    guessesTaken++;
    if(guess > 10)
    {
      std::cout << "Sorry, just between 1-10! \n";
    }
    else if(guess == Game.getNum())
    {
      std::cout << WinMsg;
      std::cout << "it took you " << guessesTaken << "tries! \n";
      guessesTaken = 0;
      Game.newNumber();
    }
    else if(guess > Game.getNum())
    {
      std::cout << "Sorry, the number we look for is lower! \n";
    }
    else if(guess < Game.getNum())
    {
      std::cout << "Sorry, the number we look for is HIGHER! \n";
    }
  }

}
