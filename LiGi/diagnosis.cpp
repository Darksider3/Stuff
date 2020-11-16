#include "diagnosis.h"
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
void Diagnosis<T, LevelT>::PrintReports()
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
