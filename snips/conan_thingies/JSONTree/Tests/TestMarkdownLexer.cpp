//
// Created by darksider3 on 04.03.21.
//
#include "Parsers/MarkdownScanner.hpp"

using namespace JSONTree::Parsers::detail;

void fmtPrintFillChar()
{
    fmt::print("\t{:->{}}\n", "", DebugDashes);
}

std::string escapedNewline(std::string_view input)
{
    std::string escaped { "\"" };
    for (auto& c : input) {
        if (c == line_feed) // \n
            escaped += "\\n";
        else
            escaped += c;
    }

    escaped += "\"";
    return escaped;
}

void fmtPrintDebugSym(SymbolObj& obj)
{
    fmt::print("\n\tType: {0}\n\tName: {1:s}\n\tAbsolute Position: {2}\n\tline: {3}\n\tTerminalchar: {4:c}\n",
        obj.Symbol->OP_SYM, obj.Symbol->SymName,
        obj.absolut_position, obj.start_line,
        obj.Symbol->Terminal);

    fmtPrintFillChar();
}

void fmtPrintDebugTextSym(SymbolObj& obj)
{
    fmt::print("\n\tType: {0}\n\tName: {1:s}\n\tdataLength: {2}\n\tstartpos: {3}\n\tstoppos: {4}\n\tlines: {5}\n\tdata: {6:s}\n",
        obj.Symbol->OP_SYM, obj.Symbol->SymName, obj.Symbol->data->userdata.length(), obj.start_position, obj.stop_position,
        obj.stop_line - (obj.start_line - 1), escapedNewline(obj.Symbol->data->userdata)); // TODO: -1 because of 1-1, but maybe it's just more wise to call it linebreaks instead of lines

    fmtPrintFillChar();
}

void PrintTestWhitespaceThing()
{
    std::string teststr { "Ich habe doch auch -  \n keine Ahnung! [Brackets](oder so)" };
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

    fmt::print("Sourcestr: {}\n\n", escapedNewline(teststr));
}
int main(int, char**)
{
    //JSONTree::Parsers::detail::MarkdownScanner MDScanner { "sometest" };
    PrintTestWhitespaceThing();
}