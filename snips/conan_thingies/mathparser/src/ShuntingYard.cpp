//
// Created by darksider3 on 29.04.21.
//

#include "ShuntingYard.hpp"
#include "MathSentinels.hpp"

void MathParser::ShuntingYard::Parse()
{

    auto VecTop = [&]() { return m_operators.top(); };
    while (m_stream.good()) {
        unsigned char cur = get();
        if (std::isspace(cur)) // ignoring any whitespace & irrelevant notation
        {
            advance();
            //fmt::print("Next char: {:c}\n", get());
            continue;
        }

        //fmt::print("Processing: {:c}\n", cur);
        if (isdigit(cur)) // digit path
        {
            long double parsedNum = parseNum();
            //fmt::print("Parsed digit {}\n", parsedNum);

            m_output.emplace_back(MakeNumberToken(parsedNum));
            continue;
        }

        if (std::isalpha(cur)) // function path
        {
        }

        if (isValidOperator(cur)) // operator path
        {
            auto Operator = getOperatorSentinel(cur);
            //fmt::print("Found valid Operator {}\n", Operator->str);

            while (!m_operators.empty()                                        // as long as we got tokens
                && ((m_operators.top()->Precedence > Operator->Precedence)     //((the operator at the top of the operator stack has greater precedence)
                    || (m_operators.top()->Precedence == Operator->Precedence  // (OR equal
                        && m_operators.top()->associative == LeftAssociative)) // AND Left Associative)
                && m_operators.top()->id != LeftParenthesis /* and NOT a  Left-Parenthesis */) {
                m_output.emplace_back(m_operators.top());
                m_operators.pop(); // pop the operators onto the output queue
            }
            m_operators.emplace(Operator);
            advance();
            continue;
        }

        if (cur == '(') // left parenth
        {
            m_operators.emplace(SentinelLeftParenthesis);
            advance();
            continue;
        }

        if (cur == ')') // right parenth
        {
            while (m_operators.top()->id != LeftParenthesis) {
                m_output.emplace_back(m_operators.top());
                m_operators.pop();
                if (m_operators.empty())
                    break;
                if (VecTop()->id == LeftParenthesis) {
                    break;
                }
            }

            if (m_operators.top()->id == LeftParenthesis) {
                m_operators.pop(); // discard
            } else {
                throw std::exception();
            }
            if (m_operators.top()->id == Function) {
                m_operators.pop();
            }
            advance();
            continue;
        }

        //fmt::print("Reached End with {:c}\n", cur);
        advance();
    }

    while (!m_operators.empty()) {
        m_output.emplace_back(m_operators.top());
        m_operators.pop();
    }
}