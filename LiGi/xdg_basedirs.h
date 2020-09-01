#ifndef XDG_BASEDIRS_H
#define XDG_BASEDIRS_H
#include <string>
#include <filesystem>
#include <vector>

#include "Singleton.h"

/*! A Singleton class managing & sanitizing the existence of XDG_$name$ variables, conform to the standard
 *
 *
 * Coded against:
 * https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
 * in current version at time of this writing, which is v 0.7 from 24th November 2010.
 * In desperate need to just code something which is different and small but finished fast!
 */
class xdg_basedirs : public Li::Singleton<xdg_basedirs>
{
private:
  std::filesystem::path M_tmpPath;
protected:
  char M_list_delim = ':';

  void empty_if_nullptr(const char *checkvar, std::string &strvar)
  {
    if(checkvar == nullptr)
      strvar = "";
    else
      strvar = checkvar;
  }

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

public:
  struct Dirs_List {};
  std::string home;
  std::string config_home;
  std::string data_home;
  std::string data_dirs;
  std::string config_dirs;
  std::string cache_home;

  xdg_basedirs(bool auto_populate = true) : Li::Singleton<xdg_basedirs>()
  {
    if(auto_populate)
      xdg_basedir_populate();
  }

  /*! The only function someone else should ever call on this class
   *
   *
   */
  void xdg_basedir_populate()
  {
    empty_if_nullptr(std::getenv("HOME"), home);
    empty_if_nullptr(std::getenv("XDG_DATA_HOME"), data_home);
    empty_if_nullptr(std::getenv("XDG_CONFIG_HOME"), config_home);
    empty_if_nullptr(std::getenv("XDG_DATA_DIRS"), data_dirs);
    empty_if_nullptr(std::getenv("XDG_CONFIG_DIRS"), config_dirs);
    empty_if_nullptr(std::getenv("XDG_CACHE_HOME"), cache_home);
    return;
  }

  /*! Validate basic environment variable functin(e.g. no null pointer)
   *
   */
  bool valid_env(std::string &e)
  {

    namespace fs = std::filesystem;
    if(e.empty())
      return false;

    M_tmpPath = fs::path(e);

    if(!M_tmpPath.is_absolute())
    {
      return false;
    }

    return true;

  }

  std::string default_to(std::string &source, std::string to)
  {
    if(source.empty() && !valid_env(source))
      return to;
    return source;
  }

  std::string DataHome()
  {
    return default_to(data_home, home + "/.local/share");
  }

  std::string ConfigHOme()
  {
    return default_to(config_home, home + "/.config");
  }

  std::string CacheHome()
  {
    return default_to(cache_home, home + "/.cache");
  }

  std::string DataDirs()
  {
    return default_to(data_dirs, "/usr/local/share/:/usr/share/");
  }

  std::string ConfigDirs()
  {
    return default_to(config_dirs, "/etc/xdg");
  }

  std::vector<std::string> DataDirs(Dirs_List)
  {
    std::vector<std::string> potRet = split(data_dirs, M_list_delim);

    if(potRet.empty())
      potRet = split(DataDirs(), M_list_delim);

    return potRet;
  }

  std::vector<std::string> ConfigDirs(Dirs_List)
  {
    std::vector<std::string> potRet = split(config_dirs, M_list_delim);
    if(potRet.empty())
      potRet.emplace_back(ConfigDirs());

    return potRet;
  }
};

#endif
