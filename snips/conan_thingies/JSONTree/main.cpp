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
public:
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
    } m_Certanity
        = NoIndications;

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

    std::string MKTest = "[Hello World](www.hello.world.de/Sagte/Ich) Nichts hier, tut mir leid. [Aber hier](Haha)";
    std::stringstream m_stream { MKTest };
    bool escaped { false };
    Symbol* CurrentLookingSym;
    char cur = 0;

    struct MKLink {
        std::string TitlePortion {};
        std::string LinkPortion {};
    } tmp;

    std::vector<MKLink> Links {};

    void FindStart()
    {
        fmt::print("FindStart!\n");
        const Symbol* OwningSym = &start;
        while (m_stream.good()) {
            cur = m_stream.get();
            if (cur == '[') {
                m_Certanity = NoIdea;
                m_stream.unget();
                CurrentLookingSym = &open_bracket;
                (*this.*functionarr[CurrentLookingSym->Sym])();
                if (CurrentLookingSym->Sym == end.Sym)
                    FindEnd();
            }
        }
    }

    void FindEnd()
    {
        fmt::print("Ended matching, returning to start!\n");
        CurrentLookingSym = &start;
        return;
    }

    void FindOpenBracket()
    {
        if (m_stream.good())
            cur = m_stream.get();
        else
            return;
        Symbol* OwningSym = &open_bracket;
        if (cur == OwningSym->terminal) {
            CurrentLookingSym = &close_bracket;
            (*this.*functionarr[CurrentLookingSym->Sym])();
        }
        return;
    }

    void FindCloseBracket()
    {
        Symbol* OwningSym = &close_bracket;
        while (m_stream.good()) {
            cur = m_stream.get();
            if (cur == OwningSym->terminal) {
                CurrentLookingSym = &optional_char;
                (*this.*functionarr[CurrentLookingSym->Sym])();
                return;
            } else {
                tmp.TitlePortion += cur;
            }
        }
    }

    // fmt::print("Cur = {}, ", cur);
    void FindOptionalChar()
    {
        Symbol* OwningSym = &optional_char;
        if (OwningSym->terminal != 0) {
            //do something to find non-zero-terminals
        } else {
            CurrentLookingSym = &open_parenth;
            (*this.*functionarr[CurrentLookingSym->Sym])();
            return;
        }

        return;
    }
    void FindOpenParenth()
    {
        Symbol* OwningSym = &open_parenth;
        while (m_stream.good()) {
            cur = m_stream.get();
            if (cur == OwningSym->terminal) {
                CurrentLookingSym = &close_parenth;
                (*this.*functionarr[CurrentLookingSym->Sym])();
                return;
            }
        }
    }
    void FindCloseParenth()
    {
        Symbol* OwningSym = &close_parenth;
        while (m_stream.good()) {
            cur = m_stream.get();
            if (cur == OwningSym->terminal) {
                Links.push_back(tmp);
                CurrentLookingSym = &end;
                break;
            } else {
                tmp.LinkPortion += cur;
            }
        }
        // Anyway we're going to reset, cuz' we're either not on good() anymore or are done
        tmp = MKLink {};
    }
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
        LookingForStarts, LookingForOpenBracket, LookingForCloseBracket,
        LookingForOptional, LookingForOpenParenth,
        LookingForCloseParenth, LookingForEnds,

        ErrorOut
    };

public:
    Parser()
    {
        m_stream.str(MKTest);
        fmt::print("LF: {}\n--------------------\n", MKTest);
    }
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

    Parser Thing {};
    Thing.FindStart();
    for (auto& Element : Thing.Links) {
        fmt::print("\nCurrent MKLink Struct:\n\t * Titleportion: {}\n\t * Linportion: {}", Element.TitlePortion, Element.LinkPortion);
    }
}