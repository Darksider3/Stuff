//
// Created by darksider3 on 09.03.21.
//

#ifndef JSONTREE_MARKDOWNLEXER_HPP
#define JSONTREE_MARKDOWNLEXER_HPP
#include "MarkdownScanner.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace JSONTree::Parsers::detail {
class MarkdownLexer {

    struct PositionalCounterparts {
        bool dec {};
        size_t counterpart {};
    };
    std::unique_ptr<PositionalCounterparts[]> m_counterparts {};

    enum MDObjectT {
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

    struct TextData {
        std::string_view Text {};
    };
    using TxtDataPtr = std::shared_ptr<TextData>;

    struct InlineCodeData : public TextData {
    };
    using InlineCodeDataPtr = std::shared_ptr<InlineCodeData>;

    struct BlockCodeData : public TextData {
        std::string_view language { "" };
        std::string_view contents { "" };
    };
    using BlockCodeDataPtr = std::shared_ptr<BlockCodeData>;

    struct BoldTextData : public TextData {
    };
    using BoldDataPtr = std::shared_ptr<BoldTextData>;

    struct ItalicTextData : public TextData {
    };
    using ItalicDataPtr = std::shared_ptr<ItalicTextData>;

    struct StrikedTextData : public TextData {
    };
    using StrikedDataPtr = std::shared_ptr<StrikedTextData>;

    struct HorizontalLineData : public NoneData {
    };
    using HorizontalLineDataPtr = std::shared_ptr<HorizontalLineData>;

    struct QuotedTextData : public TextData {
    };
    using QuotedDataPtr = std::shared_ptr<QuotedTextData>;

    struct LinkData;
    using LinkDataPtr = std::shared_ptr<LinkData>;

    using ObjectVariants = std::variant<
        RootDataPtr, NoneDataPtr,
        TxtDataPtr, InlineCodeDataPtr, BlockCodeDataPtr, ItalicDataPtr, BoldDataPtr, StrikedDataPtr,
        HorizontalLineDataPtr,
        QuotedDataPtr, LinkDataPtr>;

    struct LinkData {
        ObjectVariants Link;
        ObjectVariants Title;
    };

    struct MarkdownObject {
        MDObjectT ObjT { MD_NONE };
        ObjectVariants data;
        std::vector<MarkdownObject> childs {}; // :thinking:
    };

    ASTVec m_symvec {};
    MarkdownObject m_root { .ObjT = MD_ROOT };
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

    [[maybe_unused]] void SetCounterpartDecision(bool dec, size_t first, size_t second)
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
    MarkdownScanner m_Scanner;
    ASTVec getVec() { return m_symvec; }
    MarkdownLexer(const std::string& toParse)
        : m_startstr { toParse }
        , m_Scanner { toParse }
    {
    }

    void Stage1()
    {
        m_Scanner.Scan();
        m_symvec = m_Scanner.getVec();
        m_counterparts = std::make_unique<PositionalCounterparts[]>(m_symvec.size());
    }

    void Stage2()
    {
        SumUpSymbols();
    }

    // Links, inline blocks, inline bold and such
    void Stage3()
    {
    }

    // void handleSyms(MarkdownObject& root);
};
}
#endif //JSONTREE_MARKDOWNLEXER_HPP
