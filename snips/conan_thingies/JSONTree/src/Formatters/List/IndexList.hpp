//
// Created by darksider3 on 17.02.21.
//

#ifndef JSONTREE_INDEXLIST_HPP
#define JSONTREE_INDEXLIST_HPP
#include "Formatters/Links/MarkdownLink.hpp"
#include "common.hpp"

class IndexList {
    virtual void insert(const fs::path& p) = 0;
    virtual std::string format(const fs::path& p) = 0;
    virtual std::string getAll() = 0;
};

#endif //JSONTREE_INDEXLIST_HPP
