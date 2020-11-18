#ifndef PARSER_H
#define PARSER_H
#include "../LiGi/GeneralTools.h" // splitPair
#include <cassert>
#include <cstring>
#include <memory>
#include <optional>
#include <ostream>

#include <unordered_map>
#include <variant>
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
namespace Warc {

struct VersionNotSupported {
	std::string details;
};

struct InvalidField {
	std::string detail;
};

struct MissingRequiredField {
	std::string detail;
};

struct IncompleteRecord {
};

class Record;

using Error = std::variant<VersionNotSupported, InvalidField,
	MissingRequiredField, IncompleteRecord>;
using Result = std::variant<Record, Error>;
using Field_Map = std::unordered_map<std::string, std::string>;

namespace detail {

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

inline void SkipSuperfluousWhitespace(std::istream& ins)
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

} // detail

class Record {
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

public:
	Record() = default;

	explicit Record(std::string_view version)
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

	bool validType()
	{
		std::optional<std::string> type_opt = field("warc-type");

		if (!type_opt.has_value())
			return false;

		std::string_view type = type_opt.value();
		return (type == "warcinfo"
			|| type == "response"
			|| type == "resource"
			|| type == "request"
			|| type == "metadata"
			|| type == "revisit"
			|| type == "conversion"
			|| type == "continuation");
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
	Record record;

	if (auto error = detail::ReadVersion(in, record.m_version); error) {
		return Result(*error);
	}

	if (auto error = detail::ReadSubsequentFields(in, record.m_FM); error)
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
	detail::SkipSuperfluousWhitespace(in);
	return Result(record);
}

constexpr bool holds(Result const& result) noexcept { return std::holds_alternative<Record>(result); }

std::string const Record::Type_str = "warc-type";
std::string const Record::RecordID_str = "warc-record-id";
std::string const Record::ContentLength_str = "content-length";
std::string const Record::Date_str = "warc-date";
std::string const Record::TargetURI = "warc-target-uri";

template<typename Result = Warc::Result, typename RecordHandle, typename ErrHandler>
auto match(Result const&& result, RecordHandle const&& record_handler, ErrHandler const&& err_handler)
{
	if (Record const* record = std::get_if<Record>(&result); record != nullptr) {
		if constexpr (std::is_same_v<decltype(record_handler(*record)), void>) { // if return-type of (compile-time) record_handler == void, return nothing
			record_handler(*record);
		} else { // else, return whatever record_handler want's to return
			return record_handler(*record);
		}
	} else {
		Error const* err = std::get_if<Error>(&result);
		if constexpr (std::is_same_v<decltype(err_handler(*err)), void>) {
			err_handler(*err);
		} else {
			return err_handler(*err);
		}
	}
}

template<
	typename VersionNotSupported_F, typename InvalidField_F,
	typename MissingRequiredField_F, typename InvalidRecord_F>
auto match(Warc::Error const&& Err,
	InvalidRecord_F const&& InvalidRecord_Handler,
	MissingRequiredField_F const&& MissingRequired_Handler,
	InvalidField_F const&& InvalidField_Handler,
	VersionNotSupported_F const&& VersionNotSupported_Handler)
{
	if (auto* VNS = std::get_if<VersionNotSupported>(&Err); VNS != nullptr) {
		if constexpr (std::is_same_v<decltype(VersionNotSupported_Handler(*VNS)), void>) {
			VersionNotSupported_Handler(*VNS);
		} else {
			return VersionNotSupported_Handler(*VNS);
		}
	}

	if (auto* IF = std::get_if<InvalidField>(&Err); IF != nullptr) {
		if constexpr (std::is_same_v<decltype(InvalidField_Handler(*IF)), void>) {
			InvalidField_Handler(*IF);
		} else {
			return InvalidField_Handler(*IF);
		}
	}

	if (auto* MRF = std::get_if<MissingRequiredField>(&Err); MRF != nullptr) {
		if constexpr (std::is_same_v<decltype(MissingRequired_Handler(*MRF)), void>) {
			MissingRequired_Handler(*MRF);
		} else {
			return MissingRequired_Handler(*MRF);
		}
	}

	if (auto* IR = std::get_if<IncompleteRecord>(&Err); IR != nullptr) {
		if constexpr (std::is_same_v<decltype(InvalidRecord_Handler(*IR)), void>) {
			InvalidRecord_Handler(*IR);
		} else {
			return InvalidRecord_Handler(*IR);
		}
	}
}

namespace ProvidedVerifier {

}

namespace ProvidedHandler {

}

}

void testfunc()
{
}

#endif // PARSER_H
