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
#include <string_view>

namespace Formatting {

std::string FormatHashPrint(const std::vector<unsigned char>& digest, const Poco::File& F, bool AddMethod = false, std::string_view Method_Name = "null")
{
    std::string return_str(digest.size(), '\0');
    return_str.append(Poco::format("%s  %s", Poco::DigestEngine::digestToHex(digest), F.path()));
    if (AddMethod) {
        return_str.append(Poco::format("; %s.", Method_Name));
    }

    return_str += LN;

    return return_str;
}

}
#endif //POCO_FILE_HASHER_STRINGFORMAT_HPP
