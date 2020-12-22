//
// Created by darksider3 on 22.12.20.
//

#ifndef LIGI_APPS_RESPONSES_H
#define LIGI_APPS_RESPONSES_H
#include "AbstractStatus.h"
#include "ClientServerConnections.h"
#include <unordered_map>

using Map = std::unordered_map<std::string, std::string>;
using StringMap = std::unordered_map<std::string, std::string>;
using Li::common::has;

int ToLower(unsigned const char& c)
{
    return std::tolower(c);
}

struct HTTPClientResponse {
    std::string Version {};
    std::string Method {};
    std::string URI {};
    std::string Query {};

    StringMap Fields {};
};

class ResponseBuilder {
private:
public:
    ResponseBuilder() = default;
    ResponseBuilder(const ResponseBuilder&) = delete;
    ResponseBuilder& operator=(ResponseBuilder const&) = delete;
    ResponseBuilder(ResponseBuilder&&) = delete;
    ResponseBuilder& operator=(ResponseBuilder&&) = delete;

    virtual ~ResponseBuilder() = default;
    /**
	 * @brief get returns a full fledged response
	 * @return Response(to clients, from clients)
	 */
    virtual std::string get() = 0;

    /**
	 * @brief length of(given, gotten) response
	 * @return Length of Response
	 */
    [[nodiscard]] virtual size_t length() const = 0;
};

/**
 * @brief The HTTPResponseBuilder class provides an easy, fast interface for basic servings
 */
class HTTPResponseBuilder : public ResponseBuilder {
private:
    std::string m_Resp {};
    std::string m_Status {};
    std::string m_Content_Type { "text/html" };
    std::string m_Charset { "charset=UTF-8" };

    std::string m_out {};

    constexpr static std::string_view CLRF() { return "\r\n"; }

public:
    /**
	 * @brief HTTPResponseBuilder Response-content and Status Code as string constructor
	 * @param std::string_view Contents of the response
	 * @param std::string status code(as a string)
	 */
    explicit HTTPResponseBuilder(const std::string_view str, const std::string&& Status = "200 OK")
        : m_Resp { str }
        , m_Status { Status }
    {
    }

    /**
	 * @brief HTTPResponseBuilder vector-copy ctor providing an easy mechanism to input clients requests
	 * @param std::vector<char>& Vector of characters to append initially
	 */
    explicit HTTPResponseBuilder(const std::vector<char>& c)
    {
        m_Resp.append(c.begin(), c.end());
    }

    /**
	 * @brief HTTPResponseBuilder default ctor
	 */
    explicit HTTPResponseBuilder() = default;

    /**
	 * @brief append to currently holding content
	 * @param str String to append
	 */
    void append(std::string_view const str) { m_Resp.append(str); }

    /**
	 * @brief setStatus sets HTTP status
	 * @param Status to set
	 */
    void setStatus(AbstractStatus const&& Status) { m_Status = Status.get(); }

    /**
	 * @brief setStatus set string as status
	 * @param str Statusstring
	 */
    void setStatus(std::string_view str) { m_Status = str; }

    /**
	 * @brief get Get current build of response
	 * @return std::string holding response
	 */
    std::string get() override
    {
        m_out = "HTTP/1.1 ";
        m_out.append(m_Status);
        m_out.append(CLRF());
        m_out.append(m_Content_Type);
        m_out.append("; ");
        m_out.append(m_Charset);
        m_out.append(CLRF());
        m_out.append(CLRF());
        m_out.append(m_Resp);

        return m_out;
    }

    /**
     * @brief Clear <b>everything</b> from the Response.
     */
    void clear()
    {
        m_Resp.clear();
        m_Status.clear();
        m_Content_Type.clear();
        m_Charset.clear();
        m_out.clear();
    }

    /**
	 * @brief getResponseOnly just returns the response without the headers
	 * @return
	 */
    std::string_view getResponseOnly()
    {
        return m_Resp;
    }

    [[nodiscard]] size_t length() const override { return m_Resp.length(); }
};
#endif //LIGI_APPS_RESPONSES_H
