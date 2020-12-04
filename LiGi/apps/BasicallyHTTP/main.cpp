#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
// socket include
#include <arpa/inet.h>
#include <iomanip>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

sockaddr_in* asIncomingSocketAddress(addrinfo& s)
{
	return reinterpret_cast<struct sockaddr_in*>(s.ai_addr);
}

template<typename T>
class AbstractConnection {
private:
	friend T;

	T* d = static_cast<T*>(this);
	const T& const_d = static_cast<const T&>(*this);

	AbstractConnection()
	{
	}

protected:
	std::shared_ptr<addrinfo> Sock;

public:
	sockaddr* asSocketAddress()
	{
		return reinterpret_cast<struct sockaddr*>(d->Sock->ai_addr);
	}

	sockaddr_in* asIncomingSocketAddress4()
	{
		return reinterpret_cast<struct sockaddr_in*>(d->Sock->ai_addr);
	}

	sockaddr_in6* asIncomingSocketAddress6()
	{
		return reinterpret_cast<struct sockaddr_in6*>(d->Sock->ai_addr);
	}

	void setSocket(sockaddr* s)
	{
		d->Sock = std::make_shared<addrinfo>();
		d->Sock->ai_addr = s;
	}
};

class ClientConnection : public AbstractConnection<ClientConnection> {
protected:
public:
	void printme()
	{
		std::cout << asIncomingSocketAddress4()->sin_port << std::endl;
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
