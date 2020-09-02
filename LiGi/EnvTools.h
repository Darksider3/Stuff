#ifndef ENVTOOLS_H
#define ENVTOOLS_H
#include <string>
#include <cstdlib> // char *std::getenv

namespace Li {
namespace Env
{

/**
 * @brief empty_if_nullptr_env Is, as the name suggests, currently only a helper function
 *
 * It sets strvar to either empty, in case the given char* is a nullptr`/`NULL`, or
 * to the contents of it.
 *
 * @param checkvar Variable to check content
 * @param strvar Variable to store result in.
 * @warning The second argument gets replaced with the contents of the first or set to empty!
 *
 */
inline void empty_if_nullptr_env(const char *checkvar, std::string &strvar) noexcept
{
  if(checkvar == nullptr)
    strvar = "";
  else
    strvar = checkvar;
}
/**
 * @brief Get an environment variable, which is either set to an empty string or filled with the content of an existent one
 * @warning Of course, because this is a environment function, the same warnings from the std::getenv apply here, until
 *          the env-variable is converted to a string. This is because something can modify the environment without us
 *          shielding it. Be aware that some putenv(), unsetenv() and so on could modify the environment while
 *          converting the char* to some std::string.
 * @param var Name(`String`) to get
 * @return The variables content(in case it's existent) or empty string.
 */
inline std::string get_env(std::string const &var) noexcept
{
  std::string ENV = "";
  empty_if_nullptr_env(std::getenv(var.c_str()), ENV);
  return ENV;
}

}
}
#endif // ENVTOOLS_H
