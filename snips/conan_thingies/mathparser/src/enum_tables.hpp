//
// Created by darksider3 on 29.04.21.
//

#pragma once
#include <cstdint>

namespace MathParser {
enum OperatorsIdentifiers {
    Exponential = 1,
    Multiplication = 2,
    Division = 3,
    Addition = 4,
    Subtraction = 5,
    Function = 6,
    LeftParenthesis = 7,
    RightParenthesis = 8,
    Number = 9
};

enum Associative {
    LeftAssociative,
    RightAssociative,
    NotAssociative
};

constexpr int8_t OperatorTable[128] = {
    //1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                      // 0-15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                      // 16-31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Multiplication, Addition, 0, Subtraction, 0, Division, // 32-47
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                      // 48-63
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                      // 64-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Exponential, 0,                            // 80-95
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                      // 96-111
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                      // 112-127
};

inline bool isValidOperator(const unsigned char c)
{
    return (OperatorTable[c] > 0);
}
}