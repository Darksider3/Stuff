//
// Created by darksider3 on 03.03.21.
//

#ifndef JSONTREE_MARKDOWNSCANNER_HPP
#define JSONTREE_MARKDOWNSCANNER_HPP
#include "common.hpp"
#include <cassert>
#include <memory>
#include <sstream>
#include <variant>
#include <vector>

namespace JSONTree::Parsers::detail {
template<typename T>
using PointerT = std::shared_ptr<T>;

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
    SYM_NEWLINE = 10,
    SYM_TAB = 11,
    SYM_JUST_NORMAL_TEXT = 12
};

using TerminalType = char;
struct AbstractMarkSymbol;

struct SymbolUserData {
    size_t length { 0 };
};

struct AbstractMarkSymbol {
    MarkSyms OP_SYM { SYM_ABSTRACT };
    std::string SymName { "ABSTRACT" };
    TerminalType Terminal { 0 };

    PointerT<SymbolUserData> data { std::make_shared<SymbolUserData>() };
};

struct SymbolObj;
using ASTVec = std::vector<SymbolObj>;

struct SymbolObj {
public:
    SymbolObj() = default;
    SymbolObj(SymbolObj&&) = default;
    ~SymbolObj() = default;

    // Delete Copy ops
    SymbolObj(const SymbolObj&) = default;
    SymbolObj& operator=(const SymbolObj&) = default;
    SymbolObj& operator=(SymbolObj&&) = default;

    PointerT<AbstractMarkSymbol> Symbol {}; //NOLINT
    int OnLineNum { 0 };                    //NOLINT
    int StartColumn { 0 };                  //NOLINT
    int StopColoumn { 0 };                  //NOLINT
    int absolutePosition { 0 };             //NOLINT

    // automagically 1, because when we construct we expect it being actually present
    // why shouldn't we construct otherwise?
    int successiveCount { 1 }; //NOLINT
};

// clang-format off
constexpr uint8_t TerminalTable[128] =  {
    0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0, // 0 - 15 9=\t, 10=\n
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
class MarkdownScanner {
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

    struct SymNewLine : public AbstractMarkSymbol {
        SymNewLine()
        {
            OP_SYM = MarkSyms::SYM_NEWLINE;
            SymName = "SYM_NEWLINE";
            Terminal = '\n';
        }
    };

    struct SymTab : public AbstractMarkSymbol {
        SymTab()
        {
            OP_SYM = MarkSyms::SYM_TAB;
            SymName = "SYM_TAB";
            Terminal = '\t';
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
        auto SetupOwning = [&]() {
            OwningText = SymbolObj();
            OwningText.Symbol = std::make_shared<SymJustText>();
            OwningText.OnLineNum = CUR_LINE;
            OwningText.StartColumn = CUR_COLOUM;
            OwningText.absolutePosition = (m_stream.tellg());
            OwningText.Symbol->data->length = 0;
        };

        SetupOwning();
        while (get_new_char()) {
            if (!isTerminalChar(m_cur)) {
                OwningText.Symbol->data->length += 1;
            } else {
                putCharBackIntoStream();
                break;
            }
        }
        OwningText.StopColoumn = CUR_COLOUM;
        //--OwningText.absolutePosition; // Remember: we where already 2 char in!
        m_symvec.emplace_back(std::move(OwningText));
    }

    template<typename T>
    void PushSymbolOnCurrent()
    {
        SymbolObj Owning;
        Owning.Symbol = std::make_unique<T>();
        Owning.OnLineNum = CUR_LINE;
        Owning.StartColumn = CUR_COLOUM;
        Owning.absolutePosition = m_stream.tellg();

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

    void NewLine()
    {
        PushSymbolOnCurrent<SymNewLine>();
    }

    void Tab()
    {
        PushSymbolOnCurrent<SymTab>();
    }

    void putCharBackIntoStream()
    {
        m_stream.unget();
    }

    // NOLINTNEXTLINE
    void (MarkdownScanner::*HandleTable[128])() = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, &MarkdownScanner::Tab, &MarkdownScanner::NewLine, 0, 0, 0, 0, 0,                                                                                           // NOLINT 0 - 15
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                                                       // NOLINT 16 - 31
        0, 0, 0, 0, 0, 0, 0, 0, &MarkdownScanner::OpenParenthesis, &MarkdownScanner::CloseParenthesis, &MarkdownScanner::Asterisk, 0, 0, &MarkdownScanner::Dash, 0, 0,                        // NOLINT 32 - 47, 40=(, 41=) 42=*
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                                                       // NOLINT 48 - 63
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                                                       // NOLINT 64 - 79
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &MarkdownScanner::OpenBracket, &MarkdownScanner::Escape, &MarkdownScanner::CloseBracket, &MarkdownScanner::Circumflex, &MarkdownScanner::Underscore, // NOLINT 80 - 95  91=[, 92=\, 93=], 94=^, 95=_
        &MarkdownScanner::Backtick, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                                                              // NOLINT 96 - 111 96=`
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &MarkdownScanner::Tilde, 0, 0                                                                                                                  // NOLINT 112- 128 126=~
    };
    // clang-tidy on

    std::stringstream m_stream;
    ASTVec m_symvec {};

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

    bool m_done { false };

public:
    bool isTerminalChar(unsigned char x)
    {
        return (TerminalTable[static_cast<unsigned int>(x)] == 1);
    }

    std::vector<SymbolObj> getVec() const noexcept
    {
        return m_symvec;
    }

    std::vector<SymbolObj>& getVec() noexcept
    {
        return m_symvec;
    }

    void setVec(std::vector<SymbolObj>&& vec)
    {
        std::swap(vec, m_symvec);
    }

    void setInput(const std::string& in)
    {
        m_stream.str(in);
    }

    MarkdownScanner(const std::string& in)
    {
        setInput(in);
    }

    void Scan()
    {
        if (hasScanned())
            return;

        while (get_new_char()) {
            if (isTerminalChar(m_cur)) {
                // its a terminal char!
                (*this.*HandleTable[m_cur])();
            } else {
                // it's just text!
                this->PerfectlyFineText();
            }
        }

        m_done = true;
    }

    std::stringstream& getStream() { return m_stream; }

    static std::string getStrFromSym(SymbolObj& sym, MarkdownScanner& scanner)
    {
        auto& OperatingStream = scanner.getStream();
        OperatingStream.clear(); // clearing the eofbits
        std::string Composite { "" };
        auto OldPos = OperatingStream.tellg();
        auto OldFlags = OperatingStream.rdstate();

        OperatingStream.seekg(sym.absolutePosition - 1);
        for (size_t i = 0; i <= sym.Symbol->data->length; ++i) {
            char cur = OperatingStream.get();
            Composite += cur;
        }
        OperatingStream.seekg(OldPos);
        OperatingStream.clear(OldFlags); // Despite the fact this is called clear, it actually sets state flags. What a mess.
        return Composite;
    }

    inline bool hasScanned() const { return m_done; }
    inline void clearDoneFlag() { m_done = false; }
};
}
#endif //JSONTREE_MARKDOWNSCANNER_HPP
