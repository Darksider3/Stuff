#include <iostream>
#include <bsd/stdlib.h> //lbsd

struct Stats
{
 size_t playerWins = 0;
 size_t computerWins = 0;
 size_t ties = 0;
};
/*
 * 1 - Rock
 * 2 - Paper
 * 3 - Scissor
 */
int main()
{
  std::string tiestr = "Tie! \n";
  std::string winstr = "You Win! \n";
  std::string loosestr = "You loose! \n";
  Stats Statistics;


  bool condition=true;
  std::string inputbuf;
  while(condition)
  {
    std::cout << "Rock, Paper, Scissors! Additional commands: Stats \n";
    std::cout << "Enter what YOU want to play: ";
    std::cin >> inputbuf;
    int num  = arc4random_uniform(3)+1;
    if(inputbuf == "Rock")
    {
      switch(num)
      {
        case 1:
          std::cout << tiestr;
          Statistics.ties++;
          break;
        case 2:
          std::cout << loosestr;
          Statistics.computerWins++;
          break;
        case 3:
          std::cout << winstr;
          Statistics.playerWins++;
          break;
      }
    }
    else if(inputbuf == "Paper")
    {
      switch(num)
      {
        case 1:
          std::cout << winstr;
          Statistics.playerWins++;
          break;
        case 2:
          std::cout << tiestr;
          Statistics.ties++;
          break;
        case 3:
          std::cout << loosestr;
          Statistics.computerWins++;
          break;
      }
    }
    else if(inputbuf == "Scissor")
    {
      switch(num)
      {
        case 1:
          std::cout << loosestr;
          Statistics.computerWins++;
          break;
        case 2:
          std::cout << winstr;
          Statistics.playerWins++;
          break;
        case 3:
          std::cout << tiestr;
          Statistics.ties++;
          break;
      }
    }
    else if(inputbuf == "quit")
      return 0;
    else if(inputbuf == "Stats")
    {
      std::cout << "Computer Wins: " << Statistics.computerWins << '\n';
      std::cout << "Player   Wins: " << Statistics.playerWins << '\n';
      std::cout << "Ties         : " << Statistics.ties << '\n';
      std::fflush(stdout);
    }
    else
      std::cout << "invalid input... did you write it correclty? \n";
    std::cin.clear();
  }
}
