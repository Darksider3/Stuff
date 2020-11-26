#ifndef DIAGNOSIS_H
#define DIAGNOSIS_H
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Li {
namespace Reporting {

enum Reporting_Level {
    Warn,
    Err,
    Inf,
    Rec,
};

std::string Reporting_Level_Str[] = {
    "WARNING",
    "ERROR",
    "INFO",
    "RECOMMENDATION"
};

extern "C" {
struct Report {
    Reporting_Level Level {};
    std::string Msg {};
};
}
template<typename T, typename LevelT>
class Diagnosis {
public:
    Diagnosis() = default;
    explicit Diagnosis(LevelT L)
        : printBarrier { L }
    {
    }

    void addReport(LevelT, std::string_view);

    void PrintReports() const;

    bool has(LevelT l);

    void setReportingBarrier(LevelT min);

    ~Diagnosis()
    {
#if !defined(NDEBUG) || !defined(LI_RUNTIME_DIAGNOSIS)
        if (Reports.empty())
            return;
        std::cerr << "\n\n\t===================\n";
        std::cerr << "\t|DIAGNOSIS REPORTS|\n";
        std::cerr << "\t===================\n\n";
        for (auto const& item : Reports) {
            if (item->Level < printBarrier)
                return;
            std::cerr << "\t"
                      << Reporting_Level_Str[item->Level]
                      << " -> "
                      << item->Msg
                      << std::endl;
        }
#endif
    }

private:
    std::vector<std::unique_ptr<T>> Reports = {};
    LevelT printBarrier = static_cast<LevelT>(0);
};
}
}

namespace Li {
namespace Reporting {

template<typename T, typename LevelT>
void Diagnosis<T, LevelT>::addReport(LevelT level, std::string_view msg)
{
    auto ptr = std::make_unique<T>();
    ptr->Level = level;
    ptr->Msg = msg;
    Reports.emplace_back(std::move(ptr));
}

template<typename T, typename LevelT>
void Diagnosis<T, LevelT>::PrintReports() const
{
    for (auto& bucket : Reports) {
        std::cout << bucket->Msg << "\n";
    }
}

template<typename T, typename LevelT>
bool Diagnosis<T, LevelT>::has(LevelT l)
{
    return std::any_of(Reports.begin(), Reports.end(), [Level = l](auto const& thing) -> bool {
        return (Level == thing->Level);
    });
}

template<typename T, typename LevelT>
void Diagnosis<T, LevelT>::setReportingBarrier(LevelT min)
{
    this->printBarrier = min;
}

}
}

#endif // DIAGNOSIS_H
