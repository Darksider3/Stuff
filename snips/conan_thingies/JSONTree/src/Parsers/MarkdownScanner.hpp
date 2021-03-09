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
    std::string userdata;
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
    int absolut_position { 0 };             //NOLINT

    // automagically 1, because when we construct we expect it being actually present
    // why shouldn't we construct otherwise?
    int successive_count { 1 }; //NOLINT
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
            OwningText.Symbol->data->userdata += m_cur;
            OwningText.absolut_position = m_stream.tellg();
        };

        auto ConvertUTFNULLToReplacementCharacter
            = [&]() {
                  OwningText.Symbol->data->userdata += "\xEF\xBF\xBD"; // UTF8 Replacement character. Wanted by CommonMark Spec
              };

        SetupOwning();
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
        OwningText.StopColoumn = CUR_COLOUM;

        m_symvec.emplace_back(std::move(OwningText));
    }

    template<typename T>
    void PushSymbolOnCurrent()
    {
        SymbolObj Owning;
        Owning.Symbol = std::make_unique<T>();
        Owning.OnLineNum = CUR_LINE;
        Owning.StartColumn = CUR_COLOUM;
        Owning.absolut_position = m_stream.tellg();

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

public:
    bool isTerminalChar(unsigned char x)
    {
        return (TerminalTable[static_cast<unsigned int>(x)] == 1);
    }

    std::vector<SymbolObj> getVec() noexcept
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

class MarkdownLexer {

    struct PositionalCounterparts {
        bool dec {};
        size_t counterpart {};
    };
    std::unique_ptr<PositionalCounterparts[]> m_counterparts {};

    enum MDObjectT {
        MD_NONE,
        MD_LINK,
        MD_INLINE_CODE,
        MD_BOLD,
        MD_ITALIC,
        MD_STRIKED,
        MD_QUOTE,
        MD_CODEBLOCK,
        MD_HEADING,
    };

    struct TextData {
        std::string Text {};
    };

    struct LinkData {
        std::string Link;
        std::string Title;
    };

    struct InlineCodeData : public TextData {
    };

    struct BoldTextData : public TextData {
    };

    struct ItalicTextData : public TextData {
    };

    struct StrikedTextData : public TextData {
    };

    struct QuotedTextData : public TextData {
    };

    using TxtDataPtr = std::shared_ptr<TextData>;
    using InlineCodeDataPtr = std::shared_ptr<InlineCodeData>;
    using ItalicDataPtr = std::shared_ptr<ItalicTextData>;
    using BoldDataPtr = std::shared_ptr<BoldTextData>;
    using StrikedDataPtr = std::shared_ptr<StrikedTextData>;
    using QuotedDataPtr = std::shared_ptr<QuotedTextData>;

    struct MarkdownObject {
        MDObjectT ObjT { MD_NONE };
        std::variant<TxtDataPtr, InlineCodeDataPtr, ItalicDataPtr, BoldDataPtr, StrikedDataPtr, QuotedDataPtr> data;
    };

    ASTVec m_symvec {};
    std::vector<MarkdownObject> MDObjects {};
    const std::string& m_startstr;

    void SumUpSymbols()
    {
        for (ASTVec::size_type i = 0; i != m_symvec.size(); ++i) {
            if (m_symvec[i].Symbol->OP_SYM == m_symvec[i + 1].Symbol->OP_SYM) {
                ASTVec::size_type j = 1;
                while (i + j != m_symvec.size()
                    && m_symvec.at(i).Symbol->OP_SYM == m_symvec.at(i + j).Symbol->OP_SYM) {

                    m_symvec[i].successive_count += 1;
                    m_symvec.erase(m_symvec.begin() + i + j); // start + current + 1(next)
                    ++j;                                      // step forward
                    --i;                                      // but rather dont(really)

                    // Thats because on erause, lets say i=5, i must be i=4 when size()<6. Otherwise we will overrun!
                }
                //fmt::print("DBG: was here");
            }
        }
    }

    struct CounterpartDecision {
        bool found { false };
        long VecDistance { 0 };
    };

    CounterpartDecision SymHasCounterpart(const SymbolObj& obj, ASTVec::size_type FromIndex)
    {
        auto ItHasObj = std::find_if(m_symvec.begin() + FromIndex, m_symvec.end(),
            [obj](const SymbolObj& other) -> bool {
                if (obj.Symbol->OP_SYM == other.Symbol->OP_SYM) {
                    if (obj.successive_count == other.successive_count) {
                        return true;
                    }
                }

                return false;
            });

        CounterpartDecision FinalDecision = {
            .found = std::end(m_symvec) != ItHasObj,
            .VecDistance = std::distance(m_symvec.begin(), ItHasObj),
        };

        return FinalDecision;
    }

    void SetCounterpartDecision(bool dec, size_t first, size_t second)
    {
        assert(!m_symvec.empty());
        assert(m_counterparts);

        m_counterparts[first].counterpart = second;
        m_counterparts[first].dec = dec;
        m_counterparts[second].counterpart = first;
        m_counterparts[second].dec = dec;
    }

    void SetCounterpartDecision(size_t first, const CounterpartDecision& Decision)
    {
        m_counterparts[first].dec = Decision.found;
        m_counterparts[first].counterpart = Decision.VecDistance;
        m_counterparts[Decision.VecDistance].dec = Decision.found;
        m_counterparts[Decision.VecDistance].counterpart = Decision.VecDistance;
    }

public:
    MarkdownLexer(const std::string& toParse)
        : m_startstr { toParse }
    {
    }

    void Stage1()
    {
        MarkdownScanner Scanner { m_startstr };
        Scanner.Scan();
        m_symvec = Scanner.getVec();
        m_counterparts = std::make_unique<PositionalCounterparts[]>(m_symvec.size());
    }

    void Stage2()
    {
        SumUpSymbols();
    }

    // Links, inline blocks, inline bold and such
    void Stage3()
    {
        for (ASTVec::size_type i = 0; i != m_symvec.size(); ++i) {
            auto& CurrentNode = m_symvec.at(i);

            switch (CurrentNode.Symbol->OP_SYM) {
            case MarkSyms::SYM_BACKTICK: {
                if (!m_counterparts[i].dec) {
                    auto Decision = SymHasCounterpart(CurrentNode, i + 1);
                    if (Decision.found) {
                        fmt::print("Found counterpart, source: {0}, counterpart: {1}\n", CurrentNode.absolut_position, m_symvec[Decision.VecDistance].absolut_position);
                        SetCounterpartDecision(i, Decision);
                    } else {
                        fmt::print("Didnt find needed counterparts! :(\n");
                        SetCounterpartDecision(i, Decision);
                    }
                } else {
                    fmt::print("Ignoring counterpartsearch because we already had that, lol\n");
                }
            }
            default:
                break;
            }
        }
    }

    ASTVec getVec() { return m_symvec; }
};
}
#endif //JSONTREE_MARKDOWNSCANNER_HPP
