//
// Created by darksider3 on 12.01.21.
//

#include <arpa/inet.h>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

constexpr long recv_size = 1024l;
constexpr size_t max_epoll_events = 1024;
constexpr in_port_t sPort = 8080;
constexpr size_t listen_backlog = 1024;
std::atomic_bool c_v = false;

struct ClientDataStruct {
    int fd { -1 };
    ptrdiff_t pos { 0 };
    std::string buf {};
    std::mutex _struct_lock {};
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

    void advance_pos(int by = 1) { m_Data.pos = m_Data.pos + by; }
    ptrdiff_t pos() { return m_Data.pos; }
};

// FIXME: Decided upon this. Im going to maintain that list of shared_ptrs which represents our connections. Later usage of them are going to be referenced shared_ptrs
struct EpollSet {
    int epoll_fd {};
    epoll_event events[max_epoll_events];
};
using ClientsVec = std::vector<std::shared_ptr<ClientData>>;

struct ServerSet {
    ClientsVec Clients;
    sockaddr_in srv_addr {};
    sockaddr_in cli_addr {};

    EpollSet epoll {};
    int server_socket { -1 };
    int cli_sock { -1 };
    int event_fd { -1 };
    int reuse_addr { -1 };
};

struct Srv {
private:
    sockaddr_in serveraddr {};
    sockaddr_in clientaddr {};
    int epollFD {};

    socklen_t len = 0;
    int server_socket = 0;
    int cli_sock = 0;

    int efd { -1 };
    int EpollCreate()
    {
        epollFD = epoll_create1(0);
        if (epollFD < 0) {
            perror("epoll_create1 failed: ");
            return 1;
        }
        int reuse = 1;

        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        std::cout << "Successfully initialised epoll socket.\n";
        return 0;
    }

    int CreateSock()
    {

        if (server_socket = socket(AF_INET, SOCK_STREAM, 0); server_socket < 0) {
            perror("socket failed: ");
            return 1;
        }

        std::cout << "Successfully initialised servers socket\n";

        return 0;
    }

