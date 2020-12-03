#ifndef VERIFIERS_H
#define VERIFIERS_H
#include "../LiGi/diagnosis.h"
#include "parser.h"
#include <variant>

namespace Warc {

namespace ProvidedVerifier {
struct WrongHeaderTypeDelivered {
	std::string type;
};

using Error = WrongHeaderTypeDelivered;
using Result = std::variant<bool, Error>;

class Verifier {
public:
	Verifier() = delete;
	explicit Verifier(Warc::Record const& Record)
		: m_Record(Record)
	{
		auto ver = verify();
		if (auto errmatch = std::get_if<Error>(&ver); errmatch != nullptr) {
			m_valid = false;

			if constexpr (std::is_same_v<decltype(WrongHeaderTypeDelivered(*errmatch)), void>) {
				return;
			}
		}
		m_valid = std::get<bool>(verify());
	}

	bool valid()
	{
		return m_valid;
	}

	virtual Result verify();

	virtual ~Verifier() = default;

protected:
	Warc::Record const& m_Record;
	using RLevel = Li::Reporting::Reporting_Level;
	using Diagnosis = Li::Reporting::Diagnosis<Li::Reporting::Report, RLevel>;
	Diagnosis m_Diag {};

private:
	bool m_valid = false;
};

class WarcinfoVerifier : public Verifier {
	Result verify() override
	{
		if (auto str = m_Record.type(); str != "warcinfo") {
			return WrongHeaderTypeDelivered { str };
		}
		if (!m_Record.has_("operator")) {
			m_Diag.addReport(RLevel::Rec, "Specification recommends 'operator' contact information in Warcinfo");
		}
		if (!m_Record.has_("software")) {
			m_Diag.addReport(RLevel::Rec, "Specification recommends 'software' version information in Warcinfo");
		}
		if (!m_Record.has_("robots")) {
			m_Diag.addReport(RLevel::Rec, "Specification recommends 'robots' policy information(e.g. classic) in Warcinfo");
		}
		if (!m_Record.has_("hostname")) {
			m_Diag.addReport(RLevel::Rec, "Specification recommends 'hostname' information(machine which created the warcinfo) in Warcinfo");
		}
		if (!m_Record.has_("ip")) {
			m_Diag.addReport(RLevel::Rec, "Specification recommends 'ip'(of the machine which created the warcinfo) as a field in Warcinfo");
		}
		if (!m_Record.has_("http-header-user-agent")) {
			m_Diag.addReport(RLevel::Rec, "Specification recommends 'http-header-user-agent'in Warcinfo(which was used to create the warc)");
		}
		if (!m_Record.has_("http-header-from")) {
			m_Diag.addReport(RLevel::Rec, "Specification recommends 'http-header-from' as a field in Warcinfo(from which url the url was visited which got harvested)");
		}
		return true;
	}
};

}

}
#endif // VERIFIERS_H
