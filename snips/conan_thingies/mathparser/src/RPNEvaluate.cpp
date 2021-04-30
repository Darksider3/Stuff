//
// Created by darksider3 on 29.04.21.
//

#include "RPNEvaluate.hpp"
std::string MathParser::Evaluate::RPNEvaluate::eval()
{
    NumberType EvaluationResult { 0 };
    for (const auto& El : m_rpn) {
        if (El->id < Number) {
            SharedTokenPtr right = m_stack.top();
            assert(right->id == Number);
            m_stack.pop();
            SharedTokenPtr left = m_stack.top();
            assert(left->id == Number);
            m_stack.pop();
            EvaluationResult = El->F(left->Number, right->Number);
            right->Number = EvaluationResult;
            right->str = std::to_string(EvaluationResult);
            m_stack.push(right);
        } else if (El->id == Number) {
            m_stack.emplace(El);
        }
    }

    return std::to_string(EvaluationResult);
}
