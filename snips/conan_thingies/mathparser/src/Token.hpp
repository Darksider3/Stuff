//
// Created by darksider3 on 29.04.21.
//
#pragma once
#include "enum_tables.hpp"
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace MathParser::Tokens {

using NumberType = long double;
struct Token {
    OperatorsIdentifiers id;
    Associative associative { NotAssociative };
    NumberType Number { 0 };
    int Precedence { 0 };
    std::function<long double(long double, long double)> F {};
};

using SharedTokenPtr = std::shared_ptr<Tokens::Token>;
using RPNVec = std::vector<SharedTokenPtr>;

long double AdditionFunc(long double l, long double r);

long double SubtractionFunc(long double l, long double r);

long double MultiplicationFunc(long double l, long double r);

long double DivisonFunc(long double l, long double r);

long double ExponentionalFunc(long double l, long double r);
}