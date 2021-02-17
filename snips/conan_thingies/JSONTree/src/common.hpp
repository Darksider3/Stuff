//
// Created by darksider3 on 16.02.21.
//

#ifndef JSONTREE_COMMON_HPP
#define JSONTREE_COMMON_HPP
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <string_view>

namespace fs = std::filesystem;

bool starts_with(std::string_view View, std::string_view SearchFor)
{
    return (View.rfind(SearchFor, 0) == 0);
}

bool starts_with(const std::string& Str, std::string_view SearchFor)
{
    return (Str.rfind(SearchFor, 0) == 0);
}

bool ends_with(std::string_view View, std::string_view SearchFor)
{
    return View.size() >= SearchFor.size() && 0 == View.compare(View.size() - SearchFor.size(), SearchFor.size(), SearchFor);
}

bool ends_with(std::string View, std::string_view SearchFor)
{
    return View.size() >= SearchFor.size() && 0 == View.compare(View.size() - SearchFor.size(), SearchFor.size(), SearchFor);
}
#endif //JSONTREE_COMMON_HPP
