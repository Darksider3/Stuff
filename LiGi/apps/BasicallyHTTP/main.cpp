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

using size_t = std::size_t;
constexpr in_port_t port = 12312;
constexpr size_t max_buf_len = 8192;
constexpr int max_connections_per_socket = 10;
constexpr int enable_s = 1;
constexpr int disable_s = -1;

constexpr char DefaultPage[] = "<!DOCTYPE html><html><meta charset='utf-8'><head><title>Bye-bye baby bye-bye</title>\n"
                               "<style>body { background-color: #111 }\n"
                               "h1 { font-size:4cm; text-align: center; color: black;\n"
                               " text-shadow: 0 0 2mm red}</style></head>\n"
                               "<body><h1>Goodbye, world!</h1>\n"
                               "<form method=\"get\">\n"
                               "<label color='white'>Name:\n"
                               "<input name=\"submitted-name\" autocomplete=\"name\">\n"
                               "<input type='date' id='meeting-date' name='meeting-date'>"
                               "</label>\n"
                               "<button>Save</button>\n"
                               "</form>\n"
                               "</body></html>\r\n";

sig_atomic_t flag = false;
using StringMap = std::unordered_map<std::string, std::string>;

using Li::common::has;

void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &((reinterpret_cast<struct sockaddr_in*>(sa))->sin_addr);
    }

    return &((reinterpret_cast<struct sockaddr_in6*>(sa))->sin6_addr);
}
void flagFunc(int) // ignore sig - we gonna handle them all the same
{
    flag = true;
}

// clang-format off
template<typename T>
concept SocketAddrStore = std::is_same_v<std::remove_cvref_t<T>, sockaddr_in>
					   || std::is_same_v<std::remove_cvref_t<T>, sockaddr_in6>
					   || std::is_same_v<std::remove_cvref_t<T>, sockaddr_storage>;

// clang-format on

std::string urlencode(std::string_view in);
std::string urldecode(std::string_view in);

/*
sockaddr_in* asIncomingSocketAddress(addrinfo& s)
{
	return reinterpret_cast<struct sockaddr_in*>(s.ai_addr);
}
*/

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

    [[nodiscard]] ssize_t Write(const std::string& data) const
    {
        return Write(data.c_str(), data.length());
    }

    [[nodiscard]] ssize_t Write(const std::vector<char>& vec) const
    {
        return Write(&vec[0], vec.size());
    }

    ssize_t Write(const char* thing, const size_t len) const
    {
        return write(d->Sock, thing, len);
    }

    ssize_t Read(std::vector<char>& vec) const
    {
        return Read(&vec[0], vec.size());
    }

    ssize_t Read(char* thing, const size_t maxlen) const
    {
        return read(d->Sock, thing, maxlen);
    }

    AbstractConnection(const AbstractConnection&) = delete;
    AbstractConnection& operator=(AbstractConnection const&) = delete;
    AbstractConnection(AbstractConnection&& other) noexcept
    {
        this->d->Storage = std::move(other.Storage);
        this->d->Sock = { other.d->Sock };
    }
    AbstractConnection& operator=(AbstractConnection&&) = delete;
    virtual ~AbstractConnection()
    {
        Close();
    }
};

class ServerConnection {
};

/**
 * @brief The AbstractStatus class provides an base class for status codes, like the `Status404` class.
 */
class AbstractStatus {
private:
protected:
public:
    /**
     * @brief get returns the appropriate status code as a string
     * @return Statuscode as std::string
     */
    [[nodiscard]] virtual std::string get() const = 0;
    /**
	 * @brief operator std::string std::string conversion operator
	 * @return Same as `.get()`.
	 */
    virtual explicit operator std::string()
    {
        return get();
    }
    AbstractStatus() = default;
    AbstractStatus(const AbstractStatus&) = delete;
    AbstractStatus& operator=(AbstractStatus const&) = delete;
    AbstractStatus(AbstractStatus&&) = delete;
    AbstractStatus& operator=(AbstractStatus&&) = delete;
    virtual ~AbstractStatus() = default;
};

