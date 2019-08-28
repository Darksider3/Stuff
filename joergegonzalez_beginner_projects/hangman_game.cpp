#include <iostream>
#include <vector>
#include <bsd/stdlib.h> //-lbsd
#include <algorithm> //std::find
#include <stdio.h> //getch()

class Hangman 
{

protected:
  std::string Word;
  std::vector<int> FoundCharacters; // holds the position of characters that are found; 
  std::vector<std::string> Dict;

public:
  struct Statistic
  {
    size_t trys = 0;
    size_t wins = 0;
    size_t looses = 0;
  } STATS;

  Hangman(std::vector<std::string> Words, int x = -1) : Dict{Words}
  {
    if(x == -1)
    {
      Word = Words.at( arc4random_uniform(Words.size()) );
    }
    else
    {
      Word = Words.at(x);
    }
    Dict = Words;
  }

  void New()
  {
    Word = Dict.at(arc4random_uniform(Dict.size()));
    FoundCharacters.clear();
  }
  std::string getGameString()
  {
    std::string Ret;
    for(size_t i = 0; i != Word.length(); ++i)
    {
      if( std::find(FoundCharacters.begin(), FoundCharacters.end(), i) != FoundCharacters.end())
      {
        Ret += Word.at(i);
      }
      else
      {
        Ret += '_';
      }
    }
    return Ret;
  }

  bool FindLetter(char c)
  {
    if(Won())
      return true;

    bool ret=false;
    size_t oldSize = FoundCharacters.size();
    for(size_t i = 0; i != Word.length(); ++i)
    {
      if(Word.at(i) == c)
      {
        FoundCharacters.push_back(i);
      }
    }
    if(oldSize != FoundCharacters.size())
    {
      ret = true;
    }
    return ret;
  }

  bool Won()
  {
    if(Word.size() == FoundCharacters.size())
      return true;
    else
      return false;
  }
};


int main()
{
  std::vector<std::string> Words;
  Words.push_back("Hangman");
  Words.push_back("Oversee");
  Words.push_back("Towel");
  Words.push_back("Watchtover");
  Words.push_back("Insertion");
  Words.push_back("Ladybird");
  Words.push_back("Test");
  Words.push_back("Halleluja!");
  Hangman Game(Words);
  /*std::cout << Game.getGameString() << std::endl;
  if(Game.FindLetter('e'))
  {
    std::cout << "Found! \n";
    std::cout << Game.getGameString() << "\n";
  }
  else
    std::cout << "Nope... \n";
  */
  {
    bool run = true;
    char Guess;
    while(run)
    {
      std::cout << "Current: \n" << Game.getGameString() << "\n";
      std::cout << "Your guess: ";
      Guess = getchar();
      std::cout << "\n";
      if(Game.FindLetter(Guess))
      {
        if(Game.Won())
        {
          std::cout << "Congratulations, you won! The word was: '" << Game.getGameString() << "\n";
          std::cout << "Running again! \n";
          Guess = '.';
          Game.New();
        }
        std::cout << "You found some letters! \n";
      }
      else
        std::cout << "Sorry, " << Guess << " isn't in that word :/ \n";
    }
  }
}
