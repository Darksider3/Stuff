//
// Created by darksider3 on 09.03.21.
//
#ifndef JSONTREE_MARKDOWNLINKOPTIONAL_CPP
#define JSONTREE_MARKDOWNLINKOPTIONAL_CPP
#include "MarkdownLinkOptional.hpp"

namespace JSONTree::Parsers {
void MarkdownLinkOptionalParser::HandlePossibleEscape(std::string* apTo)
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

void MarkdownLinkOptionalParser::findBySym(MarkdownLinkOptionalParser::Symbol* lf, MarkdownLinkOptionalParser::Symbol* advanceToSym, std::string* appendTo)
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

void MarkdownLinkOptionalParser::findBySymAction(MarkdownLinkOptionalParser::Symbol* lf, MarkdownLinkOptionalParser::Symbol* advanceTo, std::function<void(void)>&& success_fn, std::function<void(void)>&& failure_fn, bool runArr, std::string* appendTo)
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

void MarkdownLinkOptionalParser::Start()
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

void MarkdownLinkOptionalParser::EndCurrentLink()
{
    CurrentLookingSym = &start;
    return;
}

void MarkdownLinkOptionalParser::LF_open_bracket()
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

void MarkdownLinkOptionalParser::LF_close_bracket()
{
    Symbol* OwningSym = &close_bracket;
    findBySymAction(
        OwningSym, &optional_char,
        []() { /*nothing*/ },
        [this]() { tmp.TitlePortion += cur; },
        true,
        &tmp.TitlePortion);
}

void MarkdownLinkOptionalParser::LF_open_parenthesis()
{
    Symbol* OwningSym = &open_parenth;
    findBySym(OwningSym, &close_parenth);
}

void MarkdownLinkOptionalParser::LF_optional_char()
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

void MarkdownLinkOptionalParser::LF_close_parenthesis()
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

void MarkdownLinkOptionalParser::EncounteredError() { }

void MarkdownLinkOptionalParser::DebugRun()
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
}
#endif