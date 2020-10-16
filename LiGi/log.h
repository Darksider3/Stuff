#ifndef LOG_H
#define LOG_H
#include "Singleton.h"
#include <chrono>
#include <iostream>
#include <locale>

#ifndef NDEBUG

class GeneralDebug {
private:
    std::ostream& m_out;

public:
    explicit GeneralDebug(std::ostream& o)
        : m_out(o)
    {
    }

    virtual std::ostream& print(std::string_view out)
    {
        m_out << out;
        return m_out;
    }

    virtual std::ostream& operator<<(std::string_view output)
    {
        return print(output);
    }

    virtual ~GeneralDebug() = default;
};

class Dbg : public GeneralDebug {
public:
    Dbg()
        : GeneralDebug(std::cerr)
    {
    }

    Dbg(std::ostream& opt)
        : GeneralDebug(opt)
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
