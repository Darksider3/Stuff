#ifndef ENVTOOLS_H
#define ENVTOOLS_H
#include <string>
#include <cstdlib> // char *std::getenv

namespace Li {
namespace Env
{

void empty_if_nullptr_env(const char *checkvar, std::string &strvar)
{
  if(checkvar == nullptr)
    strvar = "";
  else
    strvar = checkvar;
}

std::string get_env(std::string const &var)
{
  std::string ENV = "";
  empty_if_nullptr_env(std::getenv(var.c_str()), ENV);
  return ENV;
}

}
}
#endif // ENVTOOLS_H
