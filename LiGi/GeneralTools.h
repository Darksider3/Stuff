#ifndef GENERALTOOLS_H
#define GENERALTOOLS_H

#include <iostream>
#include <sstream> // istringstream
#include <string>
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

static bool is_absolute(std::string_view const& path)
{
    if (!path.starts_with("/"))
        return false;
    return true;
}
static std::string absolutise(std::string_view const&); // take current path, resolve ".." and ".", return
static bool is_canonical(std::string_view const&);
static std::string canonicalise(std::string_view const&); // do all of above(absolutise) and resolve links down to the root file
static bool is_relative(std::string_view const&);         // anything that doesn't start with a / is relative
static bool exists(std::string_view const&);              // auto guess type(dir, file) and check existence

static bool file_exists(std::string_view const&);
static bool dir_exists(std::string_view const&);

static bool is_file(std::string_view const&);
static bool is_dir(std::string_view const&);
static bool is_pipe(std::string_view const&);
static bool is_fifo(std::string_view const&);
static bool can_write(std::string_view const&);
static bool can_read(std::string_view const&);
static bool can_exec(std::string_view const&);
static bool can_del(std::string_view const&);
}
}
}

#endif // GENERALTOOLS_H
