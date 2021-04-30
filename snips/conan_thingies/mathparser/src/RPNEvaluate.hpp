//
// Created by darksider3 on 29.04.21.
//
#pragma once
#include <Token.hpp>
#include <cassert>
#include <iostream>
#include <stack>
#include <utility>

namespace MathParser::Evaluate {
using namespace Tokens;
class RPNEvaluate {
private:
    RPNVec m_rpn;
    std::stack<SharedTokenPtr> m_stack {};

public:
    RPNEvaluate(RPNVec Vec)
        : m_rpn(std::move(Vec))
    {
    }

    std::string eval();
    std::string eval(RPNVec&& vec)
    {
        m_rpn = std::move(vec);
        return eval();
    }
};
}