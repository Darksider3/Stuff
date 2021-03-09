//
// Created by darksider3 on 27.02.21.
//

#ifndef JSONTREE_MARKDOWNLINKOPTIONAL_HPP
#define JSONTREE_MARKDOWNLINKOPTIONAL_HPP
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

    void HandlePossibleEscape(std::string* apTo = nullptr);
    void findBySym(Symbol* lf, Symbol* advanceToSym, std::string* appendTo = nullptr);
    void findBySymAction(Symbol* lf, Symbol* advanceTo, std::function<void(void)>&& success_fn, std::function<void(void)>&& failure_fn, bool runArr = true, std::string* appendTo = nullptr);
    void Start();
    void EndCurrentLink();
    [[maybe_unused]] void abortLink()
    {
        tmp = MKLink {};
    }
    void LF_open_bracket();
    void LF_close_bracket();
    void LF_optional_char();
    void LF_open_parenthesis();
    void LF_close_parenthesis();
    void EncounteredError();

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

    void DebugRun();
};
}
/// \return
#endif //JSONTREE_MARKDOWNLINKOPTIONAL_HPP
