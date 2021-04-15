#include <cassert>
#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
enum OperatorsIdentifiers {
    Exponential,
    Multiplication,
    Division,
    Addition,
    Subtraction,
    Function,
    LeftParenthesis,
    RightParenthesis,
    Number
};

constexpr int PrecedenceTable[17] = {
    10, 10,
    8, 8,
    6, 6,
    4, 4,
    2,
    100, 100
};

enum Associative {
    Left,
    Right,
    None
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

struct Tokens {
    OperatorsIdentifiers id;
    Associative associative { None };
    std::string str {};
    long double Number { 0 };
    int Precedence { 0 };
    std::function<long double(long double, long double)> F {};
};

long double AdditionFunc(long double l, long double r)
{
    return l + r;
}

long double SubtractionFunc(long double l, long double r)
{
    return l - r;
}

long double MultiplicationFunc(long double l, long double r)
{
    return l * r;
}

long double DivisonFunc(long double l, long double r)
{
    return l / r;
}

std::shared_ptr<Tokens> LeftParenthSentinel = std::make_shared<Tokens>(
    Tokens {
        .id = LeftParenthesis, .associative = Right, .str = "(", .Number = 0, .Precedence = 100 });

std::shared_ptr<Tokens> RightParenthSentinel = std::make_shared<Tokens>(
    Tokens {
        .id = RightParenthesis, .associative = Left, .str = ")", .Number = 0, .Precedence = 100 });

std::shared_ptr<Tokens> ExponentialSentinel = std::make_shared<Tokens>(
    Tokens {
        .id = Exponential, .associative = Right, .str = "^", .Number = 0, .Precedence = 5 });

std::shared_ptr<Tokens> MultiplicationSentinel = std::make_shared<Tokens>(
    Tokens {
        .id = Multiplication, .associative = Left, .str = "*", .Number = 0, .Precedence = 4, .F = MultiplicationFunc });

std::shared_ptr<Tokens> DivisonSentinel = std::make_shared<Tokens>(
    Tokens {
        .id = Division, .associative = Left, .str = "/", .Number = 0, .Precedence = 4, .F = DivisonFunc });

std::shared_ptr<Tokens> SubtractionSentinel = std::make_shared<Tokens>(
    Tokens {
        .id = Subtraction, .associative = Left, .str = "-", .Number = 0, .Precedence = 2, .F = SubtractionFunc });

std::shared_ptr<Tokens> AdditionSentinel = std::make_shared<Tokens>(
    Tokens {
        .id = Addition, .associative = Left, .str = "+", .Number = 0, .Precedence = 2, .F = AdditionFunc });

std::shared_ptr<Tokens> SentinelMap[] = {
    ExponentialSentinel,
    MultiplicationSentinel,
    DivisonSentinel,
    AdditionSentinel,
    SubtractionSentinel
};

bool isValidOperator(const unsigned char c)
{
    return (OperatorTable[c] > 0);
}

std::shared_ptr<Tokens> getOperatorSentinel(unsigned char c)
{
    return SentinelMap[OperatorTable[c]];
}

class ShuntingYard {
private:
    std::stringstream m_stream;
    std::vector<std::shared_ptr<Tokens>> m_operators {};
    std::vector<std::shared_ptr<Tokens>> m_output {};

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
            } else {
                break;
            }

            advance();
            fmt::print("\t→ parseNum(): Processed {:c}\n", cur);
        }

        return std::stof(Number);
    }

public:
    ShuntingYard(const std::string& Str)
        : m_stream { Str }
    {
    }

    inline unsigned char get()
    {
        return m_stream.peek();
    }
    inline void advance()
    {
        m_stream.ignore();
    }

    void Parse()
    {

        auto VecTop = [&]() { return m_operators.back(); };
        while (m_stream.good()) {
            unsigned char cur = get();
            if (std::isspace(cur) || cur > 128) // ignoring any whitespace & irrelevant notation
            {
                advance();
                continue;
            }

            fmt::print("Processing: {:c}\n", cur);
            if (isdigit(cur)) // digit path
            {
                long double parsedNum = parseNum();
                fmt::print("Parsed digit {}\n", parsedNum);

                m_output.emplace_back(std::make_shared<Tokens>(
                    Tokens {
                        .id = Number,
                        .str = std::to_string(parsedNum),
                        .Number = parsedNum,
                        .Precedence = 0,
                    }));
                continue;
            }

            if (std::isalpha(cur)) // function path
            {
            }

            if (isValidOperator(cur)) // operator path
            {

                auto Operator = getOperatorSentinel(cur);
                fmt::print("Found valid Operator {}\n", Operator->str);

                while (!m_operators.empty()
                    && ((m_operators.back()->Precedence > Operator->Precedence)
                        || (m_operators.back()->Precedence == Operator->Precedence
                            && m_operators.back()->associative == Left))
                    && m_operators.back()->id != LeftParenthesis) {
                    m_output.emplace_back(m_operators.back());
                    m_operators.pop_back();
                    fmt::print("Popped back!\n");
                }
                m_operators.emplace_back(Operator);
                advance();
                continue;
            }

            if (cur == '(') // left parenth
            {
                m_operators.emplace_back(LeftParenthSentinel);
                continue;
            }

            if (cur == ')') // right parenth
            {
                bool foundParen { false };
                while (m_operators.back()->id != LeftParenthesis) {
                    m_output.emplace_back(m_operators.back());
                    m_operators.pop_back();
                    if (m_operators.empty())
                        break;
                    if (VecTop()->id == LeftParenthesis) {
                        foundParen = true;
                        break;
                    }
                }
            }

            fmt::print("Reached End with {:c}\n", cur);
            advance();
        }

        while (!m_operators.empty()) {
            m_output.emplace_back(m_operators.back());
            m_operators.pop_back();
        }
    }

    std::string
    toStr()
    {
        std::string str {};
        str.append("Output Queue: ");
        for (const auto& el : m_output) {
            str += el->str + " ";
        }
        str.append("\nOperator Stack: ");
        for (const auto& el : m_operators) {
            str += el->str + " ";
        }

        return str;
    }

    long double Calc()
    {
        long double Result { m_output.at(0)->Number };
        size_t i = 1;
        while (m_output.size() > i) {
            long double LeftExp = Result;
            long double RightExp = m_output.at(i++)->Number;
            Result = m_output.at(i++)->F(LeftExp, RightExp);
        }

        return Result;
    }
};

int main()
{
    std::string In = "1.1 + 1 + 1 - 3 ";
    ShuntingYard Thin { In };
    Thin.Parse();

    fmt::print("Input: '{}'\n{}\n", In, Thin.toStr());
    fmt::print("Calc(): {}\n", Thin.Calc());
}