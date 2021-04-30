//
// Created by darksider3 on 29.04.21.
//
#pragma once

#include "Token.hpp"
#include <cmath>
#include <cstdint>
#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>

namespace MathParser {
using namespace Tokens;

class ShuntingYard {
private:
    std::stringstream m_stream;
    std::stack<SharedTokenPtr> m_operators {}; // this also could be a simple stack!
    Tokens::RPNVec m_output {};

    bool latestElementIsOperator()
    {
        return (m_operators.empty() || m_operators.top()->id != Number);
    }

    // @TODO: Allow negative numbers, e and such
    long double parseNum()
    {
        std::string Number { "" };
        bool Dot = false;
        while (m_stream.good()) {
            unsigned char cur = get();
            if (std::isdigit(cur)) {
                Number += cur;
            } else if (!Dot && cur == '.') {
                Dot = true;
                Number += cur;
            }
            // it is possibly a number with sign, IF:
            // * the last operator on the operator stack is an actual operator, not a number, AND
            // * it's sign is recognizable AND
            // * it's on the beginning of said number
            else {
                break;
            }

            advance();
            //fmt::print("\t→ parseNum(): Processed {:c}\n", cur);
        }

        return std::stof(Number);
    }

    inline unsigned char get()
    {
        return m_stream.peek();
    }

    inline void advance()
    {
        m_stream.ignore();
    }

    inline std::shared_ptr<Token> MakeNumberToken(long double Num)
    {
        return std::make_shared<Token>(
            Token { .id = Number,
                .str = std::to_string(Num),
                .Number = Num,
                .Precedence = 0 });
    }

public:
    ShuntingYard(const std::string& Str)
        : m_stream { Str }
    {
    }

    void Parse();

    RPNVec getRPNVec() const { return m_output; }
};
}
