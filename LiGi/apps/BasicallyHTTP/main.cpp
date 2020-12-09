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

constexpr size_t max_buf_len = 4096;
constexpr int max_connections_per_socket = 10;
constexpr int enable_s = 1;
constexpr int disable_s = -1;

sig_atomic_t flag = false;

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

sockaddr_in* asIncomingSocketAddress(addrinfo& s)
{
	return reinterpret_cast<struct sockaddr_in*>(s.ai_addr);
}

bool hasCLRFEnd(std::string_view where)
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

	std::pair<std::string, in_port_t> getPeerName(sockaddr_storage* s)
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
	void setSock(int* s)
	{
		d->Sock = *s;
	}

	static AbstractConnection<T>&& Open(int domain, int type, int proto)
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

	void Write(std::vector<char>& vec)
	{
		return Write(&vec[0], vec.size());
	}

	void Write(char* thing, size_t len)
	{
		write(Sock, thing, len);
	}

	void Read(std::vector<char>& vec)
	{
		return Read(&vec[0], vec.size());
	}

	void Read(char* thing, size_t maxlen)
	{
		if ((read(Sock, thing, maxlen)) == -1) {
			perror("Read");
		}
	}

	virtual ~AbstractConnection()
	{
		Close();
	}
};

class ClientConnection : public AbstractConnection<ClientConnection> {
protected:
public:
	ClientConnection() = delete;
	ClientConnection(int& S, sockaddr_storage& stor)
	{
		setSock(&S);
		setInfo(stor);
	}

	void printme()
	{
		auto info = getPeerName(this->d->Storage.get());
		std::cout << "printme() Port: " << info.second << "\n"
				  << "printme() Addr: " << info.first << std::endl;
	}

	std::string ReadUntilN(std::vector<char>& into, ssize_t max = max_buf_len)
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

class ServerConnection {
};

class ResponseBuilder {
};

/**
 * @brief The AcceptServer class uses the accept() system call to serve a socket listening
 */
class AcceptServer {
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
	void EnableOpts(int sockT, const FirstArg&& first, const Args&&... args)
	{
		EnableOpts(sockT, first);
		EnableOpts(sockT, args...);
	}

public:
	AcceptServer(const in_port_t p)
		: m_serv_port { p }
	{
		m_tmp_buf.reserve(max_buf_len);
		m_tmp_buf.resize(max_buf_len, 0x00);
		setsigs();
		// hint
		memset(&hints, 0, sizeof hints);

		auto AcceptFunc = [this](ClientConnection& con) -> bool {
			std::string d {};
			con.Read(m_tmp_buf);
			d.append(m_tmp_buf.begin(), m_tmp_buf.end());
			char response[] = "HTTP/1.1 200 OK\r\n"
							  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
							  "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
							  "<style>body { background-color: #111 }"
							  "h1 { font-size:4cm; text-align: center; color: black;"
							  " text-shadow: 0 0 2mm red}</style></head>"
							  "<body><h1>Goodbye, world!</h1></body></html>\r\n";
			con.Write(response, sizeof(response) - 1);
			std::cout << "data:\n------\n "
					  << d << " \n------\n";

			auto g = con.getPeerName();
			std::cout << "got it!!!: " << g.first << ":" << g.second << "\n";
			con.Close();
			return true;
		};
		m_handlers["accept"] = AcceptFunc;

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
	bool setupSocket(int hintFam = AF_UNSPEC, int hintSockT = SOCK_STREAM, int hintFlags = AI_PASSIVE)
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
	bool Listen(int max_connections = max_connections_per_socket)
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

	void RegisterResponseHandler(std::function<void(ClientConnection&, int)> f);

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
			if (m_handlers.find("accept") == m_handlers.end())
				return;

			auto handle = m_handlers["accept"];
			ClientConnection con(cli_fd, cli);
			m_connections.emplace_back(con);
			if (handle(con))
				m_connections.pop_back();
		}
	}
};

class AbstractResponse {
private:
	std::string m_Response;

public:
	void send();
};

/**
 * @brief The EpollServer class uses Epoll under the hood
 */
class EpollServer {
};

char response[] = "HTTP/1.1 200 OK\r\n"
				  "Content-Toperator>>ype: text/html; charset=UTF-8\r\n\r\n"
				  "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
				  "<style>body { background-color: #111 }"
				  "h1 { font-size:4cm; text-align: center; color: black;"
				  " text-shadow: 0 0 2mm red}</style></head>"
				  "<body><h1>Goodbye, world!</h1></body></html>\r\n";
int main()
{

	AcceptServer ss { 12312 };
	ss.runAccept();
	ss.~AcceptServer();
	exit(EXIT_SUCCESS);
}