    int BindSock()
    {

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(sPort);
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(server_socket, (sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
            perror("bind");
            close(server_socket);
            c_v = true;
            return 4;
        }
        return 1;
    }

public:
    void ThreadLoop(int timeout, int _flags)
    {
        epoll_event ev, events[max_epoll_events];
        char bf[recv_size + 1] = {};
        long n_data = 0;
        int cnt = 0;
        int flags = _flags;
        int ret;

        while (!c_v) {
            int nfds = epoll_wait(epollFD, events, max_epoll_events, timeout);
            if (nfds < 0) {
                perror("epoll_wait");
                std::cout << strerror(errno) << std::endl;
                return;
            }

            if (nfds == 0) {
                std::cout << "Server HeartBeat[1S]: " << cnt++ << std::endl;
            }

            int n = 0;
            for (; n < nfds; ++n) {
                bool cleanup_cur = false;
                auto n_event_ptr = [&]() constexpr { return static_cast<ClientDataStruct*>(events[n].data.ptr); };
                // gather FD from client struct
                int tmpFD = n_event_ptr()->fd;
                if ((events[n].events & EPOLLIN) != 0x0) {

                    // ========= accepting clients =========

                    // ========= check for efd sentinel =========
                    if (efd == tmpFD) {
                        // ========= stop all actions =========
                        return;
                    }
                    if (tmpFD == server_socket) {
                        cli_sock = accept(server_socket, (sockaddr*)&serveraddr, &len);
                        if (cli_sock < 0) {
                            perror("accept");
                            return;
                        }

                        if (flags = fcntl(cli_sock, F_GETFL); flags < 0) {
                            perror("cli_sock_fcntl_getfl");
                            return;
                        }

                        flags |= O_NONBLOCK;
                        if (ret = fcntl(cli_sock, F_SETFL, flags); ret < 0) {
                            perror("cli_sock_fcntl_setfl");
                            return;
                        }

                        // ======== create client data! ========
                        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
                        ev.data.ptr = new ClientDataStruct { .fd = cli_sock };

                        if (epoll_ctl(epollFD, EPOLL_CTL_ADD, cli_sock, &ev) == -1) {
                            perror("epoll_ctl_add_cli_addr");
                            return;
                        }
                    } else {
                        std::lock_guard<std::mutex> _guard { n_event_ptr()->_struct_lock };
                        // ======== reading clients ========
                        n_data = read(tmpFD, bf, recv_size);
                        if (n_data < 0) { // READ ERROR
                            perror("read");
                            return;
                        } else if (n_data == 0) { // client close
                            rm_fd(n_event_ptr(), &tmpFD);
                            cleanup_cur = true;
                        }

                        // read loop
                        /*                        do {
                            std::cout << "Remote Message: " << bf << "\n";
                            n_event_ptr()->buf = bf;
                            bf[n_data] = '\0';

                            // Zero out buffer!
                            memset(bf, '\0', recv_size);
                            n_event_ptr()->pos += n_data;
                        } while ((n_data = read(tmpFD, bf, recv_size)) > 0);*/
                        handle_remote_msg(n_event_ptr(), tmpFD, bf, n_data);
                    }
                }

                // ========= writing to clients =========
                if ((events[n].events & EPOLLOUT) && tmpFD != server_socket) {
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
    }

    int ServerLoop(int _efd, int timeout = -1)
    {
        epoll_event ev, events[max_epoll_events];
        efd = _efd;
        int reuse = 1;
        int ret = -1;
        int flags = 0;

        // ========= setup =========
        if (EpollCreate() != 0)
            return 1;

        if (CreateSock() != 0)
            return 1;

        BindSock();

        ev.events = EPOLLIN | EPOLLET;
        // ev.data.fd = sock;
        ClientDataStruct* orig = new ClientDataStruct { .fd = server_socket };
        ev.data.ptr = orig;

        if (ret = listen(server_socket, listen_backlog); ret < 0) {
            perror("listen");
            close(server_socket);
            c_v = true;
            return 5;
        }

        std::cout << "Listening!\n";

        if (ret = epoll_ctl(epollFD, EPOLL_CTL_ADD, server_socket, &ev); ret < 0) {
            perror("epoll_ctl");
            close(server_socket);
            close(epollFD);
            c_v = true;
            return 6;
        }

        std::cout << "inserting eventfd for good measure to kill later"
                  << "\n";

        // ========= Add sentinel to epolls FD set =========
        epoll_event fd_event {};
        fd_event.events = EPOLLHUP | EPOLLET | EPOLLIN;
        fd_event.data.ptr = new ClientDataStruct { .fd = efd };
        if (ret = epoll_ctl(epollFD, EPOLL_CTL_ADD, efd, &fd_event); ret < 0) {
            perror("epoll_ctl_sentinel");
            close(server_socket);
            close(epollFD);
            c_v = true;
            return 6;
        }

        std::cout << "Added listening socket to epoll set! \n";

        if (flags = fcntl(server_socket, F_GETFL); flags < 0) {
            perror("fcntl");
            close(server_socket);
            close(epollFD);
            c_v = true;
            return 7;
        }

        flags = flags | O_NONBLOCK;

        if (ret = fcntl(server_socket, F_SETFL, flags); ret < 0) {
            perror("fcntl f_setfl");
            close(server_socket);
            close(epollFD);
            c_v = true;
            return 8;
        }

        std::cout << "Listening in nonblocking mode now!\n";

        std::jthread thread1 = std::jthread([this, flags] { ThreadLoop(-1, flags); });
        ThreadLoop(-1, flags);
        close(server_socket);
        close(cli_sock);
        close(epollFD);
        delete orig;
        delete (static_cast<ClientDataStruct*>(fd_event.data.ptr));

        std::cout << "Cleaned up. Exiting." << std::endl;
        return 0;
    }

    void operator()(int efd, int timeout)
    {
        ServerLoop(efd, timeout);
    }

    static void handle_remote_msg(ClientDataStruct* data, int tmpFD, char* bf, long& read_size)
    {
        data->buf.reserve(recv_size);
        do {
            //    std::cout << "Remote Message: " << bf << "\n";
            data->buf = bf;
            bf[read_size] = '\0';

            // Zero out buffer!
            memset(bf, '\0', recv_size);
            data->pos += read_size;
        } while ((read_size = read(tmpFD, bf, recv_size)) > 0);
    }

    void rm_fd(ClientDataStruct* data, int* tmpFD)
    {

        std::cout << "Client closed!\n";
        std::cout << "read from it: " << data->pos << " bytes!\n";
        std::cout << "latest read: " << data->buf << "\n";
        epoll_ctl(epollFD, EPOLL_CTL_DEL, *tmpFD, NULL);

        //delete client data
        close(*tmpFD);
    }
};

int main(int argc, char** argv)
{
    // ========= eventfd sentinel =========
    int efd;
    efd = eventfd(0, EFD_NONBLOCK);

    // ========= signal handling =========
    // ====== Flag function =======
    auto flagFunc = [](int) -> void {
        c_v = true;
        return;
    };
    struct sigaction a {
    };
    a.sa_handler = flagFunc;
    a.sa_flags = 0;
    sigemptyset(&a.sa_mask);
    sigaction(SIGINT, &a, nullptr);
    signal(SIGPIPE, SIG_IGN); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast) -> cant influence this

    // ========= thread =========
    Srv Server;
    std::jthread b = std::jthread(Server, efd, -1);
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //std::jthread h = std::jthread(std::bind(&Srv::ThreadLoop, std::ref(Server), -1, 0));
    while (!c_v) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::cout << "Killing... " << std::endl;

    // ========= cleanup thread =========
    write(efd, &a, sizeof(a));
    close(efd);
}