//
// Created by darksider3 on 17.02.21.
//

#ifndef JSONTREE_HTMLLINK_HPP
#define JSONTREE_HTMLLINK_HPP
#include "LinkFormat.hpp"
#include "common.hpp"

/**
 * @brief An HTML Link Formatter
 */
class HTMLLink : public LinkFormat {
public:
    struct Options {
        std::string alt {};
        std::string target {};
    };

private:
    Options Opts;

public:
    HTMLLink(Options&& opts)
        : Opts(opts)
    {
    }

    [[nodiscard]] std::string Format(const std::string& name, const std::string& link) override
    {
        std::string Result { fmt::format("<a href=\"{}\" ", link) };
        if (!Opts.alt.empty()) {
            Result.append(fmt::format("alt=\"{}\" ", Opts.alt));
        }

        if (!Opts.target.empty()) {
            Result.append(fmt::format(" target=\"{}\"", Opts.target));
        }

        Result.append(fmt::format(">{}</a>", name));

        return Result;
    }
};
#endif //JSONTREE_HTMLLINK_HPP
