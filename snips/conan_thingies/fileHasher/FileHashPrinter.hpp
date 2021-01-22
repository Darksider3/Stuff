//
// Created by darksider3 on 22.01.21.
//

#ifndef POCO_MD5_ENCRYPT_FILEHASHPRINTER_HPP
#define POCO_MD5_ENCRYPT_FILEHASHPRINTER_HPP

#include "Poco/DigestStream.h"
#include "Poco/FileStream.h"
#include "common.hpp"

constexpr size_t Read_Segmentation = 16777216; // 16 mbyte
#ifdef ENABLE_PAIRS
#    include <utility>
std::pair<std::string, std::string> PrintFileHash(const std::string& in, const std::string& Method, bool used_algorithm = false, std::ostream& output = std::cout)

#else

void PrintFileHash(const std::string& in, const std::string& Method, bool used_algorithm = false, std::ostream& output = std::cout)
#endif
{
    Poco::File iFile { in };
    if (!iFile.canRead()) {
#ifdef ENABLE_PAIRS
        return std::pair { "", "" };
#else
        return;
#endif
    }

    std::unique_ptr<Poco::Crypto::DigestEngine> Engine = std::make_unique<Poco::Crypto::DigestEngine>(Method);

    Poco::DigestOutputStream ds(*Engine.get());
    Poco::FileInputStream FileReadStream { iFile.path() };

    std::string read = std::string();
    read.reserve(Read_Segmentation);

    for (; !FileReadStream.eof();) {
        FileReadStream.read(&read[0], Read_Segmentation);
        Engine->update(read.c_str(), FileReadStream.gcount());
    }

    output << Poco::DigestEngine::digestToHex(Engine->digest()) << "  " << iFile.path() /* print 'em out */;
    if (used_algorithm)
        output << "; Method " << Engine->algorithm() << "";
    output << LN;
    output.flush();

#ifdef ENABLE_PAIRS
    return std::pair<std::string, std::string> { Method, std::move(Poco::DigestEngine::digestToHex(Engine->digest())) };
#else
    return;
#endif
}
#endif //POCO_MD5_ENCRYPT_FILEHASHPRINTER_HPP
