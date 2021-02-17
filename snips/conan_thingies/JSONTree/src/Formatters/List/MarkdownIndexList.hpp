//
// Created by darksider3 on 17.02.21.
//

#ifndef JSONTREE_MARKDOWNINDEXLIST_HPP
#define JSONTREE_MARKDOWNINDEXLIST_HPP

#include "Formatters/Links/MarkdownLink.hpp"
#include "common.hpp"
#include <fstream>
#include <vector>

template<typename LinkFormatter = LinkFormat>
class MarkdownIndexList {
private:
    std::string m_SlashDotBehaivour = "";
    const fs::path m_fs_root;
    std::vector<std::string> Items;
    LinkFormatter FormatObj {};

public:
    MarkdownIndexList(const fs::path&& r)
        : m_fs_root(r)
    {
        if (!starts_with(fs::relative(fs::current_path(), fs::current_path().parent_path()), "./"))
            m_SlashDotBehaivour = "./";
    }

    void insert(const fs::path& p)
    {
        Items.push_back(format(p));
    }

    [[nodiscard]] std::string format(const fs::path& p)
    {
        auto RelativePath = fs::relative(p, m_fs_root);
        auto DotSlashPath = m_SlashDotBehaivour + RelativePath.string();
        return fmt::format("- {}\n", FormatObj.Format(RelativePath, DotSlashPath));
    }

    [[nodiscard("Result requested but not captured!")]] std::string getAll()
    {
        std::string List;

        for (auto& Element : Items) {
            List.append(Element);
        }

        return List;
    }
};

#endif //JSONTREE_MARKDOWNINDEXLIST_HPP
