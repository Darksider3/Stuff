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

// @TODO: Li::Diagnosis
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
		auto ptr = std::make_unique<T>();
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

class WarcRecord;

using WarcError = std::variant<VersionNotSupported, InvalidField,
	MissingRequiredField, IncompleteRecord>;
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

inline void skipSpaces(std::istream& ins)
{
	while (std::isspace(ins.peek())) {
		ins.ignore(1);
	}
}

std::optional<InvalidField> ReadSubsequentFields(std::istream& ins, Field_Map& FM)
{
	// @TODO: Li::string_helper::to_lower(std::string str) -->
	/*
	 * std::transform(str.begin(), str.end(), str.begin(), [](unsigned const char &c) -> int {return std::tolower(c);};
	 *
	 * AND
	 * Li::string_helper::to_lower(std::string str)
	 * std::transform(str.begin(), str.end(), str.begin(), [](const char &c) -> char {return static_cast<char>(static_cast<unsigned char>(c));};
	 */

	auto strToLower = [](unsigned const char& c) -> int {
		return std::tolower(c);
	};

	std::string line;
	std::getline(ins, line);
	while (!line.empty() && line != "\r") {                    // Spec want's an empty line between record and data
		auto [name, value] = Li::common::SplitPair(line, ':'); // std::string, std::string
		if (name.empty() || value.empty()) {
			return InvalidField { std::move(line) };
		}

		name = FieldTrim(name);
		value = FieldTrim(value);
		std::transform(name.begin(), name.end(), name.begin(), strToLower);

		FM[std::move(name)] = std::move(value);
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

class WarcRecord {
private:
	using MapIterator = std::unordered_map<std::string, std::string>::iterator;

	// Mandatory fields as of Spec Version 1.1
	static std::string const Type_str;
	static std::string const RecordID_str;
	static std::string const ContentLength_str;
	static std::string const Date_str;
	static std::string const TargetURI;

	Field_Map m_FM {};

	std::string m_version {};

	std::string m_content {};

	std::pair<std::size_t, std::size_t> start_end {};

public:
	WarcRecord() = default;

	explicit WarcRecord(std::string_view version)
		: m_version(std::move(version))
	{
	}

	inline bool has_(std::string const& field_name) const noexcept
	{
		return m_FM.find(field_name) != m_FM.end();
	}

	inline bool Valid()
	{
		return has_(Type_str) && has_(ContentLength_str) && has_(RecordID_str) && has_(Date_str);
	}

	std::size_t content_length() const
	{
		std::string value = m_FM.at(ContentLength_str);
		try {
			return std::stoi(value);
		} catch (std::invalid_argument& err) {
			std::ostringstream os;
			os << "Could not convert content_length value '" << value << "'.";
			throw std::runtime_error(os.str());
		}
	}

	WarcRecord* is()
	{
		return this;
	}

	inline std::string&& content() { return std::move(m_content); }
	inline std::string const& content() const { return m_content; }

	inline std::string&& url() { return std::move(m_FM.at(TargetURI)); }
	inline std::string const& url() const { return m_FM.at(TargetURI); }

	std::optional<std::string> field(std::string const& n)
	{
		if (MapIterator pos = m_FM.find(n); pos != m_FM.end()) {
			return pos->second;
		}

		return std::nullopt;
	}
	void dumpMap()
	{
		for (auto& buck : m_FM) {
			std::cout << " Key: " << buck.first
					  << " Val: " << buck.second
					  << "\n";
		}
	}

	friend Result ReadRecords(std::istream&);
};

Result ReadRecords(std::istream& in)
{
	WarcRecord record;

	if (auto error = ReadVersion(in, record.m_version); error) {
		return Result(*error);
	}

	if (auto error = ReadSubsequentFields(in, record.m_FM); error)
		return Result(*error);

	if (!record.Valid())
		return Result(MissingRequiredField {});

	if (record.content_length() > 0) {
		std::size_t len = record.content_length();
		record.m_content.resize(len);
		if (!in.read(&record.m_content[0], len)) {
			return Result(IncompleteRecord {});
		}
	}
	skipSpaces(in);
	return Result(record);
}

std::string const WarcRecord::Type_str = "warc-type";
std::string const WarcRecord::RecordID_str = "warc-record-id";
std::string const WarcRecord::ContentLength_str = "content-length";
std::string const WarcRecord::Date_str = "warc-date";
std::string const WarcRecord::TargetURI = "warc-target-uri";

void testfunc()
{
}

#endif // PARSER_H
