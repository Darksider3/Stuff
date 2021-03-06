//
// Created by darksider3 on 12.01.21.
//

#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include <exception>
#include <poll.h>

#include "LiGi/GeneralTools.h"

constexpr long recv_size = 8192l;
constexpr size_t max_epoll_events = 1024;
constexpr in_port_t sPort = 8080;
constexpr size_t listen_backlog = 8192l;

constexpr size_t max_errnos = 1024;

constexpr auto wait_tick_time = std::chrono::milliseconds(50);

std::atomic_bool c_v = false;
std::atomic_ulong thread_num = 6;
std::atomic_int Running_Threads = 0;

struct ClientDataStruct {
    int fd { -1 };
    int cli_sock { -1 };
    ptrdiff_t pos { 0 };
    std::string buf {};
    std::mutex _struct_lock {};
};

class Socket {
private:
    int Sock { -1 };

public:
    struct ErrnoConverting : public std::runtime_error {
        ErrnoConverting(int err)
            : std::runtime_error(strerror(err))
        {
        }
    };

    struct OptionInvalid : public ErrnoConverting {
        OptionInvalid(int err)
            : ErrnoConverting(err)
        {
        }
    };

    Socket() = default;
    Socket(Socket&&) = default;
    Socket& operator=(Socket&&) = default;

    // don't ever copy a socket. This would break anything related!
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    explicit Socket(int s)
        : Sock { s }
    {
    }

    void create(int domain, int type, int proto)
    {
        Sock = socket(domain, type, proto);
        if (Sock > 0) {
            throw std::runtime_error("Couldn't create socket: " + std::string(strerror(errno)));
        }
    }

    void Option(int Ftype, int Flag, bool Enable)
    {
        if (setsockopt(Sock, Ftype, Flag, &Enable, sizeof(Enable)) == -1) {
            throw OptionInvalid(errno);
        }

        return;
    }

    void bind(const sockaddr& Addr)
    {
        if (::bind(Sock, &Addr, sizeof(Addr)) < 0) {
            throw ErrnoConverting(errno);
        }
    }

    void listen(int backlog = listen_backlog)
    {
        if (::listen(Sock, backlog) < 0) {
            throw ErrnoConverting(errno);
        }
    }

    void on(std::function<void(int)>& f) // currently for things like.. to call fcntl and such
    {
        f(Sock);
    }

    ~Socket()
    {
        if (Sock > 0)
            close(Sock);
    }
};

class Srv {
private:
    // Thread Vector
    std::vector<std::thread> m_Threads;

    // Server address
    sockaddr_in serveraddr {};

    // client address
    sockaddr_in clientaddr {};

    // epoll watch set
    int epollFD {};

    // length for the accept call(race condition?)
    socklen_t len = 0;

    // servers socket it operates on
    int server_socket = 0;

    // client tmp socket
    int cli_sock = 0;

    // eventfd
    int efd { -1 };

    std::shared_ptr<ClientDataStruct> m_original;
    // first function to call
    int CreateSock()
    {

        if (server_socket = socket(AF_INET, SOCK_STREAM, 0); server_socket < 0) {
            perror("socket failed: ");
            return 1;
        }
        int reuse = 1;

        timeval timeout { .tv_sec = 3, .tv_usec = 0 };

        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)) < 0) {
            perror("setsockopt failed");
        }

        if (setsockopt(server_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)) < 0) {
            perror("setsockopt failed^");
        }

        std::cout << "Successfully initialised servers socket\n";

        return 0;
    }

    // create an epoll set - meant to be used after the general socket setup
    int EpollCreate()
    {
        epollFD = epoll_create1(0);
        if (epollFD < 0) {
            perror("epoll_create1 failed: ");
            return 1;
        }
        std::cout << "Successfully initialised epoll socket.\n";
        return 0;
    }

    // third function to call
    int BindSock()
    {

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(sPort);
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(server_socket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) < 0) {
            perror("bind");
            close(server_socket);
            c_v = true;
            return 4;
        }
        return 1;
    }

