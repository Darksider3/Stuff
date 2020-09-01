#ifndef GENERALTOOLS_H
#define GENERALTOOLS_H
#include <string>
#include <vector>
#include <sstream> // istringstream
#include <iostream>

namespace Li
{
namespace GeneralTools
{

std::vector<std::string> split(const std::string& s, char delimiter)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter))
  {
    tokens.push_back(token);
  }
  return tokens;
}

}
}

#endif // GENERALTOOLS_H
