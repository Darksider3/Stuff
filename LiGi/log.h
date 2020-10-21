#ifndef LOG_H
#    define LOG_H
#    include "Singleton.h"
#    include "creational.h"
#    include <cassert>
#    include <chrono>
#    include <fstream>
#    include <iostream>

#    if !defined(NDEBUG) || !defined(DBG_LOG_OUTPUT)

// general

class LogProviders {
public:
    virtual std::ostream& print(std::string_view __attribute__((unused)) s) = 0;
    virtual std::ostream& operator<<(std::string_view __attribute__((unused)) s) = 0;
    virtual ~LogProviders() = default;
};

class GeneralOutputProvider : LogProviders {
private:
    std::ostream& m_out;

public:
    explicit GeneralOutputProvider(std::ostream& o)
        : m_out(o)
    {
    }

    std::ostream& print(std::string_view out) override
    {
        m_out << out;
        return m_out;
    }

    std::ostream& operator<<(std::string_view output) override
    {
        return print(output);
    }
    virtual ~GeneralOutputProvider() = default;
};

class FileLogProvider : public LogProviders
    , public Li::creational::construct_unique<FileLogProvider> {
private:
    std::ofstream f;

public:
    explicit FileLogProvider(std::string const& path, std::ios_base::openmode mode = std::ios_base::out)
        : f(path, mode)
    {
    }

    std::ostream& print(std::string_view str) override
    {
        *this << str;
        return f;
    }

    std::ostream& operator<<(std::string_view str) override
    {
        f << std::string(str);
        f.flush();
        return f;
    }

    ~FileLogProvider()
    {
        f.flush();
        f.close();
    }
};

class Dbg : public GeneralOutputProvider {
public:
    Dbg()
        : GeneralOutputProvider(std::cerr)
    {
    }

    explicit Dbg(std::ostream& opt)
        : GeneralOutputProvider(opt)
    {
    }

    std::ostream& operator<<(std::string_view str) override
    {
        std::string output;
        output += "\033[1;33mDBG: ";
        output += "\033[0m";
        output += str;
        output += "\n";

        return print(output);
    }
};

class DbgSingle : public Dbg
    , public Li::Singleton<DbgSingle> {
};

#        define DBG *DbgSingle::instance()
#        ifdef LINEDBG
#            undef DBG
class SourceFileDBG : public Dbg
    , public Li::Singleton<DbgSingle> {
private:
public:
    std::ostream& print(std::string_view str, size_t line, const char* file)
    {
        Dbg::print(file);
        Dbg::print(":");
        Dbg::print(std::to_string(line));
        Dbg::print(": ");

        return Dbg::operator<<(str);
    }
};
#            define DBG *SourceFileDBG::instance()
#        endif
#    else
class DbgSingle : public Li::Singleton<DbgSingle> {
private:
public:
    explicit DbgSingle()
    {
    }

    explicit DbgSingle(std::ostream& opt __attribute__((unused))) { }

    void operator<<(std::string_view __attribute__((unused)) str)
    {
    }
};
#    endif
#endif // LOG_H

/*
 * @TODO A class with the interface ProviderMetaClass *logoutput = CapabilitieChooser(cap::LINE, cap::FILE, cap::STRING);
 * logoutput << "bla" --> Results in "LINE:FILE: bla"
 * Executes the Arguments in order it got it, functions bound in a vector. std::vector<std::function<void()>(func)
 * The list gets parsed in the constructor once
 */
