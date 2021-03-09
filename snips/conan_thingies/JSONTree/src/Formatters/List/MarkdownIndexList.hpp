//
// Created by darksider3 on 17.02.21.
//

#ifndef JSONTREE_MARKDOWNINDEXLIST_HPP
#define JSONTREE_MARKDOWNINDEXLIST_HPP

#include "Formatters/Links/MarkdownLink.hpp"
#include "Formatters/Links/OptionalMarkdownLink.hpp"
#include "Formatters/List/IndexList.hpp"
#include "common.hpp"
#include <fstream>
#include <vector>

namespace JSONTree::Formatting::List {

/**
 * @brief Produces an MarkdownList consisting of just the Link and it's name in the specified Link format(HTML, Markdown, Dokuwiki..)
 * @tparam LinkFormatter The Link formatter to be used to produce the list
 */
template<typename LinkFormatter = JSONTree::Formatting::Links::LinkFormat>
class MarkdownIndexList : public IndexList {
private:
    /**
     * @brief Holder for the possibility of the system *not* to produce ./$Name links, so we would append them when neccessary
     */
    std::string m_SlashDotBehaivour = "";

    /**
     * @brief Root path we execute from
     */
    const fs::path m_fs_root;

    /**
     * @brief Vector of already formatted links
     */
    std::vector<std::string> Items;

    /**
     * @brief FormattingObject to produce Links with
     */
    LinkFormatter FormatObj {};

public:
    /**
     * @brief Constructor
     * @param r the root path to produce relative links to
     */
    MarkdownIndexList(fs::path& r)
        : m_fs_root(r)
    {
        if (fs::relative(fs::current_path(), fs::current_path().parent_path()).string().starts_with('/')) {
            m_SlashDotBehaivour = "./";
        }
    }

    /**
     * @brief Format and insert a given path
     * @param p An std::filesystem::path
     */
    void insert(const fs::path& p) override
    {
        Items.push_back(format(p));
    }

    /**
     * @brief Formats a given Path to be in the correct Format of the list
     * @param p An std::filesystem::path to point the link to
     * @return formatted string
     */
    [[nodiscard]] std::string format(const fs::path& p) override
    {
        auto RelativePath = fs::relative(p, m_fs_root);
        auto DotSlashPath = m_SlashDotBehaivour + RelativePath.string();
        return fmt::format("- {}\n", FormatObj.Format(RelativePath, DotSlashPath));
    }

    /**
     * @brief Returns an finished string consisting of all produced links in a markdown list(-)
     * @return std::string the produced list
     */
    [[nodiscard("Result requested but not captured!")]] std::string getAll() override
    {
        std::string List;

        for (auto& Element : Items) {
            List.append(Element);
        }

        return List;
    }
};

using OptionalMarkdownLinkList = MarkdownIndexList<JSONTree::Formatting::Links::OptionalMarkdownLink>;
using MarkdownLinkList = MarkdownIndexList<JSONTree::Formatting::Links::MarkdownLink>;
}
#endif //JSONTREE_MARKDOWNINDEXLIST_HPP
