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
#include <memory>
#include <sstream>
#include <string>
#include <variant>

namespace JSONTree::Parsers {
namespace detail {
constexpr unsigned char escape_char = '\\';
constexpr unsigned char open_bracket = '[';
constexpr unsigned char close_bracket = ']';
constexpr unsigned char open_parenth = '(';
constexpr unsigned char close_parenth = ')';
constexpr unsigned char NONE_CHAR = 0;
}

/*
 * @TODO: Recognize being in a Code-Field(```)
 */
class MarkdownLinkOptionalParser {
private:
    static const int OPS = 9;

    enum MKLINKSyms {
        OP_ERR [[maybe_unused]] = 0,
        OP_START = 1,
        LookingForOpenBracket = 2,
        LookingForClosingBracket = 3,
        LookingForOptionalChar = 4,
        LookingForOpenParenth = 5,
        LookingForCloseParenth = 6,
        OP_END = 7,
        OP_ESCAPE = 8
    };

    struct Symbol {
        MKLINKSyms Sym;
        char terminal;
    };

    struct MKLink {
        std::string TitlePortion {};
        std::string LinkPortion {};

        bool isOptional { false };
    } tmp;

    Symbol start { .Sym = OP_START, .terminal = detail::NONE_CHAR };
    Symbol end { .Sym = OP_END, .terminal = detail::NONE_CHAR };
    Symbol open_bracket { .Sym = LookingForOpenBracket, .terminal = detail::open_bracket };
    Symbol close_bracket { .Sym = LookingForClosingBracket, .terminal = detail::close_bracket };
    Symbol optional_char { .Sym = LookingForOptionalChar, .terminal = detail::NONE_CHAR };
    Symbol open_parenth { .Sym = LookingForOpenParenth, .terminal = detail::open_parenth };
    Symbol close_parenth { .Sym = LookingForCloseParenth, .terminal = detail::close_parenth };
    Symbol ESCAPE { .Sym = OP_ESCAPE, .terminal = detail::escape_char };

    std::istringstream m_stream;
    char cur = 0;
    Symbol* CurrentLookingSym = nullptr;
    std::vector<MKLink> Links {};

    void HandlePossibleEscape(std::string* apTo = nullptr)
    {
        Symbol* OwningSym = &ESCAPE;
        if (cur == OwningSym->terminal) {
            if (apTo != nullptr) {
                cur = m_stream.get();
                *apTo += cur;
            } else {
                m_stream.ignore();
            }
            cur = m_stream.get(); // After we ignored one char, the next will be again
        }
    }

    void findBySym(Symbol* lf, Symbol* advanceToSym, std::string* appendTo = nullptr)
    {
        while (m_stream.good()) {
            cur = m_stream.get();
            HandlePossibleEscape(appendTo);

            if (cur == lf->terminal) {
                CurrentLookingSym = advanceToSym;
                (*this.*functionarr[CurrentLookingSym->Sym])();
                return;
            }
        }
    }

    void findBySymAction(Symbol* lf, Symbol* advanceTo, std::function<void(void)>&& success_fn, std::function<void(void)>&& failure_fn, bool runArr = true, std::string* appendTo = nullptr)
    {
        while (m_stream.good()) {
            cur = m_stream.get();
            HandlePossibleEscape(appendTo);

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
    void Start()
    {
        fmt::print("Start!\n");
        while (m_stream.good()) {
            cur = m_stream.get();
            m_stream.unget();
            CurrentLookingSym = &open_bracket;
            (*this.*functionarr[CurrentLookingSym->Sym])();
            if (CurrentLookingSym->Sym == end.Sym)
                EndCurrentLink();
        }
    }

    void EndCurrentLink()
    {
        CurrentLookingSym = &start;
        return;
    }

    void abortLink()
    {
        tmp = MKLink {};
    }

    void LF_open_bracket()
    {
        if (m_stream.good())
            cur = m_stream.get();
        else
            return;

        HandlePossibleEscape();

        Symbol* OwningSym = &open_bracket;
        if (cur == OwningSym->terminal) {
            CurrentLookingSym = &close_bracket;
            (*this.*functionarr[CurrentLookingSym->Sym])();
        }
        return;
    }

    void LF_close_bracket()
    {
        Symbol* OwningSym = &close_bracket;
        findBySymAction(
            OwningSym, &optional_char,
            []() { /*nothing*/ },
            [this]() { tmp.TitlePortion += cur; },
            true,
            &tmp.TitlePortion);
    }

    void LF_optional_char()
    {
        Symbol* OwningSym = &optional_char;
        if (OwningSym->terminal != 0) {
            cur = m_stream.get();
            if (cur == OwningSym->terminal) {
                tmp.isOptional = true;
                fmt::print("Marked optional!\n");
            } else if (cur == open_parenth.terminal) {
                m_stream.unget();
            }
        }
        CurrentLookingSym = &open_parenth;
        (*this.*functionarr[CurrentLookingSym->Sym])();
        return;
    }

    void LF_open_parenthesis()
    {
        Symbol* OwningSym = &open_parenth;
        findBySym(OwningSym, &close_parenth);
    }
    void LF_close_parenthesis()
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
            false,
            &tmp.LinkPortion);
        // Anyway we're going to reset, cuz' we're either not on good() anymore or are done
        tmp = MKLink {};
        return;
    }
    void EncounteredError() { }

    void (MarkdownLinkOptionalParser::*functionarr[OPS])() = {
        &MarkdownLinkOptionalParser::EncounteredError,
        &MarkdownLinkOptionalParser::Start,
        &MarkdownLinkOptionalParser::LF_open_bracket,
        &MarkdownLinkOptionalParser::LF_close_bracket,
        &MarkdownLinkOptionalParser::LF_optional_char,
        &MarkdownLinkOptionalParser::LF_open_parenthesis,
        &MarkdownLinkOptionalParser::LF_close_parenthesis,
        &MarkdownLinkOptionalParser::EndCurrentLink,
    };

public:
    MarkdownLinkOptionalParser(const std::string& inputstr)
    {
        m_stream.str(inputstr);
    }

    [[maybe_unused]] MarkdownLinkOptionalParser(std::string&& input)
    {
        m_stream.str(input);
    }

    [[maybe_unused]] void setOptional(char Target)
    {
        optional_char.terminal = Target;
    }

    void DebugRun()
    {
        std::string MKTest = "[Hello World](www.hello.world.de/Sagte/Ich) Nichts hier, tut mir leid. [Aber hier\\]](Haha), da war was escaped! \\o außerdem: [Optionaler]!(hier)";

        m_stream.str(MKTest);

        setOptional('!');
        fmt::print("LF: {}\n--------------------\n", MKTest);
        this->Start();
        for (auto& Element : this->Links) {
            fmt::print("\nCurrent MKLink Struct:\n\t * Titleportion: {}\n\t * Linportion: {}\n", Element.TitlePortion, Element.LinkPortion);
            if (Element.isOptional)
                fmt::print("\t * --> Marked Optional!\n");
        }
    }
};
}
/// \return
#endif //JSONTREE_MARKDOWNLINKOPTIONAL_HPP
