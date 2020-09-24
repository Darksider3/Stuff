#ifndef GENERALTOOLS_H
#define GENERALTOOLS_H

#include <cstring>
#include <dirent.h> // stat
#include <iostream>
#include <sstream> // istringstream
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace Li {
namespace GeneralTools {

std::vector<std::string> split(const std::string& s, const char delimiter) noexcept
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
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

namespace fs {
#ifndef __linux__
static_assert(false, "Currently, just supporting linux here(pathes are not validated in that manner)");
#endif
#include <unistd.h>
bool is_file(std::string const& path)
{
    struct stat st;
    if (stat(path.c_str(), &st) == 0 && st.st_mode & S_IFMT) // ignore wether being a regular or any other type of file
    {
        return true;
    }

    return false;
}
bool is_dir(std::string const& path)
{
    struct stat st;
    if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
        return true;
    }

    return false;
}
bool is_pipe(std::string_view const&);
bool is_fifo(std::string_view const&);

bool is_absolute(std::string_view const& path)
{
    if (!path.starts_with("/"))
        return false;
    return true;
}
std::string absolutise(std::string_view const&); // take current path, resolve "..", "~" and ".", return
bool is_canonical(std::string_view const&);
std::string canonicalise(std::string_view const&); // do all of above(absolutise) and resolve links down to the root file
bool is_relative(std::string_view const& path)     // anything that doesn't start with a / is relative
{
    return !is_absolute(path);
}

bool file_exists(std::string const& path)
{
    return is_file(path);
}

bool dir_exists(std::string const& path)
{
    return is_dir(path);
}

bool exists(std::string const& path) // auto guess type(dir, file) and check existence
{
    if (is_dir(path)) {
        return true;
    } else {
        return file_exists(path);
    }
}

bool can_write(std::string_view const&)
{
}
bool can_read(std::string_view const&);
bool can_exec(std::string_view const&);
bool can_del(std::string_view const&);
std::pair<std::pair<bool, std::string_view>, FILE*> error_or_fp(std::string const& path)
{
    FILE* fp = fopen64(path.c_str(), "rw");
    std::pair<bool, std::string_view> first_part;
    std::pair<std::pair<bool, std::string_view>, FILE*> result;
    if (fp == nullptr) {
        first_part.first = false;
        first_part.second = strerror(errno);
    } else {
        first_part.first = true;
        first_part.second = "";
    }

    result.first = first_part;
    result.second = fp;

    return result;
}
}
}
}
#endif // GENERALTOOLS_H
