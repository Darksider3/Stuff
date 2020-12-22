#include <concepts>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
// socket include#include <stdio.h>
#include "LiGi/GeneralTools.h"
#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "AbstractConnection.h"
#include "ClientServerConnections.h"
#include "ConcreteStatuses.h"
#include "HTTPClientResponsesBuilder.h"
#include "Responses.h"

using size_t = std::size_t;
constexpr in_port_t port = 12312;
constexpr size_t max_buf_len = 4096;
constexpr int max_connections_per_socket = 10;
constexpr int enable_s = 1;
constexpr int disable_s = -1;

constexpr char DefaultPage[] = "<!DOCTYPE html><html><meta charset='utf-8'><head><title>Bye-bye baby bye-bye</title>\n"
                               "<style>body { background-color: #111 }\n"
                               "h1 { font-size:4cm; text-align: center; color: black;\n"
                               " text-shadow: 0 0 2mm red}</style></head>\n"
                               "<body><h1>Goodbye, world!</h1>\n"
                               "<form method=\"post\" enctype=\"multipart/form-data\">\n"
                               "<label color='white'>Name:\n"
                               "<input name=\"submitted-name\" autocomplete=\"name\">\n"
                               "<input name=\"uploadthing\" type=\"file\">"
                               "<input type='date' id='meeting-date' name='meeting-date'>"
                               "</label>\n"
                               "<button>Save</button>\n"
                               "</form>\n"
                               "</body></html>\r\n";

sig_atomic_t flag = false;

void flagFunc(int) // ignore sig - we gonna handle them all the same
{
    flag = true;
}

std::string urlencode(std::string_view in);
std::string urldecode(std::string_view in);
/*
sockaddr_in* asIncomingSocketAddress(addrinfo& s)
{
	return reinterpret_cast<struct sockaddr_in*>(s.ai_addr);
}
*/

/**
 * @brief The AcceptServer class uses the accept() system call to serve a socket listening
 */
class AcceptServer {
private:
    using HandlerMap = std::unordered_map<std::string, std::function<bool(ClientConnection&)>>;

    /// sin length - ipv6 sin pls
    socklen_t m_sin_l = sizeof(sockaddr_storage);

    /// Servers own address
    addrinfo hints {}, *m_serv_addr { nullptr };

    /// servers own port
    in_port_t m_serv_port {};

    /// servers actual Socket!
    int m_Sock {};

    /// Stores all connections established
    std::vector<ClientConnection> m_connections;

    /// Stores handlers registered for pathes
    std::unordered_map<std::string, std::function<bool(ClientConnection&)>> m_handlers;

    std::vector<char> m_tmp_buf {};

    /// Constate stores information used for setup the socket
    enum ConState {
        UNINITIALISED = 0,
        INITIALISED = 1,
        BOUND = 2,
        LISTENING = 4
    } m_ConState
        = UNINITIALISED;

    /**
	 * @brief setsigs sets up the signals so that on CTRL+C and such we are still able to unbind the socket
	 */
    static void setsigs()
    {
        struct sigaction a {
        };
        a.sa_handler = flagFunc;
        a.sa_flags = 0;
        sigemptyset(&a.sa_mask);
        sigaction(SIGINT, &a, nullptr);
        signal(SIGPIPE, SIG_IGN); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast) -> cant influence this
    }

    template<typename SingleArg>
    int EnableOpts(int& sockT, const SingleArg& opt)
    {
        return setsockopt(m_Sock, sockT, opt, &enable_s, sizeof(int));
    }

    template<typename FirstArg, typename... Args>
    void EnableOpts(int sockT, const FirstArg&& first, const Args&&... args)
    {
        EnableOpts(sockT, first);
        EnableOpts(sockT, args...);
    }

    void bufclear()
    {
        m_tmp_buf.clear(); // needed due to stupidness of holding it
        m_tmp_buf.reserve(max_buf_len);
        m_tmp_buf.resize(max_buf_len, 0x00);
    }

