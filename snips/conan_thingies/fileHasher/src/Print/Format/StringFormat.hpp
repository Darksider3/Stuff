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
    [[maybe_unused]] const std::vector<unsigned char>* m_digest {};
    [[maybe_unused]] Poco::File* m_File {};
    [[maybe_unused]] std::string* m_Method {};
    [[maybe_unused]] std::string* m_FormatStr {};

    AbstractOutputFormatter<CRTP>* u() { return &static_cast<CRTP&>(*this); }
    [[nodiscard]] const AbstractOutputFormatter<CRTP>* u_c() const { return static_cast<const CRTP*>(this); }

    std::unordered_map<std::string, bool> BoolFlags {};

protected:
    /**
     * @brief Map of settings
     */
    using OptionsMap = std::unordered_map<std::string, bool>;

    /**
     * @brief Contains digest produced by Poco::Crypto::DigestEngine.digest() - const-correctness included!
     */
    using digestVec = const std::vector<unsigned char>;

    /**
     * @brief File currently to operate on
     */
    using File = Poco::File;

    /**
     * @brief strings used in this class
     */
    using string = std::string;

    /**
     * @return digestVec& Digest-Engines produced Digest
     */
    [[maybe_unused]] const digestVec& getDigest()
    {
        return *u()->m_digest;
    }

    /**
     * @return  const File& File currently operating on
     */
    [[maybe_unused]] const File& getFile()
    {
        return *u()->m_File;
    }

    /**
     * @return const string& Hashing method used on File.
     */
    [[maybe_unused]] const string& getMethod()
    {
        return *u()->m_Method;
    }

    /**
     * @return string& Formatted string
     */
    [[maybe_unused]] string& getFormatStr()
    {
        return *u()->m_FormatStr;
    }

    /**
     * @return OptionsMap& Map conaining formatting-relevant settings in boolean format
     */
    [[maybe_unused]] OptionsMap& getOptionsMap()
    {
        return u()->BoolFlags;
    }

    /**
     * @brief see getDigest()
     */
    [[maybe_unused]] [[nodiscard]] digestVec& getDigest() const
    {
        return *u()->m_digest;
    }

    /**
     * @brief See getFile()
     */
    [[maybe_unused]] [[nodiscard]] File& getFile() const
    {
        return *u()->m_File;
    }

    /**
     * @brief See getMethod()
     */
    [[maybe_unused]] [[nodiscard]] string& getMethod() const
    {
        return *u()->m_Methodt;
    }

    /**
     * @brief See getFormatStr
     */
    [[maybe_unused]] [[nodiscard]] string& getFormatStr() const
    {
        return *u_c()->m_FormatStr;
    }

    /**
     * @brief See getOptionsMap()
     */
    [[maybe_unused]] [[nodiscard]] OptionsMap& getOptionsMap() const
    {
        return *u_c()->BoolFlags;
    }

    /**
     * @brief Set Digest-Member
     * @param digestVec& digest  Digest produced by Poco::Crypto::DigestEngine->digest();
     */
    [[maybe_unused]] void setDigest(digestVec& digest) { u()->m_digest = &digest; }

    /**
     * @brief Set File currently operating on
     * @param File& file  Current Targetfile
     */
    [[maybe_unused]] void setFile(File& file) { u()->m_File = &file; }

    /**
     * @brief Set Hash method used to produce the digest
     * @param string& str Hash methods name
     */
    [[maybe_unused]] void setMethod(string& str) { u()->m_Method = &str; }

    /**
     * @brief Set Formatting string to write into
     * @param string& str  String to operate on
     */
    [[maybe_unused]] void setFormatStr(string& str) { u()->m_FormatStr = &str; }

