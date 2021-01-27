//
// Created by darksider3 on 22.01.21.
//

#ifndef POCO_FILE_HASHER_STRINGFORMAT_HPP
#define POCO_FILE_HASHER_STRINGFORMAT_HPP

#include "common.hpp"
#include <Poco/DigestEngine.h>
#include <Poco/File.h>
#include <Poco/Format.h>
#include <string>

#include <iostream>

namespace Formatting {

/**
 * @brief CRTP Base-Class for formatting
 *
 * Deletes **all** other constructors, leaving just it's one usable, which is mandatory to use.
 *
 * @tparam CRTP Obviously the class you want to augment
 */
template<typename CRTP>
class AbstractOutputFormatter {
private:
    [[maybe_unused]] std::vector<unsigned char>& m_digest;
    [[maybe_unused]] Poco::File& m_File;
    [[maybe_unused]] std::string& m_Method;
    [[maybe_unused]] std::string& m_FormatStr;

    AbstractOutputFormatter<CRTP>* u() { return &static_cast<CRTP&>(*this); }
    [[nodiscard]] const AbstractOutputFormatter<CRTP>* u_c() const { return static_cast<const CRTP*>(this); }

protected:
    using digestVec = std::vector<unsigned char>;
    using File = Poco::File;
    using string = std::string;

    [[maybe_unused]] digestVec& getDigest()
    {
        return u()->m_digest;
    }

    [[maybe_unused]] const File& getFile()
    {
        return u()->m_File;
    }

    [[maybe_unused]] const string& getMethod()
    {
        return u()->m_Method;
    }

    [[maybe_unused]] string& getFormatStr()
    {
        return u()->m_FormatStr;
    }

    [[maybe_unused]] [[nodiscard]] digestVec& getDigest() const
    {
        return u()->m_digest;
    }

    [[maybe_unused]] [[nodiscard]] File& getFile() const
    {
        return u()->m_File;
    }

    [[maybe_unused]] [[nodiscard]] string& getMethod() const
    {
        return u()->m_Methodt;
    }

    [[maybe_unused]] [[nodiscard]] string& getFormatStr() const
    {
        return u_c()->m_FormatStr;
    }

    [[maybe_unused]] void setDigest(digestVec& digest) { u()->m_digest = digest; }
    [[maybe_unused]] void setFile(File& file) { u()->m_File = file; }
    [[maybe_unused]] void setMethod(string& str) { u()->m_Method = str; }
    [[maybe_unused]] void setFormatStr(string& str) { u()->m_FormatStr = str; }

public:
    explicit AbstractOutputFormatter(digestVec& digest, File& F, string& Method_Name, string& Format_Str)
        : m_digest(digest)
        , m_File(F)
        , m_Method(Method_Name)
        , m_FormatStr(Format_Str)
    {
    }

    virtual void reinit(digestVec& digest, File& F, string& Method_Name, string& str)
    {
        u()->setDigest(digest);
        u()->setFile(F);
        u()->setMethod(Method_Name);
        u()->setFormatStr(str);
    }

    virtual std::string FormatHash() = 0;

    [[maybe_unused]] virtual string Work() { return FormatHash(); }

    virtual ~AbstractOutputFormatter() = default;

    // This class shall never be: Moved, Copied or somehow differently be assigned aside from initialising it on an class.
    AbstractOutputFormatter() = delete;
    AbstractOutputFormatter(AbstractOutputFormatter&&) = delete;
    AbstractOutputFormatter(const AbstractOutputFormatter&) = delete;
    AbstractOutputFormatter(AbstractOutputFormatter&) = delete;
    AbstractOutputFormatter& operator=(const AbstractOutputFormatter&) = delete;
    AbstractOutputFormatter& operator=(const AbstractOutputFormatter&&) = delete;
};

class [[maybe_unused]] CSVFormat : public AbstractOutputFormatter<CSVFormat> {
public:
    CSVFormat(digestVec& digest, File& F, string& Method_Name, string& Format_Str)
        : AbstractOutputFormatter<CSVFormat>(digest, F, Method_Name, Format_Str)
    {
    }

    string FormatHash() override
    {
        return getFormatStr();
    }

    [[maybe_unused]] [[nodiscard]] bool HasCSVHeader() const
    {
        auto pos = getFormatStr().find(CSVHeader + LN);
        return pos != getFormatStr().npos;
    }

