//
// Created by darksider3 on 01.02.21.
//

#ifndef POCO_FILE_HASHER_CSVFORMAT_HPP
#define POCO_FILE_HASHER_CSVFORMAT_HPP
#include "Poco/Format.h"
#include "Print/Format/AbstractOutputFormatter.hpp"
#include "common.hpp"
#include <Poco/DigestEngine.h>
#include <Poco/File.h>
#include <string>

#include <iostream>

namespace Formatting {
namespace CSV_Details {

constexpr char CSVHeader[] = "Filename,Digestid,Hash";

}
/**
 * @brief CSV Format class.
 */
class CSVFormat : public AbstractOutputFormatter<CSVFormat> {
private:
    bool m_PresentHeader { false };

public:
    /**
     * @brief Constructor
     *
     * @param digestVec& digest Digest to format
     * @param File& F File to get the current path from
     * @param view Method_Name Hash methods name used to create the digest
     * @param view Format_Str String to format in
     */
    CSVFormat(digestVec& digest, File& F, view Method_Name, view Format_Str)
        : AbstractOutputFormatter<CSVFormat>(digest, F, Method_Name, Format_Str)
    {
    }

    /**
     * @brief Overload to insert new entries into the formatted string. Does **not** reinitialise anything at all. Only appends to the currently used!
     *
     * @param digestVec& digest Created digest by Poco::DigestEngine::Engine->digest();
     * @param File& F File used to get the hash from
     * @param view Method_Name Methods name to create current entry
     * @param view str Unused.
     */
    void reinit(digestVec& digest, File& F, view Method_Name, view /*str*/) override
    {
        if (!HasCSVHeader()) {
            InsertCSVHeader();
            std::string hexdig = Poco::DigestEngine::digestToHex(digest);
            appendToFormatStr(F.path() + "," + hexdig + "," + std::string(Method_Name) + LN);
        } else {
            std::string hexdig = Poco::DigestEngine::digestToHex(digest);
            replaceFormatStr(F.path() + "," + hexdig + "," + std::string(Method_Name) + LN);
        }
    }

    /**
     * @brief Overload for being conform to the abstract interface
     *
     * @return std::string formatted string
     */
    [[nodiscard]] std::string FormatHash() override
    {
        return getFormatStr();
    }

    /**
     * @brief Checker function to decide on wether we need a header or not
     *
     * @return True, if currently there is an header. False when not.
     */
    [[nodiscard]] bool HasCSVHeader()
    {
        return m_PresentHeader;
    }

    /**
     * @brief Inserts header into the formatstring given to the constructor
     */
    void InsertCSVHeader()
    {
        string ins = getFormatStr().insert(0, CSV_Details::CSVHeader + LN);
        replaceFormatStr(ins);
        m_PresentHeader = true;
    }
};

}
#endif //POCO_FILE_HASHER_CSVFORMAT_HPP
