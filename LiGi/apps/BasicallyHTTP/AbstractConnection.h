//
// Created by darksider3 on 22.12.20.
//

#ifndef LIGI_APPS_ABSTRACTCONNECTION_H
#define LIGI_APPS_ABSTRACTCONNECTION_H

#include "Constants.h"
#include "LiGi/GeneralTools.h"
#include <arpa/inet.h>
#include <memory>
#include <netdb.h>

// clang-format off
template<typename T>
concept SocketAddrStore = std::is_same_v<std::remove_cvref_t<T>, sockaddr_in>
                          || std::is_same_v<std::remove_cvref_t<T>, sockaddr_in6>
                          || std::is_same_v<std::remove_cvref_t<T>, sockaddr_storage>;

// clang-format on

void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &((reinterpret_cast<struct sockaddr_in*>(sa))->sin_addr);
    }

    return &((reinterpret_cast<struct sockaddr_in6*>(sa))->sin6_addr);
}
template<typename T>
class AbstractConnection {
private:
    friend T;

    T* d = static_cast<T*>(this);
    const T& const_d = static_cast<const T&>(*this);

    explicit AbstractConnection()
    {
        d->Storage = std::make_shared<sockaddr_storage>();
    }

    std::shared_ptr<sockaddr_storage> Storage;
    int Sock { 0 };

public:
    AbstractConnection(const AbstractConnection&) = delete;
    AbstractConnection& operator=(AbstractConnection const&) = delete;
    AbstractConnection& operator=(AbstractConnection&&) = delete;

    /**
     * @brief Weither a socket is still alive or not
     * @return bool True, when still alive, false when not
     */
    [[maybe_unused]] [[nodiscard]] bool is_alive() const { return Sock > 0; }

    /**
     * @see getPeerName(const sockaddr_storage*)
     */
    std::pair<std::string, in_port_t> getPeerName()
    {
        if (!const_d.Storage)
            return {};
        return getPeerName(&reinterpret_cast<sockaddr_storage&>(*d->Storage));
    }

    /**
     * @brief Retrieve IP and Port of remote connection
     * @param sockaddr_stroage* Sockaddr_Storage to derive peer information from
     * @return std::pair<std::string, in_port_t> IP(as a string) and Port Pair of a connection
     */
    std::pair<std::string, in_port_t> getPeerName(const sockaddr_storage* s)
    {
        char ipstr[INET6_ADDRSTRLEN];
        in_port_t remote_port;
        inet_ntop(const_d.Storage->ss_family,
            get_in_addr((struct sockaddr*)d->Storage.get()),
            &ipstr[0], sizeof ipstr);

        if (s->ss_family == AF_INET6) { // ipv6
            auto* ad = reinterpret_cast<sockaddr_in6*>(&s);
            remote_port = ntohs(ad->sin6_port);
        } else { // ipv4
            auto* ad = reinterpret_cast<struct sockaddr_in*>(&s);
            remote_port = ntohs(ad->sin_port);
        }

        return { ipstr, remote_port };
    }

    template<SocketAddrStore F>
    void setInfo(F& store)
    {
        memcpy(reinterpret_cast<unsigned char*>(d->Storage.get()),
            reinterpret_cast<unsigned char*>(&store), sizeof(F));
    }

    void setSock(const int* s)
    {
        d->Sock = *s;
    }

    static AbstractConnection<T>&& Open(const int& domain, const int& type, const int& proto)
    {
        int sock = socket(domain, type, proto);
        AbstractConnection<T> obj = AbstractConnection<T>();
        obj.setSock(sock);
        return std::move(obj);
    }

    void Close()
    {
        if (d->Sock != -1)
            close(d->Sock);
        d->Sock = -1;
    }

    [[nodiscard]] ssize_t Write(const std::string& data)
    {
        return Write(data.c_str(), data.length());
    }

    [[nodiscard]] ssize_t Write(const std::vector<char>& vec)
    {
        return Write(&vec[0], vec.size());
    }

    ssize_t Write(const char* thing, const size_t len)
    {
        return write(d->Sock, thing, len);
    }

    ssize_t Read(std::vector<char>& vec)
    {
        return Read(&vec[0], vec.size());
    }

    ssize_t Read(char* thing, const size_t maxlen = max_buf_len)
    {
        return read(d->Sock, thing, maxlen);
    }

    AbstractConnection(AbstractConnection&& other) noexcept
    {
        this->d->Storage = std::move(other.Storage);
        this->d->Sock = { other.d->Sock };
    }

    virtual ~AbstractConnection()
    {
        Close();
    }
};
#endif //LIGI_APPS_ABSTRACTCONNECTION_H