public:
    explicit AbstractOutputFormatter(digestVec& digest, File& F, string& Method_Name, string& Format_Str)
        : m_digest(&digest)
        , m_File(&F)
        , m_Method(&Method_Name)
        , m_FormatStr(&Format_Str)
    {
    }

    /**
     * @brief Reinitialise all relevant class members
     *
     * @param digestVec& digest Digest reinitialise
     * @param Files& F File to operate on
     * @param string& Method_Name Hashmethods name to produce digest
     * @param string& str String to modify with formatting
     */
    virtual void reinit(digestVec& digest, File& F, string& Method_Name, string& str)
    {
        u()->setDigest(digest);
        u()->setFile(F);
        u()->setMethod(Method_Name);
        u()->setFormatStr(str);
    }

    /**
     * @brief Reinitialise all relevant class members
     *
     * @param digestVec& digest Digest reinitialise
     * @param Files& F File to operate on
     * @param string& Method_Name Hashmethods name to produce digest
     * @param string& str String to modify with formatting
     */
    virtual void reinit(digestVec* digest, File* F, string* Method_Name, string* str)
    {
        u()->setDigest(*digest);
        u()->setFile(*F);
        u()->setMethod(*Method_Name);
        u()->setFormatStr(*str);
    }

    /**
     * @brief Set option in the settings vector
     *
     * @param const bool Opt Boolean value to set
     * @param const string& Name  Name of the setting to set
     */
    virtual void setOption(const bool Opt, const std::string& Name)
    {
        u()->getOptionsMap()[Name] = Opt;
    }

    /**
     * @brief Get an Option by name. Returns false if not set.
     *
     * @param string& name  Name of the setting to find in the options
     * @return bool If the value is found and true, returns `true`. Otherwise `false`.
     */
    virtual bool getOption(const std::string& name)
    {

        if (u()->getOptionsMap().find(name) == u()->getOptionsMap().end()) {
            return false;
        }
        return u()->getOptionsMap().at(name);
    }

    /**
     * @brief Tests all given parameters on sanenes. *CAUTION* Will try to validate **every pointer** given!
     *
     * @return `true`, in case every check passed, otherwise `false`.
     */
    virtual bool good()
    {
        return (u()->m_digest != nullptr)
            && (u()->m_File != nullptr)
            && (u()->m_Method != nullptr)
            && (u()->m_FormatStr != nullptr);
    }

    /**
     * @brief FormatHash is the main method any subclass should implement to use the behaivour of this class. It's the logic-function. If you've got anything to format, do it here
     *
     * @return string Formatted string
     */
    virtual std::string FormatHash() = 0;

    /**
     * @brief Standard interface to call FormatHash() for other classes because it's declared at any time.
     *
     * @return string Formatted string
     */
    [[maybe_unused]] virtual string Work() { return FormatHash(); }

    /**
     * @brief Deleted.
     */
    virtual ~AbstractOutputFormatter() = default;

    /**
     * @brief Set everything to nullptr.
     */
    AbstractOutputFormatter()
    {
        reinit(nullptr, nullptr, nullptr, nullptr);
    };

    // This class shall never be: Moved, Copied or somehow differently be assigned aside from initialising it on an class.

    /**
     * @brief Deleted.
     */
    AbstractOutputFormatter(AbstractOutputFormatter&&) = delete;

    /**
     * @brief Deleted.
     */
    AbstractOutputFormatter(const AbstractOutputFormatter&) = delete;

    /**
     * @brief Deleted.
     */
    AbstractOutputFormatter(AbstractOutputFormatter&) = delete;

    /**
     * @brief Deleted.
     */
    AbstractOutputFormatter& operator=(const AbstractOutputFormatter&) = delete;

    /**
     * @brief Deleted.
     */
    AbstractOutputFormatter& operator=(const AbstractOutputFormatter&&) = delete;
};

/**
 * @brief CSV Format class. Currently not used.
 */
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

/**
 * @brief PrintFormat class. Used as the default print-formatting-facility
 */
class PrintFormat : public AbstractOutputFormatter<PrintFormat> {
private:
public:
    PrintFormat(digestVec& digest, File& F, string& Method_Name, string& Format_Str)
        : AbstractOutputFormatter<PrintFormat>(digest, F, Method_Name, Format_Str)
    {
    }

    explicit PrintFormat() = default;

    [[nodiscard]] string FormatHash() override
    {
        string& str = getFormatStr();
        digestVec digest = getDigest();
        File F = getFile();
        const string& Method_Name = getMethod();
        assert(!digest.empty() && "We cant format something that's empty!");
        assert(F.exists() && "File must exist to hash it!");
        assert(!Method_Name.empty() && "Method name should be an optional parameter and thus needs sadly content...");
        assert(str.empty() && "This class expects it to be empty.");

        string return_str(digest.size() + digest.size(), '\0');
        return_str.append(Poco::format("%s  %s", Poco::DigestEngine::digestToHex(digest), F.path()));

        if (getOption("AddHashToFormat")) {
            return_str.append(Poco::format("; %s.", Method_Name));
        }

        assert(!return_str.empty() && "This should actually hold a string! Cant be empty!");

        return_str += LN;

        return return_str;
    }
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
template<typename T = PrintFormat>
std::string FormatHashToPrint(const std::vector<unsigned char>& digest, Poco::File& F, AbstractOutputFormatter<T>* fmt, const bool AddMethod = false, const std::string& Method_Name = "null")
{

    std::string return_str;
    std::string method = Method_Name;
    assert(fmt);
    assert(F.exists() && "File must exist to hash it!");
    assert(!method.empty() && "Method name should be an optional parameter and thus needs sadly content...");
    fmt->reinit(digest, F, method, return_str);
    assert(fmt->good() && "All pointers should be set on usage to a value that's actually usable."); // asserts all stored pointers...
    fmt->setOption(AddMethod, "AddHashToFormat");

    return fmt->FormatHash();
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
