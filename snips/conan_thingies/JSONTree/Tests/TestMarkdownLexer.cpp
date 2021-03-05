//
// Created by darksider3 on 04.03.21.
//
#include "Parsers/MarkdownScanner.hpp"

int main(int, char**)
{
    JSONTree::Parsers::detail::MarkdownScanner Lexer {};
    Lexer.PrintTestWhitespaceThing();
}