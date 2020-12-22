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
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection& operator=(ClientConnection const&) = delete;
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
    std::string ReadUntilN(std::vector<char>& into, const size_t max_read, const size_t buf_max = max_buf_len)
    {
        std::cout << "got length: " << max_read << std::endl;
        std::string ret;
        into.reserve(buf_max);

        ssize_t bytes_received = 0;
        size_t counter = 0;
        do {
            bytes_received = recv(Sock, &into[0], into.size(), 0);
            if (bytes_received == -1) { // error out
                std::cout << "out" << std::endl;
                perror("recv");
            } else {
                ret.append(into.begin(), into.end());
            }
            counter += bytes_received;
        } while (bytes_received == buf_max && counter <= max_read); // errored out!

        return ret;
    }

    std::shared_ptr<HTTPResponseBuilder> outResp()
    {
        return m_OutResponse;
    }

    ~ClientConnection() override
    {
        Close();
    }
};
#endif //LIGI_APPS_CLIENTSERVERCONNECTIONS_H
