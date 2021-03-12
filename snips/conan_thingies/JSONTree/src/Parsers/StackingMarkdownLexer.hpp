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
    MD_ROOT,
    MD_NONE,
    MD_LINK,
    MD_INLINE_CODE,
    MD_BOLD,
    MD_ITALIC,
    MD_STRIKED,
    MD_HORIZONTAL_LINE,
    MD_QUOTE,
    MD_CODEBLOCK,
    MD_HEADING,
};

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
    StackingMarkdownLexer(std::string Input)
        : m_Scanner { Input }
        , m_scannervec { m_Scanner.getVec() }
    {
        if (!m_Scanner.hasScanned()) {
            m_Scanner.Scan();
            m_scannervec = m_Scanner.getVec();
        }

        SumUpSymbols();
    }

    ASTVec getVec() { return m_scannervec; }
    MarkdownScanner& getScanner() { return m_Scanner; }
};
}
#endif //JSONTREE_STACKINGMARKDOWNLEXER_HPP
