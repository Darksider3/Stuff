#ifndef FILETOOLS_H
#define FILETOOLS_H
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "GeneralTools.h"

namespace Li {
namespace GeneralTools {
namespace fs {

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

#ifndef __linux__
static_assert(false, "Currently, just supporting linux here(pathes are not validated in that manner)");
#endif

bool exists(std::string const& path) // auto guess type(dir, file) and check existence
{
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}
bool is_file(std::string const& path)
{
    struct stat st;
    if (exists(path) && st.st_mode & S_IFMT) // ignore wether being a regular or any other type of file
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

bool is_link(std::string const& path)
{
    struct stat st;
    if (stat(path.c_str(), &st) != 0)
        return false;
    return S_ISLNK(st.st_mode) != 0;
}

bool is_pipe(std::string_view const&);
bool is_fifo(std::string_view const&);

bool is_absolute(std::string_view const& path)
{
    if (!path.starts_with("/"))
        return false;
    return true;
}
bool is_canonical(std::string const& path)
{
    if (!fs::is_absolute(path)) { // canonical pathes must be absolute
        return false;
    }

    if (fs::is_link(path)) { // canonical links are no symlinks!
        return false;
    }

    return true;
}

std::string absolutise(std::string_view const& path) // take current path, resolve "..", "~" and ".", return
{
    std::string ResultingString, tmp;
    char buff[PATH_MAX];
    if (fs::is_absolute(path)) {
        // already absolute
        ResultingString = path;
    } else if (path.starts_with("../")) {
        // parent directory reference
    } else if (path.starts_with("./")) {
        // current directory reference
        if (getcwd(buff, sizeof(buff)) != nullptr) {
            // buff has cwd
            tmp = std::string(path).erase(0, 1);
            ResultingString = buff;
            ResultingString.append(tmp);
        }
    }

    return ResultingString;
}

void canonicalise(std::string_view const& path);      // do all of above(absolutise) and resolve links down to the root file
inline bool is_relative(std::string_view const& path) // anything that doesn't start with a / is relative
{
    return !is_absolute(path);
}

inline bool file_exists(std::string const& path)
{
    return is_file(path);
}

inline bool dir_exists(std::string const& path)
{
    return is_dir(path);
}

bool can_write(std::string_view const&);
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

class FSObj {
private:
    std::string m_Name;

public:
    explicit FSObj(std::string_view const& t)
        : m_Name { t }
    {
    }

    void debugOut()
    {
        std::cout << m_Name;
    }

    static std::unique_ptr<FSObj> create(std::string_view const& t)
    {
        return std::make_unique<FSObj>(t);
    }
};

std::list<std::string> TokenizePath(std::string const& Path, bool preserveSlashes = true)
{
    std::list<std::string> L;

    std::string token;
    std::string SafePath = Path;
    if (Path[0] == '/') {
        SafePath = SafePath.erase(0, 1);
    }

    std::istringstream Stream(SafePath);

    while (std::getline(Stream, token, '/')) {
        if (preserveSlashes)
            token = '/' + token;

        L.push_back(token);
    }

    return L;
}

class Path {
private:
    struct Properties {
        bool absolute = false;
    } m_Properties;

    std::list<std::unique_ptr<FSObj>> m_Path;
    std::string m_Path_String;

public:
    explicit Path(std::string const& Path)
        : m_Path_String { Path }
    {
        for (auto& item : Li::GeneralTools::splitPreserveDelimiter(Path, '/')) {
            m_Path.push_back(FSObj::create(item));
        }
        if (is_absolute(Path)) {
            m_Properties.absolute = true;
        }
    }

    bool exists() const
    {
        return (fs::exists(m_Path_String));
    }

    std::string ToString() const
    {
        return m_Path_String;
    }

    operator std::string() const
    {
        return m_Path_String;
    }

    operator const std::string()
    {
        return m_Path_String;
    }

    void debugOut() const
    {
        for (auto& b : m_Path) {
            b->debugOut();
        }
        std::cout << std::endl;
    }
};
}
}
}
#endif // FILETOOLS_H
