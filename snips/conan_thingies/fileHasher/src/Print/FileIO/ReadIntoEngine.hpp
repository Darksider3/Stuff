//
// Created by darksider3 on 26.01.21.
//

#ifndef POCO_FILE_HASHER_READINTOENGINE_HPP
#define POCO_FILE_HASHER_READINTOENGINE_HPP
#include "../../../common.hpp"
#include "Poco/DigestEngine.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"

namespace FileIO {
/**
 * @brief Updates a given DigestEngine with the contents of a file
 *
 * @param Poco::DigestEngine& Engine     Engine to feed
 * @param Poco::File&         F          File to feed it with.
 */
__attribute__((flatten)) Poco::File ReadFileIntoEngine(Poco::DigestEngine& Engine, Poco::File&& F)
{
    assert(!Engine.digest().empty() && "The Digest Engine *must* be initialized before usage here!");
    assert(F.exists() && "Actually POCO takes care for existence here, but hell it __should__ exist!");
    Poco::FileInputStream FileReadStream { F.path() };

    assert(FileReadStream.good() && "Stream must work in order to work on it.");
    char read[Read_Segmentation + 1];

    for (; FileReadStream.eof() == false;) {          // read whole file
        FileReadStream.read(read, Read_Segmentation); // but not everything at once
        Engine.update(read, FileReadStream.gcount()); // update the engine to consider it's content
    }

    assert(FileReadStream.eof() && "We. need. the. whole. file!");
    FileReadStream.close();

    return F;
}
}
#endif //POCO_FILE_HASHER_READINTOENGINE_HPP
