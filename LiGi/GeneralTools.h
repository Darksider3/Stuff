#ifndef GENERALTOOLS_H
#define GENERALTOOLS_H

#include <cstring>
#include <dirent.h> // stat
#include <iostream>
#include <list>
#include <sstream> // istringstream
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace Li::common {

template<typename Findable, typename U>
bool has(const Findable m, const U&& thing)
{
    return m.find(thing) != m.end();
}

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

template<typename StrT>
std::pair<StrT, StrT> SplitPair(StrT str, char delim)
{
    auto split_pos = std::find(str.begin(), str.end(), delim);
    auto second_begin = split_pos != str.end() ? std::next(split_pos) : split_pos;
    return { { str.begin(), split_pos }, { second_begin, str.end() } };
}

template<typename Characters = std::string, typename Stack = std::list<Characters>>
Stack splitPreserveDelimiter(std::string const& source, const char delimitier, const char escape = '\\')
{
    std::list<std::string> ret;
    std::string token;
    bool escaped = false;
    for (auto& c : source) {
        token.push_back(c);
        if (escaped) {
            escaped = false; // escape flag clear
            continue;
        }

        else if (c == escape) {
            escaped = true;
        }

        if (c == delimitier) {
            ret.push_back(token);
            token.clear();
        }
    }
    if (!token.empty())
        ret.push_back(token);

    return ret;
}

template<typename Characters = std::string, typename Delimiter = char>
Characters strip(const Characters& Input, const Delimiter& CharToStrip)
{
    Characters result;
    for (auto& b : Input) {
        if (b == CharToStrip)
            continue;
        result += b;
    }

    return result;
}

inline bool isSpace(const char& c)
{
    return std::isspace(c);
}
/* @TODO: Generalized Parser for easy of use on the other parsers im going to write... ._,
 */
/* @TODO: URN, URI, URL-Parser
 * URI: https://tools.ietf.org/html/rfc3986
 * URL: Edit-Distance maybe helpfull by misspelling?
 * URN: Scheme://authority:AuthorityPort/PathOnAuthority?query=Value#Fragment
 */
}
#endif // GENERALTOOLS_H
