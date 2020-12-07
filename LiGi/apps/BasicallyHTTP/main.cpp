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

constexpr int max_connections_per_socket = 10;
constexpr int enable_socket_reuse = 1;
constexpr in_port_t default_port = 8080;

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

protected:
	std::shared_ptr<sockaddr_storage> Storage;
	int Sock = -1;
	bool alive = false;

public:
	bool is_alive() const { return alive; }

	int& getSock()
	{
		return d->Sock;
	}

	template<SocketAddrStore F>
	void setInfo(F& s)
	{
		memcpy(reinterpret_cast<unsigned char*>(d->Storage.get()),
			reinterpret_cast<unsigned char*>(&s), sizeof(F));
	}

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

	void setSock(int* s)
	{
		d->Sock = *s;
	}

	void test()
	{
	}

	void Close()
	{
		if (d->Sock != -1)
			close(d->Sock);
		d->Sock = -1;
	}

	virtual ~AbstractConnection()
	{
		Close();
	}
};

class ClientConnection : public AbstractConnection<ClientConnection> {
protected:
public:
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
};

class ServerConnection {
};

class ResponseBuilder {
};

class Server {
protected:
	constexpr static int enable_s = 1;

	// sin length - ipv6 sin pls
	socklen_t m_sin_l = sizeof(sockaddr_storage);

	// Servers own address
	addrinfo hints, *m_serv_addr;

	// servers own port
	in_port_t m_serv_port;

	// servers actual Socket!
	int m_Sock;

	// Stores all connections established
	std::vector<ClientConnection> m_connections;

	// Stores handlers registered for pathes
	std::unordered_map<std::string, std::function<void(ClientConnection&)>> m_handlers;

	// state!

	enum ConState {
		UNINITIALISED = 0,
		INITIALISED = 1,
		BOUND = 2,
		LISTENING = 4
	} m_ConState
		= UNINITIALISED;

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
	void EnableOpts(SingleArg opt)
	{
		std::cout << "enabled opt" << std::endl;
		setsockopt(Sock, SOL_SOCKET, opt, &enable_socket_reuse, sizeof(int));
	}

	template<typename FirstArg, typename... Args>
	void EnableOpts(FirstArg first, Args... args)
	{
		EnableOpts(first);
		EnableOpts(args...);
	}

public:
	Server()
	{
		setsigs();
		Sock = socket(AF_INET6, SOCK_STREAM, 0);
		EnableOpts(SO_REUSEADDR, SO_REUSEPORT);
	}

	~Server()
	{
		close(Sock);
	}

	void bindTo(int&& p);
	void RegisterResponseHandler(std::function<void(void)> f);
	void run()
	{
	}
};

class AbstractResponse {
private:
	std::string m_Response;

public:
	void send();
};
char response[] = "HTTP/1.1 200 OK\r\n"
				  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
				  "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
				  "<style>body { background-color: #111 }"
				  "h1 { font-size:4cm; text-align: center; color: black;"
				  " text-shadow: 0 0 2mm red}</style></head>"
				  "<body><h1>Goodbye, world!</h1></body></html>\r\n";
int main()
{

	Server ss { 12312 };
	ss.bindTo();
	ss.Listen();
	ss.runAccept();
	char buff[2048] = "";
	int client_fd;
	struct sockaddr_in6 server_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);

	int sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (sock < 0) {
		//error
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable_socket_reuse, sizeof(int));
	setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &enable_socket_reuse, sizeof(int));

	in_port_t port = 8080;
	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_addr = in6addr_any;
	server_addr.sin6_port = htons(port);

	if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		close(sock);
		//error
	}

	listen(sock, max_connections_per_socket);
	std::cout << "Serving on http://localhost:" << port << "\n";

	while (!flag) {
		//ClientConnection f {sock, cli_addr};
		client_fd = accept(sock, (struct sockaddr*)&cli_addr, &sin_len);

		if (client_fd == -1) {
			perror("Can't accept");
			return errno;
		}

		//f.setSock(&client_fd);

		printf("Connection established \n");

		read(client_fd, buff, 2048);

		if (hasCLRFEnd(buff))
			std::cout << "found end!" << std::endl;

		write(client_fd, response, sizeof(response) - 1);
	}

	close(sock);
	ss.~Server();
	exit(EXIT_SUCCESS);
}
