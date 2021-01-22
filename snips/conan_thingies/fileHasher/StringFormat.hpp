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
std::string FormatHashPrint(const std::vector<unsigned char>& digest, const Poco::File& F, const bool AddMethod = false, const std::string_view Method_Name = "null")
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
