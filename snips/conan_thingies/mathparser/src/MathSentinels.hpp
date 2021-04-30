//
// Created by darksider3 on 29.04.21.
//

#pragma once
#include "Token.hpp"
namespace MathParser::Tokens {

std::shared_ptr<Token> SentinelLeftParenthesis = std::make_shared<Token>(
    Token {
        .id = LeftParenthesis, .associative = RightAssociative, .str = "(", .Number = 0, .Precedence = 100 });

std::shared_ptr<Token> SentinelRightParenthesis = std::make_shared<Token>(
    Token {
        .id = RightParenthesis, .associative = LeftAssociative, .str = ")", .Number = 0, .Precedence = 100 });

std::shared_ptr<Token> SentinelExponential = std::make_shared<Token>(
    Token {
        .id = Exponential, .associative = RightAssociative, .str = "^", .Number = 0, .Precedence = 5, .F = ExponentionalFunc });

std::shared_ptr<Token> SentinelMultiplication = std::make_shared<Token>(
    Token {
        .id = Multiplication, .associative = LeftAssociative, .str = "*", .Number = 0, .Precedence = 4, .F = MultiplicationFunc });

std::shared_ptr<Token> SentinelDivison = std::make_shared<Token>(
    Token {
        .id = Division, .associative = LeftAssociative, .str = "/", .Number = 0, .Precedence = 4, .F = DivisonFunc });

std::shared_ptr<Token> SentinelSubstraction = std::make_shared<Token>(
    Token {
        .id = Subtraction, .associative = LeftAssociative, .str = "-", .Number = 0, .Precedence = 2, .F = SubtractionFunc });

std::shared_ptr<Token> SentinelAddition = std::make_shared<Token>(
    Token {
        .id = Addition, .associative = LeftAssociative, .str = "+", .Number = 0, .Precedence = 2, .F = AdditionFunc });

std::shared_ptr<Token> SentinelMap[] = {
    nullptr,
    SentinelExponential,    // 1
    SentinelMultiplication, // 2
    SentinelDivison,        // 3
    SentinelAddition,       // 4
    SentinelSubstraction    // 5
};

std::shared_ptr<Token> getOperatorSentinel(unsigned char c)
{
    return SentinelMap[OperatorTable[c]];
}
}