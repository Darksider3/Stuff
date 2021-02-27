//
// Created by darksider3 on 27.02.21.
//

#ifndef JSONTREE_MARKDOWNLINKOPTIONAL_HPP
#define JSONTREE_MARKDOWNLINKOPTIONAL_HPP
#include "common.hpp"
#include "fmt/core.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

namespace JSONTree::Parsers {
/*
 * @TODO: Implement and allow escaping
 * @TODO: Recognize being in a Code-Field(```)
 */
class MarkdownLinkOptionalParser {
private:
    static const int OPS = 9;

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

    struct Symbol {
        MKLINKSyms Sym;
        char terminal;
    };

    struct MKLink {
        std::string TitlePortion {};
        std::string LinkPortion {};
    } tmp;

    Symbol start { .Sym = OP_START, .terminal = 0 };
    Symbol end { .Sym = OP_END, .terminal = 0 };
    Symbol open_bracket { .Sym = FOUND_OPEN_BRACKET, .terminal = '[' };
    Symbol close_bracket { .Sym = FOUND_CLOSE_BRACKET, .terminal = ']' };
    Symbol optional_char { .Sym = FOUND_OPTIONAL_CHAR, .terminal = 0 };
    Symbol open_parenth { .Sym = FOUND_OPEN_PARENTHESIS, .terminal = '(' };
    Symbol close_parenth { .Sym = FOUND_CLOSE_PARENTHESIS, .terminal = ')' };
    Symbol ERR { .Sym = OP_NO_Link, .terminal = 0 };

    std::istringstream m_stream;
    char cur = 0;
    bool escaped { false };
    Symbol* CurrentLookingSym = nullptr;
    std::vector<MKLink> Links {};

    void findBySym(Symbol* lf, Symbol* advanceToSym)
    {
        while (m_stream.good()) {
            cur = m_stream.get();
            if (cur == lf->terminal) {
                CurrentLookingSym = advanceToSym;
                (*this.*functionarr[CurrentLookingSym->Sym])();
                return;
            }
        }
    }

    void findBySymAction(Symbol* lf, Symbol* advanceTo, std::function<void(void)> success_fn, std::function<void(void)> failure_fn, bool runArr = true)
    {
        while (m_stream.good()) {
            cur = m_stream.get();
            if (cur == lf->terminal) {
                success_fn();
                CurrentLookingSym = advanceTo;
                if (runArr)
                    (*this.*functionarr[CurrentLookingSym->Sym])();
                return;
            }
            failure_fn();
        }
    }
    /*
    void findBySimDisallowedChars(Symbol* lf, Symbol* advanceToSym, char* notAllowed)
    {
        if ()
    }
*/
    void FindStart()
    {
        fmt::print("FindStart!\n");
        while (m_stream.good()) {
            cur = m_stream.get();
            m_stream.unget();
            CurrentLookingSym = &open_bracket;
            (*this.*functionarr[CurrentLookingSym->Sym])();
            if (CurrentLookingSym->Sym == end.Sym)
                FindEnd();
        }
    }

    void FindEnd()
    {
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
        findBySymAction(
            OwningSym, &optional_char,
            [this]() { /*nothing*/ },
            [this]() { tmp.TitlePortion += cur; });
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
        findBySym(OwningSym, &close_parenth);
    }
    void FindCloseParenth()
    {
        Symbol* OwningSym = &close_parenth;
        findBySymAction(
            OwningSym, &end,
            [this]() {
                Links.push_back(tmp);
            },
            [this]() {
                tmp.LinkPortion += cur;
            },
            false);
        // Anyway we're going to reset, cuz' we're either not on good() anymore or are done
        tmp = MKLink {};
        return;
    }
    void ErrMsg() { }

    void (MarkdownLinkOptionalParser::*LookingForStarts)() = &MarkdownLinkOptionalParser::FindStart;
    void (MarkdownLinkOptionalParser::*LookingForEnds)() = &MarkdownLinkOptionalParser::FindEnd;
    void (MarkdownLinkOptionalParser::*LookingForOpenBracket)() = &MarkdownLinkOptionalParser::FindOpenBracket;
    void (MarkdownLinkOptionalParser::*LookingForCloseBracket)() = &MarkdownLinkOptionalParser::FindCloseBracket;
    void (MarkdownLinkOptionalParser::*LookingForOptional)() = &MarkdownLinkOptionalParser::FindOptionalChar;
    void (MarkdownLinkOptionalParser::*LookingForOpenParenth)() = &MarkdownLinkOptionalParser::FindOpenParenth;
    void (MarkdownLinkOptionalParser::*LookingForCloseParenth)() = &MarkdownLinkOptionalParser::FindCloseParenth;
    void (MarkdownLinkOptionalParser::*ErrorOut)() = &MarkdownLinkOptionalParser::ErrMsg;

    void (MarkdownLinkOptionalParser::*functionarr[OPS])() = {
        nullptr,
        LookingForStarts, LookingForOpenBracket, LookingForCloseBracket,
        LookingForOptional, LookingForOpenParenth,
        LookingForCloseParenth, LookingForEnds,

        ErrorOut
    };

public:
    MarkdownLinkOptionalParser(const std::string& inputstr)
    {
        m_stream.str(inputstr);
    }

    void DebugRun()
    {
        std::string MKTest = "[Hello World](www.hello.world.de/Sagte/Ich) Nichts hier, tut mir leid. [Aber hier](Haha)";

        m_stream.str(MKTest);

        fmt::print("LF: {}\n--------------------\n", MKTest);
        this->FindStart();
        for (auto& Element : this->Links) {
            fmt::print("\nCurrent MKLink Struct:\n\t * Titleportion: {}\n\t * Linportion: {}\n", Element.TitlePortion, Element.LinkPortion);
        }
    }
};
}
/// \return
#endif //JSONTREE_MARKDOWNLINKOPTIONAL_HPP
