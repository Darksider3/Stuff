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

//
class Parser {
private:
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
        NoIndications = 0,
        NoIdea = 1,
        Possible = 2,
        Probably = 3,
        Definitly = 4
    } m_Certanity;

    struct Symbol {
        MKLINKSyms Sym;
        char terminal;
    };

    Symbol start { .Sym = OP_START, .terminal = 0 };
    Symbol end { .Sym = OP_END, .terminal = 0 };
    Symbol open_bracket { .Sym = FOUND_OPEN_BRACKET, .terminal = '[' };
    Symbol close_bracket { .Sym = FOUND_CLOSE_BRACKET, .terminal = ']' };
    Symbol optional_char { .Sym = FOUND_OPTIONAL_CHAR, .terminal = 0 };
    Symbol open_parenth { .Sym = FOUND_OPEN_PARENTHESIS, .terminal = '(' };
    Symbol close_parenth { .Sym = FOUND_CLOSE_PARENTHESIS, .terminal = ')' };
    Symbol ERR { .Sym = OP_NO_Link, .terminal = 0 };

    std::string MKTest = "[Hello World](./mat)";
    std::stringstream m_stream { MKTest };
    bool escaped { false };
    Symbol* CurrentLookingSym;
    char cur = 0;

    struct MKLink {
        std::string TitlePortion {};
        std::string LinkPortion {};
    };

    void FindStart()
    {
        const Symbol* OwningSym = &start;
        while (m_stream.good() && m_Certanity <= NoIndications) {
            cur == m_stream.get();
            if (cur == '[') {
                m_Certanity = NoIdea;
                m_stream.unget();
                CurrentLookingSym = &open_bracket;
            }
        }
        (*this.*functionarr[CurrentLookingSym->Sym])();
    }

    void FindEnd()
    {
    }

    void FindOpenBracket()
    {
        cur = m_stream.get();
        Symbol* OwningSym = &open_bracket;
        if (cur == OwningSym->terminal) {
            
        } else {
            OwningSym = &ERR;
        }
    }
    void FindCloseBracket() { }
    void FindOptionalChar() { }
    void FindOpenParenth() { }
    void FindCloseParenth() { }
    void ErrMsg() { }

    void (Parser::*LookingForStarts)() = &Parser::FindStart;
    void (Parser::*LookingForEnds)() = &Parser::FindEnd;
    void (Parser::*LookingForOpenBracket)() = &Parser::FindOpenBracket;
    void (Parser::*LookingForCloseBracket)() = &Parser::FindCloseBracket;
    void (Parser::*LookingForOptional)() = &Parser::FindOptionalChar;
    void (Parser::*LookingForOpenParenth)() = &Parser::FindOpenParenth;
    void (Parser::*LookingForCloseParenth)() = &Parser::FindCloseParenth;
    void (Parser::*ErrorOut)() = &Parser::ErrMsg;

    void (Parser::*functionarr[10])() = {
        nullptr,
        LookingForStarts, LookingForOpenBracket, LookingForOpenBracket,
        LookingForCloseBracket, LookingForOptional, LookingForOpenParenth,
        LookingForCloseParenth, LookingForEnds,

        ErrorOut
    };

public:
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