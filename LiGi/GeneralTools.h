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

std::vector<std::string> split(const std::string& s, const char delimiter) noexcept
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


/* @TODO: Generalized Parser for easy of use on the other parsers im going to write... ._,
 */
/* @TODO: URN, URI, URL-Parser
 * URI: https://tools.ietf.org/html/rfc3986
 * URL: Edit-Distance maybe helpfull by misspelling?
 * URN: Scheme://authority:AuthorityPort/PathOnAuthority?query=Value#Fragment
 */

/* @TODO: Filesystem: dynamic is_absolute(), is_relative() just for excercise
 * POSIX defines absolute anything with / in front as absolute, anything else is pretty much relative
 * ... but URL/URIs are still allowed! Support? Scheme:/// <- Absolute Scheme://bla/ <- Absolute
 */
/*
 * @TODO: Filesystem class!
 *  -> 1. Li::fs::Path first! -> {get|is}_absolute, {get|is}_relative, "auto guess path type", is_{dir|file|pipe|...}, can_{read, write,exec, del}
 *  -> 2. Li::fs::GeneralFile -> Abstraction with common parts across all file-types
 *  -> 3. Li::fs::FIFO        -> FIFO-Handle&Help!
 *  -> 4. Li::fs::Pipe        -> .^^
 *  -> 5. Followed by.. TBA
 */

class Path
{
protected:
  std::string M_Path;

public:

  explicit Path(std::string const &str) : M_Path(str.c_str())
  {
  }

  explicit Path(std::string &&str) : M_Path(std::move(str))
  {
  }

  explicit Path(const Path &&other) : M_Path(std::move(other.M_Path))
  {
  }

  explicit Path(const Path &other) : M_Path(other.M_Path)
  {}
};
}
}

#endif // GENERALTOOLS_H
