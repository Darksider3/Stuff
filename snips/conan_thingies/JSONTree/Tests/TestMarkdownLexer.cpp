//
// Created by darksider3 on 04.03.21.
//
#include "Parsers/MarkdownScanner.hpp"
#include "Parsers/StackingMarkdownLexer.hpp"

using namespace JSONTree::Parsers::detail;

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
        obj.absolutePosition, obj.successive_count);
}

void PrintTestWhitespaceThing()
{
    std::string teststr { "`SingleTick?` Ich habe ___doch___ auch *nix* -  \n keine Ahnung! [Brackets](oder so)\n\t hi \n```c\nhier()\n```" };
    StackingMarkdownLexer MDLexer { teststr };
    auto SymVec = MDLexer.getVec();

    for (auto& El : SymVec) {
        if (El.Symbol->OP_SYM == SYM_JUST_NORMAL_TEXT) {
            // @TODO
            fmtPrintDebugTextSym(El, MDLexer.getScanner());
        } else {
            fmtPrintDebugSym(El);
        }
    }

    fmt::print("Sourcestr: {}\n\n", escapedControlChars(teststr));
}
int main(int, char**)
{
    //JSONTree::Parsers::detail::MarkdownScanner MDScanner { "sometest" };
    PrintTestWhitespaceThing();
}