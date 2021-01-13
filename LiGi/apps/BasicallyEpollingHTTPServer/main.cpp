//
// Created by darksider3 on 12.01.21.
//

#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

constexpr long recv_size = 1024l;
constexpr size_t max_epoll_events = 12000;
constexpr in_port_t sPort = 8080;
constexpr size_t listen_backlog = 1024;

struct ClientDataStruct {
    int fd { -1 };
    ptrdiff_t pos { 0 };
    std::string buf {};
};

class ClientData : std::enable_shared_from_this<ClientData> {
private:
    ClientDataStruct m_Data {};

public:
    std::shared_ptr<ClientData> getptr() { return shared_from_this(); }
    ClientData& the() { return *this; }
    void fd(int fd) { m_Data.fd = fd; }
    int fd() const { return m_Data.fd; }

    void buffer(std::string_view str) { m_Data.buf = str; }
    std::string buffer() const { return m_Data.buf; }

    void advance_pos(int by = 1) { m_Data.pos = m_Data.pos += by; }
    ptrdiff_t pos() { return m_Data.pos; }
};

// FIXME: Decided upon this. Im going to maintain that list of shared_ptrs which represents our connections. Later usage of them are going to be referenced shared_ptrs
using ClientsVec = std::vector<std::shared_ptr<ClientData>>;

int ServerLoop(int timeout = -1)
{
    int flags = 0;
    int reuse = 1;
    int ret = -1;
    int sock = 0;
    int cli_sock = 0;
    char bf[recv_size + 1] = {};
    long n_data = 0;

    sockaddr_in serveraddr {};
    sockaddr_in clientaddr {};
    socklen_t len = 0;

    int epollFD = 0;

    epoll_event ev, events[max_epoll_events];

    epollFD = epoll_create1(0);
    if (epollFD < 0) {
        perror("epoll_create1 failed: ");
        std::exit(1);
    }

    std::cout << "Successfully initialised epoll socket.\n";

    if (sock = socket(AF_INET, SOCK_STREAM, 0); sock < 0) {
        perror("socket failed: ");
        std::exit(2);
    }

    std::cout << "Successfully initialised servers socket\n";

    if (ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); ret < 0) {
        perror("setsockopt");
        close(sock);
        std::exit(3);
    }

    std::cout << "Set socket reuse flag\n";

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(sPort);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (ret = bind(sock, (sockaddr*)&serveraddr, sizeof(serveraddr)); ret < 0) {
        perror("bind");
        close(sock);
        std::exit(4);
    }

    std::cout << "Bound socket\n";

    ev.events = EPOLLIN | EPOLLET;
    // ev.data.fd = sock;
    ev.data.ptr = new ClientDataStruct { .fd = sock };

    auto& orig = ev;

    if (ret = listen(sock, listen_backlog); ret < 0) {
        perror("listen");
        close(sock);
        std::exit(5);
    }

    std::cout << "Listening!\n";

    if (ret = epoll_ctl(epollFD, EPOLL_CTL_ADD, sock, &ev); ret < 0) {
        perror("epoll_ctl");
        close(sock);
        close(epollFD);
        std::exit(6);
    }

    std::cout << "Added listening socket to epoll set! \n";

    if (flags = fcntl(sock, F_GETFL); flags < 0) {
        perror("fcntl");
        close(sock);
        close(epollFD);
        std::exit(7);
    }

    flags = flags | O_NONBLOCK;

    if (ret = fcntl(sock, F_SETFL, flags); ret < 0) {
        perror("fcntl f_setfl");
        close(sock);
        close(epollFD);
        std::exit(8);
    }

    std::cout << "Listening in nonblocking mode now!\n";

    int cnt = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        int nfds = epoll_wait(epollFD, events, max_epoll_events, timeout);
        if (nfds < 0) {
            perror("epoll_wait");
            close(sock);
            close(epollFD);
            std::exit(9);
        }

        if (nfds == 0) {
            std::cout << "Server HeartBeat[1S]: " << cnt++ << std::endl;
        }

        int n = 0;
        for (; n < nfds; ++n) {
            bool cleanup_cur = false;
            auto n_event_ptr = [&]() { return static_cast<ClientDataStruct*>(events[n].data.ptr); };
            // gather FD from client struct
            int tmpFD = n_event_ptr()->fd;
            if ((events[n].events & EPOLLIN) != 0x0) {

                // ========= accepting clients =========
                if (tmpFD == sock) {
                    cli_sock = accept(sock, (sockaddr*)&serveraddr, &len);
                    if (cli_sock < 0) {
                        perror("accept");
                        close(sock);
                        close(epollFD);
                        std::exit(10);
                    }

                    if (flags = fcntl(cli_sock, F_GETFL); flags < 0) {
                        perror("cli_sock_fcntl_getfl");
                        close(sock);
                        close(epollFD);
                        std::exit(11);
                    }

                    flags |= O_NONBLOCK;
                    if (ret = fcntl(cli_sock, F_SETFL, flags); ret < 0) {
                        perror("cli_sock_fcntl_setfl");
                        close(sock);
                        close(epollFD);
                        std::exit(12);
                    }

                    // ======== create client data! ========
                    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
                    ev.data.ptr = new ClientDataStruct { .fd = cli_sock };

                    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, cli_sock, &ev) == -1) {
                        perror("epoll_ctl_add_cli_addr");
                        close(sock);
                        close(epollFD);
                        std::exit(13);
                    }
                } else {

                    // ======== reading clients ========
                    n_data = read(tmpFD, bf, recv_size);
                    if (n_data < 0) { // READ ERROR
                        perror("read");
                        close(sock);
                        close(epollFD);
                        close(tmpFD);
                        std::exit(14);
                    } else if (n_data == 0) { // client close
                        std::cout << "Client closed!\n";
                        std::cout << "read from it: " << n_event_ptr()->pos << " bytes!\n";
                        std::cout << "latest read: " << n_event_ptr()->buf << "\n";
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, tmpFD, NULL);

                        //delete client data
                        close(tmpFD);
                        cleanup_cur = true;
                    }

                    // read loop
                    do {
                        std::cout << "Remote Message: " << bf << "\n";
                        n_event_ptr()->buf = bf;
                        bf[n_data] = '\0';

                        // Zero out buffer!
                        memset(bf, '\0', recv_size);
                        n_event_ptr()->pos += n_data;
                    } while ((n_data = read(tmpFD, bf, recv_size)) > 0);
                }
            }

            // ========= writing to clients =========
            if ((events[n].events & EPOLLOUT) && tmpFD != sock) {
                n_data = write(tmpFD, "WRITE TIME BABY: ", 17);
                std::cout << "TCP Server ready to write data!\n";
            } else if (events[n].events & EPOLLERR) {
                std::cout << "TCP Server EPOLL Error!\n";
            }

            // ========= Cleanup in case we marked it =========
            if (cleanup_cur)
                delete static_cast<ClientDataStruct*>(events[n].data.ptr);
            std::flush(std::cout);
        }
    }

    /*close(sock);
    close(epollFD);
    close(cli_sock);
    delete (static_cast<ClientData*>(orig.data.ptr));

    std::cout << "Cleaned up. Exiting." << std::endl;
     */
    return 0;
#pragma clang diagnostic pop
}

int main(int argc, char** argv)
{
    return ServerLoop();
}