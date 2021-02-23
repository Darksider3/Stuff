//
// Created by darksider3 on 17.02.21.
//

#ifndef JSONTREE_LINKFORMAT_HPP
#define JSONTREE_LINKFORMAT_HPP
#include "common.hpp"

namespace JSONTree::Formatting::Links {

class LinkFormat {
public:
    virtual std::string Format(const std::string& name, const std::string& link) = 0;
};
}
#endif //JSONTREE_LINKFORMAT_HPP
