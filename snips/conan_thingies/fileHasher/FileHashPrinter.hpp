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
 * @brief Updates a given DigestEngine with the contents of a file
 *
 * @param Poco::DigestEngine& Engine     Engine to feed
 * @param Poco::File&         F          File to feed it with.
 */
__attribute__((flatten)) void ReadFileIntoEngine(Poco::DigestEngine& Engine, Poco::File& F)
{
    Poco::FileInputStream FileReadStream { F.path() };
    char read[Read_Segmentation + 1];

    for (; FileReadStream.eof() == false;) {          // read whole file
        FileReadStream.read(read, Read_Segmentation); // but not everything at once
        Engine.update(read, FileReadStream.gcount()); // update the engine to consider it's content
    }
}

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
    assert(!Path.empty() && !Method.empty() && "This both shouldnt be empty, ever.");
    Poco::File iFile { Path };
    if (!iFile.canRead()) { // cant generate a hash if i cant read what i've got to hash
        return output;
    }

    std::unique_ptr<Poco::Crypto::DigestEngine> Engine = std::make_unique<Poco::Crypto::DigestEngine>(Method);

    Poco::DigestOutputStream ds(*Engine.get());

    ReadFileIntoEngine(*Engine, iFile);

    output << Formatting::FormatHashPrint(Engine->digest(), iFile, used_algorithm, Method); // print 'em out!
    output.flush();                                                                         // make sure it's in there!

    assert(output.good() && "We somehow f*cked up the stream state!");
    return output;
}

#endif // POCO_MD5_ENCRYPT_FILEHASHPRINTER_HPP
