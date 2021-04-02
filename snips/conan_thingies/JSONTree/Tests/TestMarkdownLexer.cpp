//
// Created by darksider3 on 04.03.21.
//
#include <doctest/doctest.h>

#include "Parsers/MarkdownScanner.hpp"
#include "Parsers/StackingMarkdownLexer.hpp"

using namespace JSONTree::Parsers::detail;
namespace Test::Tooling {
void fmtPrintFillChar()
{
    fmt::print("\t{:->{}}\n", "", DebugDashes);
}

std::string escapedControlChars(std::string_view input)
{
    std::string escaped { "\"" };
    for (auto& c : input) {
        if (c == line_feed) // \n
            escaped += "\\n";
        else if (c == tab)
            escaped += "\\t";
        else
            escaped += c;
    }

    escaped += "\"";
    return escaped;
}

std::string escapedControlChars(char input)
{
    std::string in;
    in += input;
    return escapedControlChars(in);
}

void fmtPrintDebugSymVerbose(SymbolObj& obj)
{
    fmt::print("Type: {0}\n\tName: {1:s}\n\t"
               "Absolute Position: {2}\n\tline: {3}\n\t"
               "Terminalchar: {4:s}\n",
        obj.Symbol->OP_SYM, obj.Symbol->SymName,
        obj.absolutePosition, obj.OnLineNum,
        escapedControlChars(obj.Symbol->Terminal));

    fmtPrintFillChar();
}

void fmtPrintDebugTextSymVerbose(SymbolObj& obj, MarkdownScanner& scanner)
{
    std::string data = scanner.getStrFromSym(obj, scanner);
    fmt::print("Type: {0}\n\t-> Name: {1:s}\n"
               "\t-> dataLength: {2}\n\t-> startpos: {3}\n"
               "\t-> stoppos: {4}\n\t-> lines: {5}\n"
               "\t-> data: {6:s}\n",
        obj.Symbol->OP_SYM, obj.Symbol->SymName,
        data.length(),
        obj.StartColumn, obj.StopColoumn,
        obj.OnLineNum, escapedControlChars(data));

    fmtPrintFillChar();
}

void fmtPrintDebugTextSym(SymbolObj& obj, MarkdownScanner& scanner)
{
    std::string data = scanner.getStrFromSym(obj, scanner);
    fmt::print("| Type{0}| Name: {1: <22s}| len: {2}| Pos: {3}| Data: {4:s}|\n",
        obj.Symbol->OP_SYM, obj.Symbol->SymName,
        data.length(),
        obj.absolutePosition, escapedControlChars(data));
}

void fmtPrintDebugSym(SymbolObj& obj)
{
    fmt::print("| Type{0: <2}| Name: {1: <22s}| Pos: {2}| Count: {3}|\n",
        obj.Symbol->OP_SYM, obj.Symbol->SymName,
        obj.absolutePosition, obj.successiveCount);
}

}

TEST_SUITE("Basics")
{
    size_t char_counter(const std::string& in, char ch)
    {
        size_t c { 0 };
        for (const auto& el : in) {
            if (el == ch)
                ++c;
        }

        return c;
    }

    size_t successiveSymCount(const std::vector<SymbolObj>& SymVec, MarkSyms Sym)
    {
        size_t counter { 0 };
        for (const auto& el : SymVec) {
            if (el.Symbol->OP_SYM == Sym) {
                counter += el.successiveCount;
            }
        }

        return counter;
    }

    std::string teststr { "`SingleTick?` Ich habe ___doch___ auch *nix* -  \n keine Ahnung! [Brackets](oder so)\n\t hi \n```c\nhier()\n```" };

    size_t known_backticks { char_counter(teststr, '`') };
    size_t known_underscores { char_counter(teststr, '_') };

    StackingMarkdownLexer MDLexer { teststr };
    auto SymVec = MDLexer.getVec();

    TEST_CASE("Just Run-Through while printing")
    {
        for (auto& El : SymVec) {
            if (El.Symbol->OP_SYM == SYM_JUST_NORMAL_TEXT) {
                Test::Tooling::fmtPrintDebugTextSym(El, MDLexer.getScanner());
            } else {
                Test::Tooling::fmtPrintDebugSym(El);
            }
        }

        fmt::print("Sourcestr: {}\n\n", Test::Tooling::escapedControlChars(teststr));
    }

    TEST_CASE("Only one(!) abstract symbol")
    {
        CHECK_LE(
            std::count_if(SymVec.begin(), SymVec.end(), [](SymbolObj& sym) -> bool {
                return (sym.Symbol->OP_SYM == SYM_ABSTRACT);
            }),
            2);
    }

    TEST_CASE("Backtick Counter")
    {
        size_t counter = successiveSymCount(SymVec, SYM_BACKTICK);

        CHECK_EQ(counter, known_backticks);
    }

    TEST_CASE("Underscore Counter")
    {
        size_t counter = successiveSymCount(SymVec, SYM_UNDERSCORE);

        CHECK_EQ(counter, known_underscores);
    }
}