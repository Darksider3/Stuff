#include "RPNEvaluate.hpp"
#include "ShuntingYard.hpp"
#include <fmt/core.h>
#include <iostream>

int main()
{
    bool exit = false;
    MathParser::ShuntingYard Thin {};
    MathParser::Evaluate::RPNEvaluate Evaluator {};
    std::string In { "" };
    In.reserve(100);
    while (!exit) {
        fmt::print("> ");
        std::getline(std::cin, In);
        if (In == "e") {
            exit = true;
            continue;
        }

        Thin.setParsingTarget(In);
        Thin.Parse();

        Evaluator.setRPN(Thin.getRPNVec());
        fmt::print("{}\n", Evaluator.eval());
    }

    In.clear();
}