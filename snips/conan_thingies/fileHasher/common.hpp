//
// Created by darksider3 on 22.01.21.
//

#ifndef POCO_FILE_HASHER_COMMON_HPP
#define POCO_FILE_HASHER_COMMON_HPP

#include "Poco/LineEndingConverter.h"

/// Line Ending Macro
#define LN Poco::LineEnding::NEWLINE_DEFAULT; // NOLINT(cppcoreguidelines-macro-usage)

/// Versioning String, also copyright and stuff
constexpr char version_str[] = "Licensed under MIT. (c) 2021, fileHasher Version 0.2.7 by darksider3. ";

constexpr size_t Read_Segmentation = 16777216; // 16 mbyte
#endif                                         //POCO_FILE_HASHER_COMMON_HPP