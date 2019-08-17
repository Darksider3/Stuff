#include <iostream>
#include <vector>
#include <bsd/stdlib.h> //-lbsd
#include <algorithm> //std::find
class Hangman 
{
protected:
  std::string Word;
  std::vector<int> FoundCharacters; // holds the position of characters that are found; 
  std::vector<std::string> Dict;

public:
  Hangman(std::vector<std::string> Words, int x = -1)
  {
    Dict = Words;
    if(x == -1)
    {
      Word = Words.at( arc4random_uniform(Words.size()) );
    }
    else
    {
      Word = Words.at(x);
    }
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
  Hangman Game(Words, 1);
  std::cout << Game.getGameString() << std::endl;
  if(Game.FindLetter('e'))
  {
    std::cout << "Found! \n";
    std::cout << Game.getGameString() << "\n";
  }
  else
    std::cout << "Nope... \n";
}
