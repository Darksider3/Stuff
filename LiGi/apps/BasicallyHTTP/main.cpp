#include <concepts>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
// socket include#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iomanip>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "LiGi/GeneralTools.h"

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
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
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

sockaddr_in* asIncomingSocketAddress(addrinfo& s)
{
	return reinterpret_cast<struct sockaddr_in*>(s.ai_addr);
}

bool hasCLRFEnd(std::string_view const where)
{
	size_t start = where.length() - 4; // CLRF*2+1
	if (where[start] != '\r')
		return false;
	if (where[start + 1] != '\n')
		return false;
	if (where[start + 2] != '\r')
		return false;
	if (where[start + 3] != '\n')
		return false;

	return true;
}

template<typename T>
class AbstractConnection {
private:
	friend T;

	T* d = static_cast<T*>(this);
	const T& const_d = static_cast<const T&>(*this);

	AbstractConnection()
	{
		d->Storage = std::make_shared<sockaddr_storage>();
	}

	std::shared_ptr<sockaddr_storage> Storage;
	int Sock;

public:
	bool is_alive() const { return Sock != -1; }

	std::pair<std::string, in_port_t> getPeerName()
	{
		if (!d->Storage)
			return {};
		return getPeerName(&reinterpret_cast<sockaddr_storage&>(*d->Storage.get()));
	}

	std::pair<std::string, in_port_t> getPeerName(const sockaddr_storage* s)
	{
		char ipstr[INET6_ADDRSTRLEN];
		in_port_t port;
		inet_ntop(d->Storage.get()->ss_family,
			get_in_addr((struct sockaddr*)d->Storage.get()),
			ipstr, sizeof ipstr);
		if (s->ss_family == AF_INET6) { // ipv6
			struct sockaddr_in6* ad = reinterpret_cast<sockaddr_in6*>(&s);
			port = ntohs(ad->sin6_port);
		} else { // ipv4
			struct sockaddr_in* ad = reinterpret_cast<struct sockaddr_in*>(&s);
			port = ntohs(ad->sin_port);
		}

		return { ipstr, port };
	}

	template<SocketAddrStore F>
	void setInfo(F& s)
	{
		memcpy(reinterpret_cast<unsigned char*>(d->Storage.get()),
			reinterpret_cast<unsigned char*>(&s), sizeof(F));
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

	ssize_t Write(const std::string& data)
	{
		return Write(data.c_str(), data.length());
	}

	ssize_t Write(const std::vector<char>& vec) const
	{
		return Write(&vec[0], vec.size());
	}

	ssize_t Write(const char* thing, const size_t len) const
	{
		return write(Sock, thing, len);
	}

	ssize_t Read(std::vector<char>& vec)
	{
		return Read(&vec[0], vec.size());
	}

	ssize_t Read(char* thing, const size_t maxlen)
	{
		return read(Sock, thing, maxlen);
	}

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
	virtual const std::string get() const = 0;
	/**
	 * @brief operator std::string std::string conversion operator
	 * @return Same as `.get()`.
	 */
	virtual operator std::string()
	{
		return get();
	}
	virtual ~AbstractStatus() = default;
};

/**
 * @brief The Status200 class implements Status Code 200
 */
class Status200 : public AbstractStatus {
public:
	const std::string get() const override { return "200 OK"; }
};

/**
 * @brief The Status404 class implements Status Code 404
 */
class Status404 : public AbstractStatus {
public:
	const std::string get() const override
	{
		return "404 Not Found";
	}
};

/**
 * @brief The ResponseBuilder class provides the interface for Responses
 */
class ResponseBuilder {
private:
protected:
public:
	ResponseBuilder() {};
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
	virtual size_t length() const = 0;
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

