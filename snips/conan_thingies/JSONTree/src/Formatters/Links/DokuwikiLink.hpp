//
// Created by darksider3 on 17.02.21.
//

#ifndef JSONTREE_DOKUWIKILINK_HPP
#define JSONTREE_DOKUWIKILINK_HPP
#include "LinkFormat.hpp"
#include "common.hpp"

namespace JSONTree::Formatting::Links {
class DokuwikiLink : public LinkFormat {
public:
    [[nodiscard]] std::string Format(const std::string& name, const std::string& link) override
    {
        return fmt::format("[[{}|{}]]", name, link);
    }
};
}
#endif //JSONTREE_DOKUWIKILINK_HPP