/**
 * @brief The Status200 class implements Status Code 200
 */
class Status200 : public AbstractStatus {
public:
    [[nodiscard]] std::string get() const override { return "200 OK"; }
};

/**
 * @brief The Status404 class implements Status Code 404
 */
class Status404 : public AbstractStatus {
public:
    [[nodiscard]] std::string get() const override { return "404 Not Found"; }
};

/**
 * @brief The ResponseBuilder class provides the interface for Responses
 */
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

struct HTTPClientResponse {
    std::string Version {};
    std::string Method {};
    std::string URI {};
    std::string Query {};

    std::unordered_map<std::string, std::string> Fields {};
};

using Map = std::unordered_map<std::string, std::string>;
int ToLower(unsigned const char& c)
{
    return std::tolower(c);
}

template<typename StrT = std::string>
class HTTPClientResponseBuilder {
private:
    static StrT TrimField(std::string_view in)
    {
        auto begin = in.begin();
        auto end = in.end();
        begin = std::find_if_not(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        end = std::find_if(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        return StrT(begin, end);
    }

    void SkipWhitespace(std::istream& in)
    {
        while (std::isspace(in.peek())) {
            in.ignore(1);
        }
    }

    /**
     * @brief Read HTTP Mehtod and Version from a stream, and return a accordingly filled HTTPClientResponse
     * @note This always should be given a "untouched" stream, e.g. always a stream set at position 0
     * @param std::istream& Inputstream to process
     * @return HTTPClientResponse Empty on failure, filled on success.
     */
    static HTTPClientResponse ReadHTTPMethodAndVersion(std::istream& in)
    {
        HTTPClientResponse response;
        std::string line;
        std::string tmp;
        if (in.tellg() > 0) {
            return {}; // @TODO: ERROR! Shall not pass in
                       // some stream in useless state
                       // because HTTP headers start in the first char...
        }

        // ORDER is important here! Ideally, we should be past
        // the HTTP verb(GET, POST, PATCH, etc) after this call
        // so dont mess up order!

        std::getline(in, line);
        if (line.empty()) {
            return {}; // @TODO: ERROR! Line MUST have words!
        }

        // Step 1: get the method
        auto begin = line.begin();
        auto end = line.end();
        begin = std::find_if_not(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });
        end = std::find_if(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        if (end == begin) {
            // matching error
        } else {
            // here we should have the method - store it!
            response.Method = std::string(begin, end);
        }

        // Step 2: get Destination URL

        // we recycle the begin as our end because it already points to the space behind the verb

        begin = end;
        end = line.end();

        begin = std::find_if_not(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        end = std::find_if(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        if (end == begin) {
            // @TODO: ERROR! somehow we read 0 size!
        } else {
            /*
			 * Because we already know where the URL begins, we just have to search for its end -
			 * which is also the beginning of the Query portion.
			 * So
			 * begin+end == whole URL
			 * begin+URI_end = /path/to/whatever
			 * URI_end+end = ?thisWhole=Section&of=Fluff
			*/
            auto URI_end = std::find_if(begin, end, [](char cur) -> bool {
                return cur == '?';
            });

            response.URI = std::string(begin, URI_end);

            // @TODO: Query parsing - not the right place to throw out the ?
            response.Query = std::string(URI_end, end);

            std::cout << "Split up URL and Query: URL: " << response.URI << ", Query: " << response.Query << "\n";
        }
        // Step 3: get HTTP-Version

        begin = end;
        end = line.end();

        begin = std::find_if_not(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        end = std::find_if(begin, end, [](char cur) -> bool {
            return std::isspace(cur);
        });

        if (end == begin) {
            // @TODO: ERROR! Still shouldn't be 0!
        } else {
            response.Version = std::string(begin, end);
        }

        return response;
    }

    /**
     * @brief Reads in HTTP fields - ideally called directly after HTTPReadVersionAndMethod - and maps them to a given StringMap
     * @param std::istream& Inputstream
     * @param Map& Map to write the fields in
     */
    static void ReadFields(std::istream& in, Map& map)
    {
        std::string line;
        std::getline(in, line);

        while (!line.empty() && line != "\r") /* In HTTP Header */ {
            auto [name, value] = Li::common::SplitPair(line, ':');
            if (name.empty() || value.empty()) {
                // @TODO: ERROR! Invalid HTTP Header Field!
            }

            name = TrimField(name);
            value = TrimField(value);

            // any field may also be just lowercase!
            std::transform(name.begin(), name.end(), name.begin(), ToLower);
            map[std::move(name)] = std::move(value);
            std::getline(in, line);
        }
    }

    /**
     * @brief Process possible HTTP body, return it as a string on success
     * @param std::istream& Inputstream to process
     * @param Map Map of strings to read relevant informations from
     * @param ssize_t max_streamlen  maximum length per body
     * @return std::string On success a string with a length greater than 0, otherwise a string with length 0.
     */
    static std::string processPossibleContent(std::istream& in, Map& map, ssize_t max_streamlen = max_buf_len)
    {
        std::string ret {};
        if (!has<StringMap, std::string>(map, "content-length"))
            return ret; // no content to possibly read

        ssize_t len = std::stol(map.at("content-length"));

        if (len > max_streamlen)
            return ret; // @TODO: ERROR! Size given is bigger then what we accept
        // @TODO: Shall we force the non-zero check? Actually i think the user should know that
        // when he wants to read that the content is bigger then 0...

        for (ssize_t i = 0; i < len && in.good(); ++i) {
            ret += static_cast<char>(in.get());
        }

        return ret;
    }

    /**
     *
     * @return
     */
    static StringMap parsedResponseBody()
    {
        StringMap ret {};
        return ret;
    }

public:
    /**
     * @brief Parses a request
     * @param std::string_view Input to parse
     * @return A filled HTTPClientResponse. Empty when not.
     */
    HTTPClientResponse parse(std::string_view in)
    {
        std::string constr { in };
        std::istringstream x(constr);
        auto r = ReadHTTPMethodAndVersion(x);
        ReadFields(x, r.Fields);

        std::cout << "Parsed! "
                  << "Method: " << r.Method << "\n"
                  << "URL: " << r.URI << "\n"
                  << "Version: " << r.Version << "\n"
                  << std::endl;

        for (auto& debug : r.Fields) {
            std::cout << "Field: " << debug.first << " "
                      << "Value: " << debug.second
                      << "\n";
        }

        if (r.Fields.find("content-length") != r.Fields.end() && std::stol(r.Fields.at("content-length")) > 0) {
            std::cout << "Input: " << processPossibleContent(x, r.Fields) << "\n";
        }
        // @TODO: FIELDS PARSING
        // @TODO: BODY PARSING
        return r;
    }
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

class ClientConnection : public AbstractConnection<ClientConnection> {
private:
    std::shared_ptr<HTTPResponseBuilder> m_OutResponse;

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

    std::string ReadUntilN(std::vector<char>& into, const ssize_t max = max_buf_len)
    {
        std::string ret;
        into.reserve(static_cast<size_t>(max));

        ssize_t bytes_received = 0;
        do {
            bytes_received = recv(Sock, &into[0], into.size(), 0);
            if (bytes_received == -1) { // error out
            } else {
                ret.append(into.begin(), into.end());
            }
        } while (bytes_received == max); // errored out!

        return ret;
    }

    std::shared_ptr<HTTPResponseBuilder> outResp()
    {
        return m_OutResponse;
    }

    ClientConnection(const ClientConnection&) = delete;
    ClientConnection& operator=(ClientConnection const&) = delete;
    ClientConnection& operator=(ClientConnection&&) = delete;
    ~ClientConnection() override
    {
        Close();
    }
};

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

            HTTPClientResponse resp = respB.parse(InputData);
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