public:
    struct ThreadParams {
        int epollFD { -1 };
        int server_socket { -1 };
        sockaddr serveraddr {};
        socklen_t len { 0 };

        int efd { -1 };
        std::vector<int> errors {};
    };

    // makes life easier with threads
    void operator()(int& e_fd, int timeout)
    {
        ServerLoop(e_fd, timeout);
    }

    // Setups things for the server and call ThreadLoop
    int ServerLoop(int& _efd, int timeout = -1)
    {
        epoll_event ev {};

        efd = _efd;
        int ret;
        int flags;

        // ========= setup =========
        if (CreateSock() != 0)
            return 1;

        if (EpollCreate() != 0)
            return 1;

        BindSock();

        ev.events = EPOLLIN | EPOLLET;

        // ev.data.fd = sock;
        m_original = std::make_shared<ClientDataStruct>();
        m_original->fd = server_socket;
        ClientDataStruct* orig = m_original.get();
        ev.data.ptr = orig;

        if (ret = listen(server_socket, listen_backlog); ret < 0) {
            perror("listen");
            close(server_socket);
            c_v = true;
            return EXIT_FAILURE;
        }

        std::cout << "Listening!\n";

        if (ret = epoll_ctl(epollFD, EPOLL_CTL_ADD, server_socket, &ev); ret < 0) {
            perror("epoll_ctl");
            close(server_socket);
            close(epollFD);
            c_v = true;
            return EXIT_FAILURE;
        }

        std::cout << "inserting eventfd for good measure to kill later"
                  << "\n";

        // ========= Add sentinel to epolls FD set =========
        epoll_event fd_event {};
        fd_event.events = EPOLLHUP | EPOLLET | EPOLLIN | EPOLLERR | EPOLLEXCLUSIVE;
        fd_event.data.ptr = new ClientDataStruct { .fd = _efd };
        if (ret = epoll_ctl(epollFD, EPOLL_CTL_ADD, _efd, &fd_event); ret < 0) {
            perror("epoll_ctl_sentinel");
            close(server_socket);
            close(epollFD);
            c_v = true;
            return EXIT_FAILURE;
        }

        std::cout << "Added listening socket to epoll set! \n";

        // Setting the servers socket to nonblocking mode
        // by editing it's flags through fcntl
        if (flags = fcntl(server_socket, F_GETFL); flags < 0) {
            perror("fcntl");
            close(server_socket);
            close(epollFD);
            c_v = true;
            return EXIT_FAILURE;
        }

        flags = flags | O_NONBLOCK;

        if (ret = fcntl(server_socket, F_SETFL, flags); ret < 0) {
            perror("fcntl f_setfl");
            close(server_socket);
            close(epollFD);
            c_v = true;
            return EXIT_FAILURE;
        }

        std::cout << "Listening in nonblocking mode now!\n";
        ThreadParams Params;

        /*
         *
    struct ThreadParams {
        int epollFD;
        int server_socket;
        sockaddr serveraddr;
        socklen_t len;
        int cli_sock;

         int efd;
    };
         */
        Params.epollFD = epollFD;
        Params.server_socket = server_socket;
        Params.serveraddr = reinterpret_cast<sockaddr&>(serveraddr);
        Params.len = len;
        Params.efd = efd;

        Params.errors.reserve(max_errnos);

        m_Threads.reserve(thread_num);
        for (size_t buckets = 0; buckets < thread_num; ++buckets) {
            auto Thread = std::thread(ThreadLoop, std::ref(Params), timeout, flags);
            m_Threads.emplace_back(std::move(Thread));
            std::this_thread::sleep_for(wait_tick_time);
        }

        auto serverRunningSleep = [&]() {
            while (!c_v) {
                std::this_thread::sleep_for(wait_tick_time);
        } };

        serverRunningSleep();

        for (size_t thrds = 0; thrds < thread_num; ++thrds) {
            write(efd, &Params, sizeof(Params));
            std::this_thread::sleep_for(wait_tick_time);
        }
        for (auto& buckets : m_Threads)
            buckets.join();
        close(server_socket);
        close(cli_sock);
        close(epollFD);
        if (Params.errors.size() > 1) {
            auto ErrnoMap = Li::common::ErrnoConsolidation(Params.errors);

            for (auto& ErrnoPair : ErrnoMap) {
                std::cout << "Errno: " << ErrnoPair.first << ", times: " << ErrnoPair.second << std::endl;
            }
        }
        delete (static_cast<ClientDataStruct*>(fd_event.data.ptr));
        std::flush(std::cout);
        return EXIT_SUCCESS;
    }

    // Loop for threads
    static void ThreadLoop(ThreadParams& Params, int timeout, int _flags)
    {

        int activated = 0;
        int thread_id = ++Running_Threads;

        // local buffer to read/write to
        char bf[recv_size + 1] = {};

        // Size of read content
        long n_data = 0;

        // HeartBeat-Counter in case timeout is not -1
        int cnt = 0;

        // epoll flags we receive and put back into
        int flags;

        // inform the User about the Thread number
        std::cout << "I am Thread #" << thread_id << ", successfully started!!\n";

        while (!c_v) {
            epoll_event events[max_epoll_events];
            int nfds = epoll_wait(Params.epollFD, events, max_epoll_events, timeout);
            if (nfds < 0) {
                perror("epoll_wait");
                std::cout << strerror(errno) << std::endl;
                return;
            }

            if (nfds == 0) {
                std::cout << "Server HeartBeat[1S]: " << cnt++ << std::endl;
            }

            for (int n = 0; n < nfds; ++n) {
                epoll_event ev {};
                auto n_event_ptr = [&events, &n]() -> ClientDataStruct* { return static_cast<ClientDataStruct*>(events[n].data.ptr); };
                bool cleanup_cur = false;
                std::scoped_lock cli_lock { n_event_ptr()->_struct_lock };
                // gather FD from client struct
                //std::scoped_lock<std::mutex> _guard { n_event_ptr()->_struct_lock };
                auto& tmpFD = n_event_ptr()->fd;
                if ((events[n].events & EPOLLIN) != 0x0) {
                    // ========= accepting clients =========

                    // ========= check for efd sentinel =========
                    if (Params.efd == tmpFD) {
                        // ========= stop all actions =========
                        c_v = true;
                        break;
                    }
                    if (tmpFD == Params.server_socket) {
                        n_event_ptr()->cli_sock = accept(Params.server_socket, (sockaddr*)&Params.serveraddr, &Params.len);
                        if (n_event_ptr()->cli_sock < 0) {
                            perror("accept");
                            c_v = true;
                            break;
                        }

                        if (flags = fcntl(n_event_ptr()->cli_sock, F_GETFL); flags < 0) {
                            perror("cli_sock_fcntl_getfl");
                            c_v = true;
                            break;
                        }

                        flags |= O_NONBLOCK;
                        if (fcntl(n_event_ptr()->cli_sock, F_SETFL, flags) < 0) {
                            perror("cli_sock_fcntl_setfl");
                            c_v = true;
                            break;
                        }

                        // ======== create client data! ========
                        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
                        ev.data.ptr = new ClientDataStruct { .fd = n_event_ptr()->cli_sock };

                        if (epoll_ctl(Params.epollFD, EPOLL_CTL_ADD, ((ClientDataStruct*)ev.data.ptr)->fd, &ev) == -1) {
                            perror("epoll_ctl_add_cli_addr");
                            c_v = true;
                            break;
                        }
                    } else if (tmpFD > 0) {
                        // ======== reading clients ========
                        n_data = read(tmpFD, bf, recv_size);
                        if (n_data < 0) { // READ ERROR
                            switch (errno) {
                            case (EAGAIN):
                                continue;
                            case (EBADF):
                            case (ECONNRESET):
                                cleanup_cur = true;
                                break;
                            default:
#ifdef DBG_READS
                                perror("read");
                                std::cout << "errno: " << errno << "\n";
#endif
#ifndef NERRNO_STATS
                                Params.errors.push_back(errno);
#endif
                            }
                        } else if (tmpFD > 0 && n_data == 0) { // client close
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
                if (n_event_ptr()->fd > 0 && (events[n].events & EPOLLOUT) && tmpFD != Params.server_socket) {
                    n_data = safe_write(tmpFD, (char*)"WRITE TIME BABY: ", 17);
#ifdef DBG_WRITE
                    std::cout << "TCP Server ready to write data!\n";
#endif
                } else if (events[n].events & EPOLLERR) {
                    std::cout << "TCP Server EPOLLERR Error!\n";
                    cleanup_cur = true;
                }

                // ========= Cleanup in case we marked it =========
                if (cleanup_cur) {
                    rm_fd(n_event_ptr(), &Params.epollFD);
                }

                std::flush(std::cout);
            }
        }

        std::cout << "Exiting Thread: " << thread_id << std::endl;
        --Running_Threads;

        return;
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

    static void rm_fd(ClientDataStruct* data, int* epollFD)
    {

        std::cout << "Client closed!\n";
        std::cout << "read from it: " << data->pos << " bytes!\n";
        std::cout << "latest read: " << data->buf << "\n";
        epoll_ctl(*epollFD, EPOLL_CTL_DEL, data->fd, nullptr);
    }

    static int safe_write(int& fd, char* buf, ssize_t count)
    {
        int return_value = 0;
        for (int n = 0; n < count;) {
            int ret = write(fd, (char*)buf + n, count - n);
            if (ret < 0) {
                if (errno == EINTR || errno == EAGAIN) {
                    pollfd wait_for[1];
                    wait_for[0] = { .fd = fd, .events = POLLOUT };
                    poll(wait_for, 1, 1);
                    continue; // try again
                }
                return_value = -1;
                break;
            } else {
                n += ret;
            }
        }

        return return_value;
    }
};

void flagFunc(int)
{
    c_v = true;
    return;
}

int main(/*int argc, char** argv*/)
{
    // ========= eventfd sentinel =========
    int efd;
    efd = eventfd(0, EFD_NONBLOCK);

    // ========= signal handling =========
    struct sigaction a {
    };
    a.sa_handler = flagFunc;
    a.sa_flags = 0;
    sigemptyset(&a.sa_mask);
    sigaction(SIGINT, &a, nullptr);
    signal(SIGPIPE, SIG_IGN); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast) -> cant influence this

    // ========= thread =========
    Srv Server;
    std::thread b = std::thread(std::ref(Server), std::ref(efd), -1);

    while (!c_v) {
        std::this_thread::sleep_for(wait_tick_time);
    }
    std::cout << "Killing... " << std::endl;

    // ========= cleanup thread =========
    std::this_thread::sleep_for(wait_tick_time);
    b.join();
    close(efd);

    return EXIT_SUCCESS;
}