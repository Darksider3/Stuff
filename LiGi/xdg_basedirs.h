#ifndef XDG_BASEDIRS_H
#define XDG_BASEDIRS_H
#include <string>
#include <filesystem>
#include <vector>

#include "EnvTools.h"
#include "Singleton.h"
#include "GeneralTools.h"

/**
 * A Singleton class managing & sanitizing the existence of XDG_$name$ variables, conform to the standard
 *
 * Coded against:
 * https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
 * in current version at time of this writing, which is v 0.7 from 24th November 2010.
 * In desperate need to just code something which is different and small but finished fast!
 */
class xdg_basedirs : public Li::Singleton<xdg_basedirs>
{
private:
  /**
   * @brief default_to helper function to default to a supplied string in case it's empty
   * @param source String to check if it's empty or not
   * @param to Replacement value in case `source` is empty
   * @return Result of the comparison and assignment of the described behaivour
   */
  std::string default_to(std::string &source, std::string to)
  {
    if(source.empty() || !valid_xdg_var(source))
    {
      return to;
    }
    return source;
  }

protected:
  char M_list_delim = ':';

public:
  /**
   * @brief Used as a function specialisation-flag for directory lists processing
   *
   * To use said function, an example would smthing like:
   * `xdg_basedirs::DataDirs(Dirs_List());`
   * which will return a vector of already separated and (not yet verified absolute) directories
   */
  struct Dirs_List {};
  std::string M_home;
  std::string M_config_home;
  std::string M_data_home;
  std::string M_data_dirs;
  std::string M_config_dirs;
  std::string M_cache_home;

  xdg_basedirs() : Li::Singleton<xdg_basedirs>()
  {
    xdg_basedir_populate();
  }

  /**
   * @brief The only function someone else should ever call on this class
   *
   *
   */
  void xdg_basedir_populate() noexcept
  {
    M_home = Li::Env::home_dir;
    M_data_home = Li::Env::get_env("XDG_DATA_HOME");
    M_config_home = Li::Env::get_env("XDG_CONFIG_HOME");
    M_data_dirs = Li::Env::get_env("XDG_DATA_DIRS");
    M_config_dirs = Li::Env::get_env("XDG_CONFIG_DIRS");
    M_cache_home = Li::Env::get_env("XDG_CACHE_HOME");
    return;
  }

  /**
   * @brief Validate basic given environment variable functin(e.g. no null pointer) to spec
   *
   * Specification requires the given pathes to be absolute.
   *
   * @note Not sure if using is_absolute() is actually better then
   * anything else, but it is at least platform independent already
   *
   * @param &e String to check weither it's valid or not
   *
   * @return `True` in case it's valid, otherwise `False`
   */
  bool valid_xdg_var(std::string &e) const noexcept
  {
    if(e.empty())
      return false;

    return is_absolute(e);
  }

  /**
   * @brief is_absolute determines(as a wrapper around path::is_absolute) if a given path is absolute
   *
   * @param str Path to check
   * @returns bool True in case it's absolute, false if not
   */
  bool is_absolute(std::string const &str) const noexcept
  {
    std::filesystem::path tmpPath(str);
    tmpPath = std::filesystem::path(str);
    return tmpPath.is_absolute();
  }

  /**
   * @brief Get the Home of the current user
   * @return HOME-Directory of the current user
   */
  inline std::string Home() const noexcept
  {
    return M_home;
  }

  /**
   * @brief DataHome returns an already(to the spec) sanitized path to $XDG_DATA_HOME
   *
   * @returns A std::string Path to XDG_DATA_HOME
   */
  std::string DataHome()
  {
    return default_to(M_data_home, M_home + "/.local/share");
  }

  /**
   * @brief ConfigHome get(according to spec) sanitized path to $XDG_CONFIG_HOME
   *
   * @returns A std::string Path to XDG_CONFIG_HOME
   */
  std::string ConfigHome()
  {
    return default_to(M_config_home, M_home + "/.config");
  }

  /**
   * @brief CacheHome get(according to spec) sanitized path to $XDG_CACHE_HOME
   *
   * @returns A `std::string` Path to XDG_CACHE_HOME
   */
  std::string CacheHome()
  {
    return default_to(M_cache_home, M_home + "/.cache");
  }

  std::string DataDirs()
  {
    return default_to(M_data_dirs, "/usr/local/share/:/usr/share/");
  }

  std::string ConfigDirs()
  {
    return default_to(M_config_dirs, "/etc/xdg");
  }

  /**
   * @brief DataDirs Get a list of data directories
   * @param Dirs_list Flag
   * @return A `std::vector<std::string>` consisting of either -
   *        by the environment `$XDG_DATA_DIRS` - given
   *        pathes or the default pathes of the standard in case there are none.
   */
  std::vector<std::string> DataDirs(Dirs_List)
  {
    std::vector<std::string> potRet = Li::common::split(M_data_dirs, M_list_delim);

    if(potRet.empty())
      potRet = Li::common::split(DataDirs(), M_list_delim);

    return potRet;
  }

  /**
   * @brief ConfigDirs Get a list of data directories
   * @param Dirs_List Flag
   * @return A `std::vector<std::string>` consisting of either - by
   *        the environment `$XDG_CONFIG_DIRS` - given
   *        pathes or the default pathes of the standard in case there are none.
   */
  std::vector<std::string> ConfigDirs(Dirs_List)
  {
    std::vector<std::string> potRet = Li::common::split(M_config_dirs, M_list_delim);
    if(potRet.empty())
      potRet.emplace_back(ConfigDirs());

    return potRet;
  }
};

#endif
