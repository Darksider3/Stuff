//
// Created by darksider3 on 12.03.21.
//

#ifndef JSONTREE_STACKINGMARKDOWNLEXER_HPP
#define JSONTREE_STACKINGMARKDOWNLEXER_HPP
#include "Parsers/MarkdownScanner.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace JSONTree::Parsers::detail {
enum MDType {
    MD_ROOT = 1,
    MD_NONE = 2,
    MD_LINK = 4,
    MD_CLOSE_LINK = 8,
    MD_INLINE_CODE = 16,
    MD_CLOSE_INLINE_CODE = 32,
    MD_BOLD = 64,
    MD_END_BOLD = 128,
    MD_ITALIC = 256,
    MD_END_ITALIC = 512,
    MD_STRIKED = 1024,
    MD_END_STRIKED = 2048,
    MD_HORIZONTAL_LINE = 4096,
    MD_QUOTE = 8192,
    MD_END_QUOTE = 16384,
    MD_CODEBLOCK = 32768,
    MD_END_CODEBLOCK = 65536,
    MD_HEADING = 131072,
};

inline MDType operator|(MDType a, MDType b) {
    return static_cast<MDType>(static_cast<int>(a) | static_cast<int>(b));
}

struct NoneData {
};
struct RootData : NoneData {
};
using RootDataPtr = std::shared_ptr<RootData>;
using NoneDataPtr = std::shared_ptr<NoneData>;

struct TextualPropertyData {
    std::string_view TextWithProperty {};
};

struct BlockCodeData {
    std::string_view language { "" };
    std::string_view contents { "" };
};
struct HorizontalLineData : public NoneData {
};

using TxtDataPtr = std::shared_ptr<TextualPropertyData>;
using BlockCodeDataPtr = std::shared_ptr<BlockCodeData>;
using HorizontalLineDataPtr = std::shared_ptr<HorizontalLineData>;

struct LinkData;
using LinkDataPtr = std::shared_ptr<LinkData>;

using ObjectVariants = std::variant<
    RootDataPtr, NoneDataPtr,
    TxtDataPtr, BlockCodeDataPtr,
    HorizontalLineDataPtr,
    LinkDataPtr>;

struct LinkData {
    ObjectVariants Link;
    ObjectVariants Title;
};

struct MarkdownObject {
    MDType ObjT { MD_NONE };
    ObjectVariants data;
    std::vector<MarkdownObject> childs {}; // :thinking:
};

class StackingMarkdownLexer {
private:
    MarkdownScanner m_Scanner;
    ASTVec& m_scannervec;
    MarkdownObject m_root {};
    std::vector<MDType> m_open_tags;

    void SumUpSymbols()
    {
        for (ASTVec::size_type i = 0; i != m_scannervec.size(); ++i) {
            MarkSyms& LF_Sym = m_scannervec[i].Symbol->OP_SYM;
            if (m_scannervec.size() > i + 1 && LF_Sym == m_scannervec[i + 1].Symbol->OP_SYM) {
                while (m_scannervec.size() > i + 1 && LF_Sym == m_scannervec[i + 1].Symbol->OP_SYM) {
                    m_scannervec.erase(m_scannervec.begin() + i + 1);
                    i -= 1;
                    m_scannervec[i].successiveCount += 1;
                }
            }
        }
    }

public:

    bool hasOpen(const MDType &t) const {
        auto it = std::find_if(m_open_tags.begin(), m_open_tags.end(), [t](const MDType cur) -> bool{
            return (t == cur);
        });
        return it != m_open_tags.end();
    }

    StackingMarkdownLexer(std::string Input)
        : m_Scanner { Input }
        , m_scannervec { m_Scanner.getVec() }
    {
        if (!m_Scanner.hasScanned()) {
            m_Scanner.Scan();
            m_scannervec = m_Scanner.getVec();
        }

        m_root.ObjT = MD_ROOT;
        SumUpSymbols();
    }

    void Parse()
    {
        auto &CurrentTypeObj = m_root;
        for(size_t i = 0; i < m_scannervec.size(); ++i)
        {
            auto& cur = m_scannervec[i];
            switch(cur.Symbol->OP_SYM) {
            case SYM_ASTERISK:
            case SYM_UNDERSCORE: {
                MDType operatingSym;
                if(cur.successiveCount % 3 == 0)
                    operatingSym = MD_ITALIC | MD_BOLD;
                else if(cur.successiveCount == 2)
                    operatingSym = MD_BOLD;
                else if(cur.successiveCount == 1)
                    operatingSym = MD_ITALIC;

                if(!hasOpen(operatingSym)) // not open
                {
                    MarkdownObject insertable;

                    insertable.ObjT = operatingSym;
                    CurrentTypeObj.childs.emplace_back(insertable);
                } else {

                }
            }
                break;
            case SYM_OPEN_PARENTHESIS:
                break;
            case SYM_CLOSE_PARENTHESIS:
                break;
            case SYM_OPEN_BRACKET:
                break;
            case SYM_CLOSE_BRACKET:
                break;
            case SYM_TILDE:
                break;
            case SYM_BACKTICK:
                break;
            case SYM_CIRCUMFLEX:
                break;
            case SYM_DASH:
                break;
            case SYM_NEWLINE:
                break;
            case SYM_TAB:
                break;
            case SYM_JUST_NORMAL_TEXT:
                // data here
                break;
            case SYM_ABSTRACT:
            default:
                break;
            }
        }
    }

    ASTVec getVec() { return m_scannervec; }
    MarkdownScanner& getScanner() { return m_Scanner; }
};
}
#endif //JSONTREE_STACKINGMARKDOWNLEXER_HPP