public:
    AcceptServer(AcceptServer&) = delete;
    AcceptServer(AcceptServer&&) = delete;
    AcceptServer(const AcceptServer&&) = delete;
    AcceptServer operator=(AcceptServer& other) = delete;
    AcceptServer operator=(AcceptServer&& other) = delete;

    explicit AcceptServer(const in_port_t p)
        : m_serv_port { p }
    {
        bufclear();
        setsigs();
        // hint
        memset(&hints, 0, sizeof hints);

        auto AcceptFunc = [this](ClientConnection& con) -> bool {
            HTTPClientResponseBuilder respB {};
            bufclear();
            con.Read(m_tmp_buf);
            auto clientIn = HTTPResponseBuilder(m_tmp_buf);
#ifdef HTTP_RAW_DBG
            std::cout << "RAW:"
                      << "\n---------------------------------------------------------------------\n"
                      << clientIn.getResponseOnly()
                      << "\n---------------------------------------------------------------------\n";
#endif
            auto g = con.getPeerName();
#ifdef HTTP_RAW_DBG
            std::cout << "got it!!!: " << g.first << ":" << g.second << "\n";
#endif
            std::string InputData(m_tmp_buf.begin(), m_tmp_buf.end());
            HTTPClientResponse resp = respB.parse(InputData, con);

            if (m_handlers.find(resp.URI) != m_handlers.end()) {
                m_handlers.at(resp.URI)(con);
            } else {
                if (has<HandlerMap, std::string>(m_handlers, "http_404")) {
                    std::cout << "Found 404 page serve \n";
                    m_handlers["http_404"](con);
                } else {
                    std::cout << "Didnt find 404 page serve\n";
                }
            }

            con.Close();
            return true;
        };
        m_handlers["http_accept"] = AcceptFunc;

        setupSocket();
        bindTo();
        Listen();
    }

    ~AcceptServer()
    {
        std::cout << "Usage exit: " << m_connections.size() << std::endl;
        unbind();
    }

    /**
	 * @brief setupSocket sets everything related to the socket itself up
	 * @param hintFam AF_* protocol
	 * @param hintSockT SOCK_* Type
	 * @param hintFlags AI_* Type
	 * @return True, when successfully set up the server, false when not
	 */
    bool setupSocket(const int hintFam = AF_UNSPEC, const int hintSockT = SOCK_STREAM, const int hintFlags = AI_PASSIVE)
    {
        if (m_ConState != UNINITIALISED) {
            m_connections.clear();
            close(m_Sock);
        }
        hints.ai_family = hintFam;
        hints.ai_socktype = hintSockT;
        hints.ai_flags = hintFlags; // fill my ip!

        if (getaddrinfo(nullptr, std::string(std::to_string(m_serv_port)).c_str(), &hints, &m_serv_addr) != 0) {
            perror("getaddrinfo");
        }

        m_Sock = socket(m_serv_addr->ai_family, m_serv_addr->ai_socktype, m_serv_addr->ai_protocol);
        if (m_Sock == -1) {
            perror("Couldnt set socket");
            return false;
        }
        m_ConState = INITIALISED;
        EnableOpts(SOL_SOCKET, SO_REUSEADDR, SO_REUSEPORT);
        int err = setsockopt(m_Sock, IPPROTO_IPV6, IPV6_V6ONLY, &disable_s, sizeof(int));
        if (err != 0) {
            perror("setsockopt ipv_v6only-disable didnt work");
            return false;
        }

        return true;
    }

    /**
	 * @brief bindTo binds the socket to a specific address
	 * @return True when bind was successfull, false when not
	 */
    bool bindTo()
    {
        if (m_ConState != INITIALISED)
            return false;

        if (bind(m_Sock, (const sockaddr*)m_serv_addr->ai_addr, m_serv_addr->ai_addrlen) == -1) {
            perror("Couldn't bind! ");
            exit(EXIT_FAILURE);
        }

        m_ConState = BOUND;
        freeaddrinfo(m_serv_addr);
        return true;
    }

    /**
	 * @brief unbind the socket
	 */
    void unbind()
    {
        if (m_ConState < INITIALISED) {
            return;
        }

        close(m_Sock);
    }

    /**
	 * @brief Listen setups the listener on the socket setup earlier
	 * @param max_connections
	 * @return True, when setting up the listening worked, false when not
	 */
    bool Listen(const int max_connections = max_connections_per_socket)
    {
        // requiring being bound before listening
        if (m_ConState != BOUND) {
            return false;
        }
        if (listen(m_Sock, max_connections) != 0) {
            perror("Couldn't bind: ");
            return false;
        }

        m_ConState = LISTENING;
        return true;
    }

    /**
         *  RegisterResponseHandler registers function handlers for specific routes
         * @param Functor Function to be called on route
         * @param route Route to function
         */
    void RegisterResponseHandler(std::function<bool(ClientConnection&)>&& Functor, std::string_view route)
    {
        m_handlers[std::string(route)] = std::move(Functor);
    }

    /**
	 * @brief runAccept runs accept() in a loop and calling the "accept" routine
	 */
    void runAccept()
    {
        while (!flag) {
            sockaddr_storage cli {};
            int cli_fd = accept(m_Sock, reinterpret_cast<sockaddr*>(&cli), &m_sin_l);

            if (cli_fd == -1)
                perror("Couldn't accept peer: ");
            if (m_handlers.find("http_accept") == m_handlers.end())
                return;

            auto handle = m_handlers["http_accept"];
            ClientConnection con(cli_fd, cli);
            m_connections.push_back(std::move(con));
            if (handle(con))
                m_connections.pop_back();
        }
    }
};

/**
 * @brief The EpollServer class uses Epoll under the hood
 */
class EpollServer {
public:
    static void doSmthing()
    {
    }
};

int main()
{
    auto SlashRoute = [](ClientConnection& con) -> bool {
        auto Out = con.outResp();
        Out->append(&DefaultPage[0]);
        Out->setStatus(Status200());
        if (con.Write(Out->get()) == -1)
            perror("write: ");
        return true;
    };

    auto FourZeroFour = [](ClientConnection& con) -> bool {
        auto Out = con.outResp();
        Out->append("<html><head><title>404</title></head><body><h1>404</h1></body></html>");
        Out->setStatus(Status404());
        if (con.Write(Out->get()) == -1)
            perror("write: ");

        return true;
    };
    AcceptServer ss { port };
    ss.RegisterResponseHandler(SlashRoute, "/");
    ss.RegisterResponseHandler(FourZeroFour, "http_404");
    ss.runAccept();
    ss.~AcceptServer();
    exit(EXIT_SUCCESS);
}