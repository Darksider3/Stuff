#include <concepts>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
// socket include
#include <arpa/inet.h>
#include <iomanip>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

constexpr int max_connections_per_socket = 10;
constexpr int enable_socket_reuse = 1;
sig_atomic_t flag = false;

void flagFunc(int) // ignore sig - we gonna handle them all the same
{
	flag = true;
}

template<typename T>
concept SocketAddrStore = std::is_same_v<std::remove_cvref_t<T>, sockaddr_in> || std::is_same_v<std::remove_cvref_t<T>, sockaddr_in6> || std::is_same_v<std::remove_cvref_t<T>, sockaddr_storage>;

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
		//d->Socket = std::make_shared<int>();
	}

	virtual ~AbstractConnection()
	{
		//close(*d->Socket);
	}

protected:
	std::shared_ptr<sockaddr_storage> Storage;
	int FD;
	bool alive = false;

public:
	bool is_alive() const { return alive; }

	template<SocketAddrStore F>
	void setInfo(F* s)
	{
		memcpy(reinterpret_cast<unsigned char*>(d->Storage.get()),
			reinterpret_cast<unsigned char*>(&s), sizeof(F));
	}

	std::pair<std::string, in_port_t> getPeerName(int& s)
	{
		char ipstr[INET6_ADDRSTRLEN];
		socklen_t len;
		sockaddr_storage addr;
		in_port_t port;
		len = sizeof(addr);
		getpeername(s, reinterpret_cast<struct sockaddr*>(&addr), &len);
		setInfo(&addr);
		if (addr.ss_family == AF_INET) { // ipv4
			struct sockaddr_in* ad = reinterpret_cast<struct sockaddr_in*>(&addr);
			port = ntohs(ad->sin_port);
			inet_ntop(AF_INET, &ad->sin_addr, ipstr, sizeof(ipstr));
		} else { // ipv6
			struct sockaddr_in6* ad = reinterpret_cast<sockaddr_in6*>(&addr);
			port = ntohs(ad->sin6_port);
			inet_ntop(AF_INET6, &ad->sin6_addr, ipstr, sizeof(ipstr));
		}

		return { ipstr, port };
	}

	void test()
	{
	}
};

class ClientConnection : public AbstractConnection<ClientConnection> {
protected:
public:
	void printme()
	{
		char ipstr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &d->Storage, ipstr, sizeof(ipstr));
		std::cout << "printme() Port: " << ntohs(reinterpret_cast<sockaddr_in6*>(&d->Storage)->sin6_port) << "\n"
				  << "printme() Addr: " << ipstr << std::endl;
	}
};

class ServerConnection {
};

class ResponseBuilder {
};

class Server {
protected:
	// Stores all connections established
	std::vector<ClientConnection> m_connections;

	// Stores handlers registered for pathes
	std::map<std::string, std::function<void(void)>> m_handlers;

public:
	void bindTo(int&& p);
	void RegisterResponseHandler(std::function<void(void)> f);
	void run();
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
	ClientConnection f;
	char buff[2048] = "";
	int client_fd;
	struct sockaddr_in server_addr, cli_addr;
	socklen_t sin_len = sizeof(cli_addr);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		//error
	}

	int port = 8080;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		close(sock);
		//error
	}

	listen(sock, 5);
	std::cout << "Serving on http://localhost:" << port << "\n";
	while (true) {

		client_fd = accept(sock, (struct sockaddr*)&cli_addr, &sin_len);
		f.setSocket((struct sockaddr*)&cli_addr);
		f.printme();
		printf("Connection established \n");
		if (client_fd == -1) {
			perror("Can't accept");
			continue;
		}

		write(client_fd, response, sizeof(response) - 1);
		close(client_fd);
	}
}
