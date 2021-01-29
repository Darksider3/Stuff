//
// Created by darksider3 on 29.01.21.
//

#ifndef POCO_FILE_HASHER_PRINTFORMAT_HPP
#define POCO_FILE_HASHER_PRINTFORMAT_HPP
#include "Poco/Format.h"
#include "Print/Format/AbstractOutputFormatter.hpp"
#include "common.hpp"
#include <Poco/DigestEngine.h>
#include <Poco/File.h>
#include <string>
#include <string_view>

namespace Formatting {
/**
 * @brief PrintFormat class. Used as the default print-formatting-facility
 */
class PrintFormat : public AbstractOutputFormatter<PrintFormat> {
private:
public:
    explicit PrintFormat(digestVec& digest, File& F, view Method_Name, view Format_Str)
        : AbstractOutputFormatter<PrintFormat>(digest, F, Method_Name, Format_Str)
    {
    }

    explicit PrintFormat() = default;

    [[nodiscard]] string FormatHash() override
    {
        string str = getFormatStr();
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
}
#endif //POCO_FILE_HASHER_PRINTFORMAT_HPP
