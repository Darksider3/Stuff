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
};

std::string Reporting_Level_Str[] = {
	"EWARN",
	"ERROR",
	"EINFO"
};

struct Report {
	Reporting_Level Level;
	std::string Msg;
};

template<typename T = Report, typename LevelT = Reporting_Level>
class Diagnosis {
public:
	Diagnosis() = default;
	explicit Diagnosis(LevelT L)
		: printBarrier { L }
	{
	}

	void addReport(LevelT, std::string_view);

	void PrintReports();

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
	std::vector<std::unique_ptr<T>> Reports;
	LevelT printBarrier = static_cast<LevelT>(0);
};
}
}
#endif // DIAGNOSIS_H
