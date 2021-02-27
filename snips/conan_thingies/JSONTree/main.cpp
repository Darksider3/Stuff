#include "Parsers/MarkdownLinkOptional.hpp"
#include "common.hpp"
#include <Formatters/Links/OptionalMarkdownLink.hpp>
#include <Formatters/List/MarkdownIndexList.hpp>
#include <fstream>
#include <sstream>
#include <vector>
std::vector<fs::path> GetFilesWithPrefix(std::string_view prefix, fs::path& from)
{
    std::vector<fs::path> ResultVec {};
    auto RecursiveIt = fs::recursive_directory_iterator(from);
    for (auto& PossibleFile : RecursiveIt) {
        auto possiblePath = PossibleFile.path().filename().string();
        if (PossibleFile.is_regular_file() && possiblePath.starts_with(prefix)) {
            ResultVec.push_back(PossibleFile);
        }
    }

    return ResultVec;
}

template<class ListIndex>
ListIndex getFilesListForIndex(fs::path& root, fs::path& Indexfile, std::string_view Suffixfilter)
{
    fs::path Root = root;
    ListIndex List(Root);

    auto DirectoryIter = fs::directory_iterator(Indexfile.parent_path());
    for (auto& EntryInIndexDir : DirectoryIter) {
        auto& Path = EntryInIndexDir.path();
        if (Path == Indexfile || !EntryInIndexDir.is_regular_file() || !Path.string().ends_with(Suffixfilter)) {
            continue;
        }

        List.insert(Path);
    }

    return List;
}

int main()
{
    std::string_view FileSuffixFilter { ".md" };
    std::string_view FilePrefixFilter { "index_" };
    std::vector<fs::path> Indexes;
    auto CurPath = fs::current_path();
    // Step 1 - Push Indexes back
    Indexes = GetFilesWithPrefix(FilePrefixFilter, CurPath);

    // Step 2: Build Directory Layout
    for (auto& Index_File : Indexes) {
        auto List = getFilesListForIndex<JSONTree::Formatting::List::OptionalMarkdownLinkList>(CurPath, Index_File, FileSuffixFilter);

        std::ofstream writer { Index_File };
        writer << List.getAll();
        writer.close();
    }

    std::string Test { "World" };
    JSONTree::Parsers::MarkdownLinkOptionalParser Thing { Test };
    Thing.DebugRun();
}