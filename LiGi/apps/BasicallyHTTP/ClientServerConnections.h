//
// Created by darksider3 on 22.12.20.
//

#ifndef LIGI_APPS_CLIENTSERVERCONNECTIONS_H
#define LIGI_APPS_CLIENTSERVERCONNECTIONS_H
#include "AbstractConnection.h"
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

    std::string ReadUntilN(std::vector<char>& into, const ssize_t max = 4096)
    {
        std::string ret;
        into.reserve(max * 2);

        ssize_t bytes_received = 0;
        do {
            bytes_received = recv(Sock, &into[0], into.size(), 0);
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
