#ifndef ENVTOOLS_H
#define ENVTOOLS_H
#include <cstdlib> // char *std::getenv
#include <mutex>
#include <string>
namespace Li {
namespace Env {

/**
 * @brief EnvStateLock locks whenever something get's requested from the current environment
 */
std::mutex EnvStateLock;

/**
 * @brief EnvInputStringLock Convenience lock for possible data races through the input strings(will be locked when read or written to)
 */
std::mutex EnvInputStringLock;

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
inline void empty_if_nullptr_env(const char* checkvar, std::string& strvar) noexcept
{
    if (checkvar == nullptr)
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
inline std::string get_env(std::string const& var) noexcept
{
    std::string ENV = "";
    char* t;
    // anonymous scope guarding environment state
    {
        std::scoped_lock<std::mutex, std::mutex> ReadLock(EnvStateLock, EnvInputStringLock);
        t = std::getenv(var.c_str());
    }

    empty_if_nullptr_env(t, ENV);
    return ENV;
}

/**
 * @brief set_env Thread-Safe set_env.
 * @param variable Variable to write into
 * @param contents Contents to write into `variable`
 * @param overwrite Weither or not to overwrite existing values in case the variable already exists
 * @return True, when the variable got set, `false` otherwise. Sets `errno` in case of `false`..
 *
 */
inline bool set_env(const std::string& variable, const std::string& contents, const bool overwrite = true)
{
    std::scoped_lock<std::mutex, std::mutex> WriteLock(EnvStateLock, EnvInputStringLock);
    return (setenv(variable.c_str(), contents.c_str(), static_cast<int>(overwrite)) == -1) ? false : true;
}

/**
 * @brief unset_env Thread-Safe environment-variable deletion
 * @param variable Variables name to delete(will lock `EnvInputStringLock`)
 * @return `true` if everything worked, otherwise `false` and `errno` will be set.
 */
inline bool unset_env(const std::string& variable)
{
    std::scoped_lock<std::mutex, std::mutex> StateLock(EnvStateLock, EnvInputStringLock);
    return (unsetenv(variable.c_str()) == -1) ? false : true;
}

std::string home_dir = get_env("HOME");

}
}
#endif // ENVTOOLS_H
