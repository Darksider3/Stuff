#ifndef FILETOOLS_H
#define FILETOOLS_H
#include <cstring>
#include <dirent.h>
#include <iostream>

#include <list>
#include <numeric>

#include <memory>
#include <sstream>
#include <string>

#include <sys/stat.h>
#include <unistd.h>

#include "GeneralTools.h"
#include "creational.h"
namespace Li {
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

class FSObj : public Li::constructTPL::construct_unique<FSObj> {
public:
    std::string m_Name;

    explicit FSObj(std::string_view const& t)
        : m_Name { t }
    {
    }

    void debugOut()
    {
        std::cout << m_Name;
    }
};

class Path {
private:
    struct Properties {
        bool absolute = false;
        bool exists = false;
    } m_Properties;

    std::vector<std::unique_ptr<FSObj>> m_Path;
    std::string m_Path_String;
    bool m_dirty_Path = false;

public:
    explicit Path(std::string const& Path)
        : m_Path_String { Path }
    {
        auto list = Li::GeneralTools::splitPreserveDelimiter(Path, '/');

        auto insertSplitsIntoList = [=](std::string const& item) {
            return FSObj::create(item);
        };

        std::transform(list.begin(), list.end(), std::back_inserter(m_Path), insertSplitsIntoList);
        if (is_absolute(Path)) {
            m_Properties.absolute = true;
        }
        __exists();
    }

    //refactor exists
    void __exists()
    {
        m_Properties.exists = fs::exists(m_Path_String);
    }

    bool exists()
    {
        return m_Properties.exists;
    }

    void __createStrFromPath()
    {
        m_Path_String.clear();
        for (auto& str : m_Path) {
            m_Path_String += str->m_Name;
        }
    }

    void __dirtyCleanup()
    {
        if (m_dirty_Path) {
            __createStrFromPath();
            m_Properties.absolute = is_absolute(m_Path_String);
            __exists();
            m_dirty_Path = false;
        }

        return;
    }

    void append(std::string const& path)
    {
        __dirtyCleanup();
        auto ext_List = Li::GeneralTools::splitPreserveDelimiter(path, '/');

        for (auto& item : ext_List) {
            m_Path.insert(m_Path.end(), FSObj::create(item));
        }

        m_dirty_Path = true;
        __dirtyCleanup();
    }

    std::string get()
    {
        __dirtyCleanup();
        return m_Path_String;
    }

    std::string Parent()
    {
        __dirtyCleanup();
        if (m_Path.size() < 2) {
            return m_Path_String;
        }
        return m_Path[m_Path.size() - 2]->m_Name;
    }

    std::string ToString()
    {
        return get();
    }

    operator std::string()
    {
        return get();
    }

    friend std::ostream& operator<<(std::ostream& ostr, Path& p)
    {
        ostr << p.get();
        return ostr;
    }

    void debugOut() const
    {
        for (auto& b : m_Path) {
            b->debugOut();
        }
        std::cout << std::endl;
    }

    size_t stacksize()
    {
        return m_Path.size();
    }

    void clear()
    {
        m_Path.clear();
        m_Path_String.clear();
        m_dirty_Path = true;
    }
};
}
}
#endif // FILETOOLS_H
