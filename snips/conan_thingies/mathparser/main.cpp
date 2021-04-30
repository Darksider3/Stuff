#include "RPNEvaluate.hpp"
#include "ShuntingYard.hpp"
#include <fmt/core.h>
#include <iostream>

int main()
{
    bool exit = false;
    while (!exit) {
        std::string In { "" };
        In.reserve(100);
        fmt::print("> ");
        std::getline(std::cin, In);
        if (In == "e") {
            exit = true;
            continue;
        }
        MathParser::ShuntingYard Thin { In };
        Thin.Parse();
        MathParser::Evaluate::RPNEvaluate Evaluator { Thin.getRPNVec() };
        fmt::print("{}\n", Evaluator.eval());
    }
}