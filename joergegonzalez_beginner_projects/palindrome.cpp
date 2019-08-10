#include <iostream>
#include <string>
#include <cctype>

/*
 * A palindrome is a word that is the same regardless you read forward or backwards.
 */
bool isPalindrome(std::string const word)
{
  char start, end;
  for(size_t i = 0; i != word.length(); ++i)
  {
    start = tolower(word[i]);
    end = tolower(word[word.length()-1-i]);
    if(start != end)
      return false;
  }
  return true;
}

int main(int argc, char **argv)
{
  std::string helper;
  helper=argv[1];
  if(isPalindrome(helper))
    std::cout << "yepp!";
  else
    std::cout << "nope!";
  std::cout << std::endl;
}
