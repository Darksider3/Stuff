//
// Created by darksider3 on 04.03.21.
//
#include "Parsers/MarkdownScanner.hpp"

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

void fmtPrintDebugSym(SymbolObj& obj)
{
    fmt::print("Type: {0}\n\tName: {1:s}\n\tAbsolute Position: {2}\n\tline: {3}\n\tTerminalchar: {4:s}\n",
        obj.Symbol->OP_SYM, obj.Symbol->SymName,
        obj.absolut_position, obj.OnLineNum,
        escapedControlChars(obj.Symbol->Terminal));

    fmtPrintFillChar();
}

void fmtPrintDebugTextSym(SymbolObj& obj)
{
    fmt::print("Type: {0}\n\t-> Name: {1:s}\n\t-> dataLength: {2}\n\t-> startpos: {3}\n\t-> stoppos: {4}\n\t-> lines: {5}\n\t-> data: {6:s}\n",
        obj.Symbol->OP_SYM, obj.Symbol->SymName, obj.Symbol->data->userdata.length(), obj.StartColumn, obj.StopColoumn,
        obj.OnLineNum, escapedControlChars(obj.Symbol->data->userdata)); // TODO: -1 because of 1-1, but maybe it's just more wise to call it linebreaks instead of lines

    fmtPrintFillChar();
}

void PrintTestWhitespaceThing()
{
    std::string teststr { "Ich habe doch auch -  \n keine Ahnung! [Brackets](oder so)```c\nhier()\n```" };
    MarkdownScanner MDScanner { teststr };
    MDScanner.Scan();
    auto SymVec = MDScanner.getVec();

    for (auto& El : SymVec) {
        if (El.Symbol->OP_SYM == SYM_JUST_NORMAL_TEXT) {
            // @TODO
            fmtPrintDebugTextSym(El);
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