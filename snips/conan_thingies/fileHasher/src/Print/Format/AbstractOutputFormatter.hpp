//
// Created by darksider3 on 28.01.21.
//

#ifndef POCO_FILE_HASHER_ABSTRACTOUTPUTFORMATTER_HPP
#define POCO_FILE_HASHER_ABSTRACTOUTPUTFORMATTER_HPP
#include "common.hpp"
#include <Poco/DigestEngine.h>
#include <Poco/File.h>
#include <Poco/Format.h>
#include <string>
#include <string_view>
#include <unordered_map>

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
    /**
     * @brief Digest by Poco::Util
     */
    [[maybe_unused]] std::vector<unsigned char> m_digest {};
    [[maybe_unused]] Poco::File m_File {};
    [[maybe_unused]] std::string m_Method {};
    [[maybe_unused]] std::string m_FormatStr {};

    AbstractOutputFormatter<CRTP>* u() { return &static_cast<CRTP&>(*this); }
    [[nodiscard]] const AbstractOutputFormatter<CRTP>* u_c() { return static_cast<const CRTP*>(this); }

    std::unordered_map<std::string, bool> BoolFlags {};

public:
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
     * @brief Pass strings through a view, not the string-class!
     */
    using view = std::string_view;

protected:
    /**
     * @return digestVec& Digest-Engines produced Digest
     */
    [[maybe_unused]] const digestVec&
    getDigest()
    {
        return u()->m_digest;
    }

    /**
     * @return  const File& File currently operating on
     */
    [[maybe_unused]] const File& getFile()
    {
        return u()->m_File;
    }

    /**
     * @return string Hashing method used on File.
     */
    [[maybe_unused]] string getMethod()
    {
        return u()->m_Method;
    }

    /**
     * @return string Formatted string
     */
    [[maybe_unused]] string getFormatStr()
    {
        return u()->m_FormatStr;
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
    [[maybe_unused]] [[nodiscard]] const digestVec& getDigest() const
    {
        return u()->m_digest;
    }

    /**
     * @brief See getFile()
     */
    [[maybe_unused]] [[nodiscard]] const File& getFile() const
    {
        return u_c()->m_File;
    }

    /**
     * @brief See getMethod()
     */
    [[maybe_unused]] [[nodiscard]] string getMethod() const
    {
        return u()->m_Methodt;
    }

    /**
     * @brief See getFormatStr
     */
    [[maybe_unused]] [[nodiscard]] string getFormatStr() const
    {
        return u()->m_FormatStr;
    }

    /**
     * @brief See getOptionsMap()
     */
    [[maybe_unused]] [[nodiscard]] OptionsMap getOptionsMap() const
    {
        return u()->BoolFlags;
    }

    /**
     * @brief Set Digest-Member
     * @param digestVec& digest  Digest produced by Poco::Crypto::DigestEngine->digest();
     */
    [[maybe_unused]] void replaceDigest(digestVec& digest) { u()->m_digest = digest; }

    /**
     * @brief Set File currently operating on
     * @param File& file  Current Targetfile
     */
    [[maybe_unused]] void replaceFile(File& file) { u()->m_File = file; }

    /**
     * @brief Set Hash method used to produce the digest
     * @param std::string_view str Hash methods name
     */
    [[maybe_unused]] void replaceMethod(view str) { u()->m_Method = str; }

    /**
     * @brief Set Formatting string to write into
     * @param std::string_view str  String to operate on
     */
    [[maybe_unused]] void replaceFormatStr(view str) { u()->m_FormatStr = str; }

    /**
     * @brief Append-Only operation for FormatStr
     * @param std::string_view String to append to FormatStr
     */

    [[maybe_unused]] void appendToFormatStr(view str) { u()->m_FormatStr.append(str); }

public:
    explicit AbstractOutputFormatter(digestVec& digest, File& F, view Method_Name, view Format_Str)
        : m_digest(digest)
        , m_File(F)
        , m_Method(Method_Name)
        , m_FormatStr(Format_Str)
    {
    }

    /**
     * @brief Reinitialise all relevant class members
     *
     * @param digestVec& digest Digest reinitialise
     * @param Files& F File to operate on
     * @param std::string_view Method_Name Hashmethods name to produce digest
     * @param std::string_view str String to modify with formatting
     */
    virtual void reinit(digestVec& digest, File& F, view Method_Name, view str)
    {
        u()->replaceDigest(digest);
        u()->replaceFile(F);
        u()->replaceMethod(Method_Name);
        u()->replaceFormatStr(str);
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
     * @brief FormatHash is the main method any subclass should implement to use the behaivour of this class. It's the logic-function. If you've got anything to format, do it here
     *
     * @return string Formatted string
     */
    virtual string FormatHash() = 0;

    /**
     * @brief Standard interface to call FormatHash() for other classes because it's declared at any time.
     *
     * @return string Formatted string
     */
    [[maybe_unused]] virtual string Work() { return FormatHash(); }

    [[maybe_unused]] [[nodiscard]] bool good()
    {
        /*if (u()->getMethod().empty())
            std::cerr << "Method empty!\n";
        if (u()->getDigest().empty())
            std::cerr << "Digest empty!\n";
        if (!u()->getFile().exists())
            std::cerr << "File doesnt exist.!\n";
        std::flush(std::cerr);*/
        return (
            !u()->getMethod().empty()
            && !u()->getDigest().empty()
            && u()->getFile().exists());
    }

    /**
     * @brief Deleted.
     */
    virtual ~AbstractOutputFormatter() = default;

    AbstractOutputFormatter() = default;

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

}
#endif //POCO_FILE_HASHER_ABSTRACTOUTPUTFORMATTER_HPP
