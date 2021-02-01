//
// Created by darksider3 on 22.01.21.
//

#ifndef POCO_FILE_HASHER_COMMON_HPP
#define POCO_FILE_HASHER_COMMON_HPP

#include "Poco/Format.h"
#include "Poco/LineEndingConverter.h"
#include <Poco/DigestEngine.h>
#include <Poco/File.h>
#include <cassert>

#define ASSERT_NOT_REACHED() assert(false && "This section shouldn't be reached, ever!") // NOLINT(cppcoreguidelines-macro-usage)

/// Line Ending Macro
#define LN Poco::LineEnding::NEWLINE_DEFAULT // NOLINT(cppcoreguidelines-macro-usage)

/// Versioning String, also copyright and stuff
constexpr char version_str[] = "Licensed under MIT. (c) 2021, fileHasher Version 0.3.8 by darksider3. ";

constexpr size_t Read_Segmentation = 8192; // default linux kernel page size

#endif //POCO_FILE_HASHER_COMMON_HPP
