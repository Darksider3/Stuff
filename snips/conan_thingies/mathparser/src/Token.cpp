//
// Created by darksider3 on 29.04.21.
//
#include "Token.hpp"

long double MathParser::Tokens::AdditionFunc(long double l, long double r)
{
    return l + r;
}
long double MathParser::Tokens::SubtractionFunc(long double l, long double r)
{
    return l - r;
}
long double MathParser::Tokens::MultiplicationFunc(long double l, long double r)
{
    return l * r;
}
long double MathParser::Tokens::DivisonFunc(long double l, long double r)
{
    return l / r;
}
long double MathParser::Tokens::ExponentionalFunc(long double l, long double r)
{
    return powl(l, r);
}
