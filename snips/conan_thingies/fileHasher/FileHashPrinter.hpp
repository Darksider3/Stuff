//
// Created by darksider3 on 22.01.21.
//

#ifndef POCO_FILE_HASHER_FILEHASHPRINTER_HPP
#define POCO_FILE_HASHER_FILEHASHPRINTER_HPP

#include "Poco/DigestStream.h"
#include "Poco/FileStream.h"
#include "StringFormat.hpp"
#include "common.hpp"

/**
 * @brief Hashes a file and prints its Hex-representation to the specified output stream
 *
 * @param const std::string&    Path                   Path to file which to create a hash from
 * @param const std::string&    Method                 Method to use for hashing done by OpenSSL
 * @param bool                  used_algorithm         Specifies weither to print the used algorithms alongside the filename or not
 * @param std::ostream&         output=std::cout     Output Stream to write to
 *
 * @return
 */

std::ostream& PrintFileHash(const std::string& Path, const std::string& Method, bool used_algorithm = false, std::ostream& output = std::cout)
{
    Poco::File iFile { Path };
    if (!iFile.canRead()) { // cant generate a hash if i cant read what i've got to hash
        return output;
    }

    std::unique_ptr<Poco::Crypto::DigestEngine> Engine = std::make_unique<Poco::Crypto::DigestEngine>(Method);

    Poco::DigestOutputStream ds(*Engine.get());
    Poco::FileInputStream FileReadStream { iFile.path() };

    std::string read = std::string();
    read.reserve(Read_Segmentation);

    for (; !FileReadStream.eof();) {                           // read whole file
        FileReadStream.read(&read[0], Read_Segmentation);      // but not everything at once
        Engine->update(read.c_str(), FileReadStream.gcount()); // update the engine to consider it's content
    }

    output << Formatting::FormatHashPrint(Engine->digest(), iFile, used_algorithm, Method); // print 'em out!
    output.flush();                                                                         // make sure it's in there!

    return output;
}

#endif // POCO_MD5_ENCRYPT_FILEHASHPRINTER_HPP
