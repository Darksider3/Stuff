#ifndef XDG_BASEDIRS_H
#define XDG_BASEDIRS_H
#include <string>

#include "../Singleton.h"

/*! A Singleton class managing & sanitizing the existence of XDG_$name$ variables, conform to the standard
 *
 *
 * Coded against:
 * https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
 * in current version at time of this writing, which is v 0.7 from 24th November 2010.
 * In desperate need to just code something which is different and small but finished fast!
 */
class xdg_basedirs : Li::Singleton<xdg_basedirs>
{
public:
  std::string home;
  std::string config_home;
  std::string data_home;
  std::string data_dirs;
  std::string config_dirs;
  std::string cache_home;
  std::string runtime_dir;

  /*! The only function someone else should ever call on this class
   *
   *
   */
  void xdg_basedir_populate();

  /*! Validate basic environment variable functin(e.g. no null pointer)
   *
   *
   */
  void valid_env(std::string &e);

  /*! Sanitizes given environment variables
   *
   * Here we're going to support the spec about the sentence
   * about absolute paths. In case we find something
   */
  bool sanitize();
};

#endif
