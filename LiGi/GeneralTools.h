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

}
}
#endif // GENERALTOOLS_H
