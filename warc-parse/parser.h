#ifndef PARSER_H
#define PARSER_H
#include "../LiGi/GeneralTools.h" // splitPair
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <vector>

#ifndef NDEBUG
template<typename T>
[[noreturn]] constexpr void UNREACHABLE(const T& msg = "<No Message>")
{
	std::cerr << "Unreachable code reached. Message: " << msg << std::endl;
	std::abort();
}
#else
template<typename T>
[[noreturn]] void UNREACHABLE(const T& msg = "<No Message>")
{
	std::cerr << "Unreachable code reached. Message: " << msg << std::endl;
}
#endif

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
	void addReport(LevelT level, std::string_view msg)
	{
		auto ptr = std::make_shared<T>();
		ptr->Level = level;
		ptr->Msg = msg;
		Reports.emplace_back(std::move(ptr));
	}

	void PrintReports()
	{
		for (auto& bucket : Reports) {
			std::cout << bucket->Msg << "\n";
		}
	}

	bool has(LevelT l)
	{
		return std::any_of(Reports.begin(), Reports.end(), [Level = l](auto const& thing) -> bool {
			return (Level == thing->Level);
		});
	}

	void setReportingBarrier(LevelT min)
	{
		this->printBarrier = min;
	}

	~Diagnosis()
	{
#if !defined(NDEBUG) || !defined(LI_NO_DIAGNOSIS)
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
	std::vector<std::shared_ptr<T>> Reports;
	LevelT printBarrier = static_cast<LevelT>(0);
};

#include <unordered_map>
#include <variant>

struct VersionNotSupported {
	std::string details;
};

struct InvalidField {
	std::string detail;
};

struct MissingRequiredField {
	std::vector<std::string> list;
};

struct IncompleteRecord {
};

using WarcError = std::variant<VersionNotSupported, InvalidField,
	MissingRequiredField, IncompleteRecord>;

class WarcRecord;
using Result = std::variant<WarcRecord, WarcError>;

using Field_Map = std::unordered_map<std::string, std::string>;

template<typename StringT>
std::string FieldTrim(StringT str)
{
	auto begin = str.begin();
	auto end = str.end();
	begin = std::find_if_not(begin, end, [](char cur) -> bool {
		return std::isspace(cur);
	});

	end = std::find_if(begin, end, [](char cur) -> bool {
		return std::isspace(cur);
	});

	return StringT(begin, end);
}

std::optional<InvalidField> ReadFields(std::istream& ins, Field_Map& FM)
{
	auto strToLower = [](unsigned const char& c) {
		return std::tolower(c);
	};

	std::string line;
	std::getline(ins, line);
	while (!line.empty() && line != "\r") {
		auto [name, value] = Li::common::SplitPair(line, ':'); // std::string, std::string
		if (name.empty() || value.empty()) {
			return InvalidField { line };
		}

		name = FieldTrim(name);
		value = FieldTrim(value);

		std::transform(name.begin(), name.end(), name.begin(), strToLower);
		FM[std::string(name.begin(), name.end())] = std::string(value.begin(), value.end()); // FM["fieldname"] = "FieldValue"
		std::getline(ins, line);
	}

	return std::nullopt; // alternative {}
}

std::optional<VersionNotSupported> ReadVersion(std::istream& ins, std::string& version)
{
	std::string_view Prefix = "WARC/";
	std::string line {};
	if (!std::getline(ins, line)) {
		return VersionNotSupported { std::move(line) };
	}

	std::string trimmed_line = FieldTrim(line);
	if (trimmed_line.size() < (Prefix.size() + 1) || std::string_view(&trimmed_line[0], Prefix.size()) != Prefix) {
		return VersionNotSupported { std::move(line) };
	}
	version = std::string(std::next(trimmed_line.begin(), Prefix.size()), trimmed_line.end());
	return std::nullopt;
}

// @TODO: Move to own file - Li::range_helpers::for_each?
template<typename RangeT, typename FunctionT>
FunctionT for_each(RangeT& range, FunctionT& f)
{
	return std::for_each(std::begin(range), std::end(range), f);
}

void testfunc()
{
}

#endif // PARSER_H