    [[maybe_unused]] void InsertCSVHeader()
    {
        getFormatStr().insert(0, CSVHeader + LN);
    }
};

class PrintFormat : public AbstractOutputFormatter<PrintFormat> {
private:
    bool m_AddHashToPrint { false };

public:
    PrintFormat(digestVec& digest, File& F, string& Method_Name, string& Format_Str)
        : AbstractOutputFormatter<PrintFormat>(digest, F, Method_Name, Format_Str)
    {
    }

    [[nodiscard]] string FormatHash() override
    {
        string& str = getFormatStr();
        digestVec digest = getDigest();
        File F = getFile();
        const string& Method_Name = getMethod();
        assert(!digest.empty() && "We cant format something that's empty!");
        assert(F.exists() && "File must exist to hash it!");
        assert(!Method_Name.empty() && "Method name should be an optional parameter and thus needs sadly content...");

        string return_str(digest.size(), '\0');
        return_str.append(Poco::format("%s  %s", Poco::DigestEngine::digestToHex(digest), F.path()));

        if (m_AddHashToPrint) {
            return_str.append(Poco::format("; %s.", Method_Name));
        }

        assert(!return_str.empty() && "This should actually hold a string! Cant be empty!");

        return_str += LN;

        return return_str;
    }

    void setAddHashToPrint(bool op) { m_AddHashToPrint = op; }
};

/*
 * @TODO: This whole idea. Print As CSV!
 */
bool HasCSVHeader(std::string& str)
{
    auto pos = str.find(CSVHeader + LN);
    return pos != str.npos;
}

std::string InsertCSVHeader(std::string& str)
{
    str.reserve(str.size() + sizeof(CSVHeaderLength));
    str.insert(0, CSVHeader + LN);

    return str;
}

[[maybe_unused]] std::string FormatHashAsCSV(const std::vector<unsigned char>& digest, const Poco::File& F, const std::string& Method_Name, std::string& Format_Str)
{
    assert(!digest.empty());
    assert(F.exists());
    assert(!Method_Name.empty());

    // Step 1: Either insert Header or accept it's existence.
    if (!HasCSVHeader(Format_Str))
        InsertCSVHeader(Format_Str);

    // Step 2: WRITE IT ALL IN YOU LITTLE PIECE OF RAINBOW-Y AWESOMENESS
    Format_Str.append(Poco::format("%s,%s,%s%s", F.path(), Method_Name, Poco::DigestEngine::digestToHex(digest), LN));

    // RETURN IT!!
    return Format_Str;
}

/**
 * @brief Formatting function for hashes generated by our little main logic function, which mimics the sha\*sum programs
 *
 * @param  const std::vector<unsigned char>& digest          Digest generated by the engine(`Poco::DigestEngine B.digest()`) to format
 * @param  const Poco::File&                 F               Fileobject used for hashing(here just used to get the path)
 * @param  const bool                        AddMethod       Weither we append the method's name alongside the Files Path and Name
 * @param  const std::string_view            Method_Name     Method names then used beforehand
 *
 * @return std::string Formatted Hash that mimics the  behaivour of sha1sum, sha256sum, md5sum etc. (HexDigest, followed by 2 spaces, followed by Path and newline)
 */
std::string FormatHashToPrint(std::vector<unsigned char>& digest, Poco::File& F, const bool AddMethod = false, const std::string& Method_Name = "null")
{

    std::string return_str;
    std::string method = Method_Name;
    PrintFormat Formatter(digest, F, method, return_str);
    Formatter.setAddHashToPrint(AddMethod);

    return Formatter.FormatHash();
}

/**
 * @brief Compares Hash1 with Hash1, currently without considerating the filename
 *
 * @param const std::string& HashStr1 First Hash
 * @param const std::string& HashStr2 Second hash to compare against
 *
 * @return bool If equal, `true`. Otherwhise `false`.
 */
bool CompareHash(const std::string& HashStr1, const std::string& HashStr2)
{
    // bc458c17dc8eefd118d81133c2be26a2  ./CMakeCache.txt
    // bc458c17dc8eefd118d81133c2be26a2  ./CMakeCache.txt; MD5.

    assert(!HashStr1.empty() && "Should not be empty!");
    assert(!HashStr2.empty() && "Should not be empty!");

    std::string delimiter = "  ";
    std::string hash1 = HashStr1.substr(0, HashStr1.find(delimiter));
    std::string hash2 = HashStr2.substr(0, HashStr2.find(delimiter));

    assert(!hash1.empty() && "If correctly formatted, this shouldnt ever be empty!");
    assert(!hash2.empty() && "If correctly formatted, this shouldnt ever be empty!");

    return hash1 == hash2;
}

}
#endif //POCO_FILE_HASHER_STRINGFORMAT_HPP