	static HTTPClientResponse ReadHTTPMethodAndVersion(std::istream& in)
	{
		HTTPClientResponse response;
		std::string line, tmp;
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
				if (cur == '?')
					return true;
				return false;
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

	static StringMap parsedResponseBody()
	{
		StringMap ret {};
		return ret;
	}

public:
	HTTPClientResponse parse(const std::string&& in)
	{
		std::istringstream x(in);
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
protected:
	std::string m_Resp {};
	std::string m_Status {};
	std::string m_Content_Type { "text/html" };
	std::string m_Charset { "charset=UTF-8" };

	std::string m_out {};

	constexpr std::string_view CLRF() { return "\r\n"; }

public:
	/**
	 * @brief HTTPResponseBuilder Response-content and Status Code as string constructor
	 * @param str Contents of the response
	 * @param Status status code(as a string)
	 */
	HTTPResponseBuilder(const std::string_view str, const std::string Status = "200 OK")
		: m_Resp { str }
		, m_Status { Status }
	{
	}

	/**
	 * @brief HTTPResponseBuilder vector-copy ctor providing an easy mechanism to input clients requests
	 * @param c
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

	void clear()
	{
		m_Resp.clear();
	}

	/**
	 * @brief getResponseOnly just returns the response without the headers
	 * @return
	 */
	std::string_view getResponseOnly()
	{
		return m_Resp;
	}

	size_t length() const override { return m_Resp.length(); }
};

class ClientConnection : public AbstractConnection<ClientConnection> {
protected:
public:
	std::shared_ptr<HTTPResponseBuilder> outResp;

	ClientConnection() = delete;
	ClientConnection(const int& S, sockaddr_storage& stor)
	{
		setSock(&S);
		setInfo(stor);
		outResp = std::make_shared<HTTPResponseBuilder>();
	}

	void printme()
	{
		auto info = getPeerName(this->d->Storage.get());
		std::cout << "printme() Port: " << info.second << "\n"
				  << "printme() Addr: " << info.first << std::endl;
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

	virtual ~ClientConnection()
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

protected:
	/// sin length - ipv6 sin pls
	socklen_t m_sin_l = sizeof(sockaddr_storage);

	/// Servers own address
	addrinfo hints, *m_serv_addr;

	/// servers own port
	in_port_t m_serv_port;

	/// servers actual Socket!
	int m_Sock;

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
	void setsigs()
	{
		struct sigaction a;
		a.sa_handler = flagFunc;
		a.sa_flags = 0;
		sigemptyset(&a.sa_mask);
		sigaction(SIGINT, &a, NULL);
		signal(SIGPIPE, SIG_IGN);
	}

	template<typename SingleArg>
	int EnableOpts(const int& sockT, const SingleArg& opt)
	{
		return setsockopt(m_Sock, sockT, opt, &enable_s, sizeof(int));
	}

	template<typename FirstArg, typename... Args>
	void EnableOpts(const int sockT, const FirstArg&& first, const Args&&... args)
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
	AcceptServer(const in_port_t p)
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

			HTTPClientResponse resp = respB.parse(std::move(InputData));
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

	virtual ~AcceptServer()
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

		getaddrinfo(NULL, std::string(std::to_string(m_serv_port)).c_str(), &hints, &m_serv_addr);

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

	void RegisterResponseHandler(const std::function<bool(ClientConnection&)>&& f, std::string_view route)
	{
		m_handlers[std::string(route)] = std::move(f);
	}

	/**
	 * @brief runAccept runs accept() in a loop and calling the "accept" routine
	 */
	void runAccept()
	{
		while (!flag) {
			sockaddr_storage cli;
			int cli_fd = accept(m_Sock, reinterpret_cast<sockaddr*>(&cli), &m_sin_l);

			if (cli_fd == -1)
				perror("Couldn't accept peer: ");
			if (m_handlers.find("http_accept") == m_handlers.end())
				return;

			auto handle = m_handlers["http_accept"];
			ClientConnection con(cli_fd, cli);
			m_connections.emplace_back(con);
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
		con.outResp->append(DefaultPage);

		con.outResp->setStatus(Status200());
		con.Write(con.outResp->get());
		std::cout << "Was here!" << std::endl;
		return true;
	};

	auto FourZeroFour = [](ClientConnection& con) -> bool {
		con.outResp->append("<html><head><title>404</title></head><body><h1>404</h1></body></html>");
		con.outResp->setStatus(Status404());
		con.Write(con.outResp->get());

		return true;
	};
	AcceptServer ss { 12312 };
	ss.RegisterResponseHandler(SlashRoute, "/");
	ss.RegisterResponseHandler(FourZeroFour, "http_404");
	ss.runAccept();
	ss.~AcceptServer();
	exit(EXIT_SUCCESS);
}
