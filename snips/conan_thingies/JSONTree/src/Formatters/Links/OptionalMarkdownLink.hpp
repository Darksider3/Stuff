//
// Created by darksider3 on 17.02.21.
//

#ifndef JSONTREE_OPTIONALMARKDOWNLINK_HPP
#define JSONTREE_OPTIONALMARKDOWNLINK_HPP
#include "LinkFormat.hpp"
#include "common.hpp"

/**
 * @brief "Optional Markdown Links" are usual Markdown links but with an ! in between: [Name]!(link).
 *
 * I just invented that one for my own knowledge base to have an option to parse instead of setting it in stone. Like
 * generating graphs from the links. Makes the whole thing a little bit more dynamic. :)
 */
class OptionalMarkdownLink : public LinkFormat {
public:
    std::string Format(const std::string& name, const std::string& link) override
    {
        return fmt::format("[{}]!({})", name, link);
    }
};

#endif //JSONTREE_OPTIONALMARKDOWNLINK_HPP
