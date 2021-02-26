#include "common.hpp"
#include <Formatters/Links/OptionalMarkdownLink.hpp>
#include <Formatters/List/MarkdownIndexList.hpp>
#include <fstream>
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

/// \brief

enum MKLINKSyms {
    OP_START = 1,
    FOUND_OPEN_BRACKET = 2,
    FOUND_CLOSE_BRACKET = 3,
    FOUND_OPTIONAL_CHAR = 4,
    FOUND_OPEN_PARENTHESIS = 5,
    FOUND_CLOSE_PARENTHESIS = 6,
    OP_END = 7,
    OP_NO_Link = 8
};

enum Certainity {
    NoIdea = 1,
    Possible = 2,
    Probably = 3,
    Definitly = 4
};

struct Symbol {
    MKLINKSyms Sym;
    char character;
};

Symbol start { .Sym = OP_START, .character = 0 };
Symbol end { .Sym = OP_END, .character = 0 };
Symbol open_bracket { .Sym = FOUND_OPEN_BRACKET, .character = '[' };
Symbol close_bracket { .Sym = FOUND_CLOSE_BRACKET, .character = ']' };
Symbol optional_char { .Sym = FOUND_OPTIONAL_CHAR, .character = 0 };
Symbol open_parenth { .Sym = FOUND_OPEN_PARENTHESIS, .character = '(' };
Symbol close_parenth { .Sym = FOUND_CLOSE_PARENTHESIS, .character = ')' };
Symbol ERR { .Sym = OP_NO_Link, .character = 0 };

class Parser {
private:
    void (*LookingForStarts)();
    void (*LookingForEnds)();
    void (*LookingForOpenBracket)();
    void (*LookingForCloseBracket)();
    void (*LookingForOptional)();
    void (*LookingForOpenParenth)();
    void (*LookingForCloseParenth)();
    void (*ErrorOut)();

    void (*functionarr[10])() = {
        0,
        LookingForStarts, LookingForOpenBracket, LookingForOpenBracket,
        LookingForCloseBracket, LookingForOptional, LookingForOpenParenth,
        LookingForCloseParenth, LookingForEnds,

        ErrorOut
    };
};

/// \return

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
}