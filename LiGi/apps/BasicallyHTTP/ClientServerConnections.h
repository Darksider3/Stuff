//
// Created by darksider3 on 22.12.20.
//

#ifndef LIGI_APPS_CLIENTSERVERCONNECTIONS_H
#define LIGI_APPS_CLIENTSERVERCONNECTIONS_H
#include "AbstractConnection.h"
#include "Constants.h"
#include "Responses.h"
#include <memory>
#include <string>

class ServerConnection {
};

/**
 * @brief The ResponseBuilder class provides the interface for Responses
 */
class ClientConnection : public AbstractConnection<ClientConnection> {
private:
    std::shared_ptr<HTTPResponseBuilder> m_OutResponse {};

public:
    ClientConnection(const int& Socket, sockaddr_storage& stor)
    {
        setSock(&Socket);
        setInfo(stor);
        m_OutResponse = std::make_shared<HTTPResponseBuilder>();
    }

    ClientConnection(ClientConnection&& other) noexcept
    {
        m_OutResponse = other.m_OutResponse;
    }

    ClientConnection& operator=(ClientConnection&& other) noexcept
    {
        m_OutResponse = other.m_OutResponse;
        return *this;
    }

    /**
     * @brief Return up to N bytes. Will read either till the specified maximum len or, in case of close or error, return the content read up to this point
     * @param std::vector<char>& Temporary buffer - Subject to change here, will probably just allocate one myself at that point
     * @param ssize_t maximum read length
     * @return std::string
     */
    std::string_view ReadUntilN(std::vector<char>& into, const ssize_t max = max_buf_len)
    {
        std::string ret;
        into.reserve(max * 2);

        ssize_t bytes_received = 0;
        do {
            bytes_received = recv(Sock, &into[bytes_received], into.size(), 0);
            if (bytes_received == -1) { // error out
            } else {
                ret.append(into.begin(), into.end());
            }
        } while (bytes_received == max); // errored out!

        std::cout << "have read: " << bytes_received << std::endl;
        return ret;
    }

    std::shared_ptr<HTTPResponseBuilder> outResp()
    {
        return m_OutResponse;
    }

    ClientConnection(const ClientConnection&) = delete;
    ClientConnection& operator=(ClientConnection const&) = delete;
    ~ClientConnection() override
    {
        Close();
    }
};
#endif //LIGI_APPS_CLIENTSERVERCONNECTIONS_H
