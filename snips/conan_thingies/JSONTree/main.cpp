#include "Formatters/Links/MarkdownLink.hpp"
#include "Formatters/List/MarkdownIndexList.hpp"
#include "common.hpp"
#include <fstream>
#include <vector>

int main()
{
    std::string_view FileSuffixFilter { ".md" };
    std::string_view FilePrefixFilter { "index_" };
    std::vector<fs::path> Indexes;
    auto CurPath = fs::current_path();
    auto RcrsvIterator = fs::recursive_directory_iterator(CurPath);

    // Step 1 - Push Indexes back
    for (auto& PossibleIndex : RcrsvIterator) {
        auto& PossbileIndexPath = PossibleIndex.path();
        if (PossibleIndex.is_regular_file() && starts_with(PossbileIndexPath.filename(), FilePrefixFilter)) {
            Indexes.push_back(PossbileIndexPath);
        }
    }

    // Step 2: Build Directory Layout
    for (auto& Index_File : Indexes) {
        auto DirectoryIter = fs::directory_iterator(Index_File.parent_path());
        auto List = MarkdownIndexList<MarkdownLink>(fs::current_path());
        for (auto& EntryInIndexDir : DirectoryIter) {
            /*
             * Whenever the following criteria doesnt apply, we ignore the entry:
             *  - Its the same file(Index == Entry)
             *  - It isnt a regular file
             *  - it doesnt end with the specified suffix
             */
            if (EntryInIndexDir.path() == Index_File || !EntryInIndexDir.is_regular_file() || !ends_with(EntryInIndexDir.path(), FileSuffixFilter))
                continue;

            List.insert(EntryInIndexDir.path());
        }

        std::ofstream writer { Index_File };
        writer << List.getAll();
        writer.close();
    }
}