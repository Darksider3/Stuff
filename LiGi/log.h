#ifndef LOG_H
#define LOG_H
#include "Singleton.h"
#include "creational.h"
#include <chrono>
#include <fstream>
#include <iostream>

#ifndef NDEBUG

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
    FileLogProvider(std::string const& path, std::ios_base::openmode mode = std::ios_base::out)
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

    Dbg(std::ostream& opt)
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
#else
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
#endif
static DbgSingle& DBG = *DbgSingle::instance();
#endif // LOG_H
