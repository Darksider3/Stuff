//
// Created by darksider3 on 03.03.21.
//

#ifndef JSONTREE_MARKDOWNLEXER_HPP
#define JSONTREE_MARKDOWNLEXER_HPP
#include "common.hpp"
#include <memory>
#include <sstream>
#include <variant>
#include <vector>

namespace JSONTree::Parsers::detail {
// clang-format off
constexpr uint8_t TerminalTable[128] =  {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0 - 15
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 16 - 31
    0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0, // 32 - 47, 40=(, 41=) 42=*, 45=-
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 48 - 63
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 64 - 79
    0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1, // 80 - 95  91=[, 92=\, 93=], 94=^, 95=_
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 96 - 111 96=`
    0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0  // 112- 128 126=~
};


constexpr unsigned char line_feed = 0x0a;
constexpr unsigned char carriage_return = 0x0d;
constexpr unsigned char tab = 0x09;
constexpr unsigned char vertical_tab = 0x0b;
constexpr unsigned char space = 0x20;
constexpr unsigned char form_feed = 0x0c;
constexpr unsigned char dash = '-';
constexpr uint8_t DebugDashes = 40;

// clang-format on
/*
 * @TODO: All Scanning/Tokenizing into MarkdownScanner, Logics for the language itself into MarkdownLexer, display and manipulation into MarkdownParser
 *
 * The proposed Markdownlexer would take the vector of the scanner, turn it into an actual AST that is iterateable
 */
class MarkdownLexer {
    enum MarkSyms {
        SYM_ABSTRACT = -1,
        SYM_ASTERISK = 0,
        SYM_UNDERSCORE = 1,
        SYM_OPEN_PARENTHESIS = 2,
        SYM_CLOSE_PARENTHESIS = 3,
        SYM_OPEN_BRACKET = 4,
        SYM_CLOSE_BRACKET = 5,
        SYM_TILDE = 6,
        SYM_BACKTICK = 7,
        SYM_CIRCUMFLEX = 8,
        SYM_DASH = 9,
        SYM_JUST_NORMAL_TEXT = 10
    };

    using TerminalType = char;
    struct AbstractMarkSymbol;

    struct SymbolUserData {
        std::string userdata;
    };

#ifndef NDEBUG

    void fmtPrintFillChar()
    {
        fmt::print("\t{:->{}}\n", "", DebugDashes);
    }

    std::string escapedNewline(std::string_view input)
    {
        std::string escaped { "\"" };
        for (auto& c : input) {
            if (c == 0x0a) // \n
                escaped += "\\n";
            else
                escaped += c;
        }

        escaped += "\"";
        return escaped;
    }
#endif

    struct AbstractMarkSymbol {
        MarkSyms OP_SYM { SYM_ABSTRACT };
        std::string SymName { "ABSTRACT" };
        TerminalType Terminal { 0 };

        std::unique_ptr<SymbolUserData> data { std::make_unique<SymbolUserData>() };
    };

    struct SymbolObj {
    public:
        SymbolObj() = default;
        SymbolObj(SymbolObj&&) = default;
        ~SymbolObj() = default;

        // Delete Copy ops
        SymbolObj(const SymbolObj&) = delete;
        SymbolObj& operator=(const SymbolObj&) = delete;
        SymbolObj& operator=(SymbolObj&&) = default;

        std::unique_ptr<AbstractMarkSymbol> Symbol {}; //NOLINT
        int start_line { 0 };                          //NOLINT
        int stop_line { 0 };                           //NOLINT
        int start_position { 0 };                      //NOLINT
        int stop_position { 0 };                       //NOLINT
    };

    struct SymAsterisk : public AbstractMarkSymbol {
        SymAsterisk()
        {
            OP_SYM = MarkSyms::SYM_ASTERISK;
            SymName = "SYM_ASTERISK";
            Terminal = '*';
        }
    };

    struct SymUnderscore : public AbstractMarkSymbol {
        SymUnderscore()
        {
            OP_SYM = MarkSyms::SYM_UNDERSCORE;
            SymName = "SYM_UNDERSCORE";
            Terminal = '_';
        }
    };

    struct SymParenthesisOpen : public AbstractMarkSymbol {
        SymParenthesisOpen()
        {
            OP_SYM = MarkSyms::SYM_OPEN_PARENTHESIS;
            SymName = "SYM_OPEN_PARENTHESIS";
            Terminal = '(';
        }
    };

    struct SymParenthesisClose : public AbstractMarkSymbol {
        SymParenthesisClose()
        {
            OP_SYM = MarkSyms::SYM_CLOSE_PARENTHESIS;
            SymName = "SYM_CLOSE_PARENTHESIS";
            Terminal = ')';
        }
    };

    struct SymBracketOpen : public AbstractMarkSymbol {
        SymBracketOpen()
        {
            OP_SYM = MarkSyms::SYM_OPEN_BRACKET;
            SymName = "SYM_OPEN_BRACKET";
            Terminal = '[';
        }
    };

    struct SymBracketClose : public AbstractMarkSymbol {
        SymBracketClose()
        {
            OP_SYM = MarkSyms::SYM_CLOSE_BRACKET;
            SymName = "SYM_CLOSE_BRACKET";
            Terminal = ']';
        }
    };

    struct SymTilde : public AbstractMarkSymbol {
        SymTilde()
        {
            OP_SYM = MarkSyms::SYM_TILDE;
            SymName = "SYM_TILDE";
            Terminal = '~';
        }
    };

    struct SymDash : public AbstractMarkSymbol {
        SymDash()
        {
            OP_SYM = MarkSyms ::SYM_DASH,
            SymName = "SYM_DASH";
            Terminal = '-';
        }
    };

    struct SymBacktick : public AbstractMarkSymbol {
        SymBacktick()
        {
            OP_SYM = MarkSyms::SYM_BACKTICK;
            SymName = "SYM_BACKTICK";
            Terminal = '`';
        }
    };

    struct SymCircumflex : public AbstractMarkSymbol {
        SymCircumflex()
        {
            OP_SYM = MarkSyms::SYM_CIRCUMFLEX;
            SymName = "SYM_CIRCUMFLEX";
            Terminal = '`';
        }
    };

    struct SymJustText : public AbstractMarkSymbol {
        SymJustText()
        {
            OP_SYM = MarkSyms::SYM_JUST_NORMAL_TEXT;
            SymName = "SYM_TEXT";
            Terminal = 0;
        }
    };

    void PerfectlyFineText()
    {
        SymbolObj OwningText;
        OwningText.Symbol = std::make_unique<SymJustText>();
        OwningText.start_line = CUR_LINE;
        OwningText.start_position = CUR_COLOUM;
        OwningText.Symbol->data->userdata += m_cur;

        auto ConvertUTFNULLToReplacementCharacter = [&]() {
            OwningText.Symbol->data->userdata += "\xEF\xBF\xBD"; // UTF8 Replacement character. Wanted by CommonMark Spec
        };

        while (get_new_char()) {
            if (m_cur == 0x000000) {
                ConvertUTFNULLToReplacementCharacter();
            }

            if (!isTerminalChar(m_cur)) {
                OwningText.Symbol->data->userdata += m_cur;
            } else {
                putCharBackIntoStream();
                break;
            }
        }

        OwningText.stop_line = CUR_LINE;
        OwningText.stop_position = CUR_COLOUM;

        m_symvec.emplace_back(std::move(OwningText));
    }

    void fmtPrintDebugSym(SymbolObj& obj)
    {
        fmt::print("\n\tType: {0}\n\tName: {1:s}\n\tstartpos: {2}\n\tline: {3}\n\tTerminalchar: {4:c}\n",
            obj.Symbol->OP_SYM, obj.Symbol->SymName,
            obj.start_position, obj.start_line,
            obj.Symbol->Terminal);

        fmtPrintFillChar();
    }

    void fmtPrintDebugTextSym(SymbolObj& obj)
    {
        fmt::print("\n\tType: {0}\n\tName: {1:s}\n\tdataLength: {2}\n\tstartpos: {3}\n\tstoppos: {4}\n\tlines: {5}\n\tdata: {6:s}\n",
            obj.Symbol->OP_SYM, obj.Symbol->SymName, obj.Symbol->data->userdata.length(), obj.start_position, obj.stop_position,
            obj.stop_line - (obj.start_line - 1), escapedNewline(obj.Symbol->data->userdata)); // TODO: -1 because of 1-1, but maybe it's just more wise to call it linebreaks instead of lines

        fmtPrintFillChar();
    }

    template<typename T>
    void PushSymbolOnCurrent()
    {
        SymbolObj Owning;
        Owning.Symbol = std::make_unique<T>();
        Owning.start_line = CUR_LINE;
        Owning.start_position = CUR_COLOUM;

        m_symvec.emplace_back(std::move(Owning));
    }

    void OpenParenthesis()
    {
        PushSymbolOnCurrent<SymParenthesisOpen>();
    }

    void CloseParenthesis()
    {
        PushSymbolOnCurrent<SymParenthesisClose>();
    }

    void Asterisk()
    {
        PushSymbolOnCurrent<SymAsterisk>();
    }

    void OpenBracket()
    {
        PushSymbolOnCurrent<SymBracketOpen>();
    }

    void CloseBracket()
    {
        PushSymbolOnCurrent<SymBracketClose>();
    }

    void Escape()
    {
        if (m_stream.good())
            m_cur = m_stream.get(); // ignore the following char
    }

    void Circumflex()
    {
        PushSymbolOnCurrent<SymCircumflex>();
    }

    void Underscore()
    {
        PushSymbolOnCurrent<SymUnderscore>();
    }

    void Backtick()
    {
        PushSymbolOnCurrent<SymBacktick>();
    }

    void Tilde()
    {
        PushSymbolOnCurrent<SymTilde>();
    }

    void Dash()
    {
        PushSymbolOnCurrent<SymDash>();
    }

    void putCharBackIntoStream()
    {
        m_stream.unget();
    }

    // NOLINTNEXTLINE
    void (MarkdownLexer::*HandleTable[128])() = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                                             // NOLINT 0 - 15
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                                             // NOLINT 16 - 31
        0, 0, 0, 0, 0, 0, 0, 0, &MarkdownLexer::OpenParenthesis, &MarkdownLexer::CloseParenthesis, &MarkdownLexer::Asterisk, 0, 0, &MarkdownLexer::Dash, 0, 0,                      // NOLINT 32 - 47, 40=(, 41=) 42=*
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                                             // NOLINT 48 - 63
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                                             // NOLINT 64 - 79
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &MarkdownLexer::OpenBracket, &MarkdownLexer::Escape, &MarkdownLexer::CloseBracket, &MarkdownLexer::Circumflex, &MarkdownLexer::Underscore, // NOLINT 80 - 95  91=[, 92=\, 93=], 94=^, 95=_
        &MarkdownLexer::Backtick, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                      // NOLINT 96 - 111 96=`
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &MarkdownLexer::Tilde, 0, 0                                                                                                          // NOLINT 112- 128 126=~
    };
    // clang-tidy on

    std::stringstream m_stream;
    std::vector<SymbolObj> m_symvec {};

    uint32_t CUR_LINE = 1;
    uint32_t CUR_COLOUM = 1;

    unsigned char m_cur {};

    bool get_new_char()
    {
        auto ResetCharCounter = [this]() { CUR_COLOUM = 1; };
        auto AddToCharCounter = [this](int by = 1) { CUR_COLOUM += by; };
        auto AddNewLine = [this]() { CUR_LINE += 1; };

        while (m_stream.good()) {
            m_cur = m_stream.get();
            switch (m_cur) {
            case line_feed: // line feed, \n
                ResetCharCounter();
                AddNewLine();
                return m_stream.good();
            case carriage_return: // carriage return, \r
            case tab:             // tab  \ŧ
            case vertical_tab:    // vertical \v
            case space:           // space ' '
            case form_feed:       // form feed \f
            default:
                AddToCharCounter(1);
                return m_stream.good();
            }
        }

        return m_stream.good();
    }

public:
    void PrintTestWhitespaceThing()
    {
        std::string teststr { "Ich habe doch auch \n keine Ahnung! (oder so)" };
        m_stream.str(teststr);
        parse();

        for (auto& El : m_symvec) {
            if (El.Symbol->OP_SYM == SYM_JUST_NORMAL_TEXT) {
                // @TODO
                fmtPrintDebugTextSym(El);
            } else {
                fmtPrintDebugSym(El);
            }
        }

        fmt::print("Sourcestr: {}\n\n", escapedNewline(teststr));
    }

    bool isTerminalChar(unsigned char x)
    {
        return (TerminalTable[static_cast<unsigned int>(x)] == 1);
    }

    std::vector<SymbolObj> getVec()
    {
        return std::move(m_symvec);
    }

    void setVec(std::vector<SymbolObj>&& vec)
    {
        std::swap(vec, m_symvec);
    }
    void parse()
    {
        while (get_new_char()) {
            if (isTerminalChar(m_cur)) {
                // its a terminal char!
                (*this.*HandleTable[m_cur])();
            } else {
                // it's just text!
                this->PerfectlyFineText();
            }
        }
    }
};
}
#endif //JSONTREE_MARKDOWNLEXER_HPP
