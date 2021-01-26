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

std::string FormatHashAsCSV(const std::vector<unsigned char>& digest, const Poco::File& F, const std::string& Method_Name, std::string& Format_Str)
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
std::string FormatHashToPrint(const std::vector<unsigned char>& digest, const Poco::File& F, const bool AddMethod = false, const std::string& Method_Name = "null")
{
    assert(!digest.empty() && "We cant format something that's empty!");
    assert(F.exists() && "File must exist to hash it!");
    assert(!Method_Name.empty() && "Method name should be an optional parameter and thus needs sadly content...");

    std::string return_str(digest.size(), '\0');
    return_str.append(Poco::format("%s  %s", Poco::DigestEngine::digestToHex(digest), F.path()));

    if (AddMethod) {
        return_str.append(Poco::format("; %s.", Method_Name));
    }

    assert(!return_str.empty() && "This should actually hold a string! Cant be empty!");

    return_str += LN;
    return return_str;
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
