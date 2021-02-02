//
// Created by darksider3 on 02.02.21.
//

#ifndef POCO_FILE_HASHER_BSDFORMAT_HPP
#define POCO_FILE_HASHER_BSDFORMAT_HPP
#include "Poco/Format.h"
#include "Print/Format/AbstractOutputFormatter.hpp"
#include "common.hpp"
#include <Poco/DigestEngine.h>
#include <Poco/File.h>
#include <string>

#include <iostream>

namespace Formatting {

/**
 * @brief BSD Format class.
 */
class BSDFormat : public AbstractOutputFormatter<BSDFormat> {
public:
    /**
     * @brief Constructor
     *
     * @param digestVec& digest Digest to format
     * @param File& F File to get the current path from
     * @param view Method_Name Hash methods name used to create the digest
     * @param view Format_Str String to format in
     */
    BSDFormat(digestVec& digest, File& F, view Method_Name, view Format_Str)
        : AbstractOutputFormatter<BSDFormat>(digest, F, Method_Name, Format_Str)
    {
    }

    /**
     * @brief Format current data in the format of `DIGESTNAME (Filename_without_./) = hash`
     *
     * @return std::string formatted string
     */
    [[nodiscard]] std::string FormatHash() override
    {
        replaceFormatStr(
            Poco::format(Poco::cat(std::string("%s (%s) = %s"), LN),
                Poco::toUpper(getMethod()),
                Formatting::Tooling::cleanPathFromDotSlash(getFile().path()),
                Poco::DigestEngine::digestToHex(getDigest())));
        return getFormatStr();
    }
};
}
#endif //POCO_FILE_HASHER_BSDFORMAT_HPP
