#include <iostream>
#include <vector>
#include <bsd/stdlib.h> //-lbsd

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
  Words.push_back("");
  Words.push_back("");
}
