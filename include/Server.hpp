#pragma once

#include <cstdint>
#include <sys/poll.h>
#include <utility>
#include <vector>
#include "HttpServer.hpp"
#include "Socket.hpp"
#include <memory>

class Server
{
public:
	//Default Constructors
	Server() = delete;
	Server(const Server &);
	Server &operator=(const Server &) = default;
	~Server() = default;

	//Custom constructors
	Server(uint16_t port);
	Server(std::vector<uint16_t> ports);

	//Methods
	void 										handle_events();
	int 										poll();

	bool										ready_to_read(short revents);
	bool										ready_to_write(short revents);
	bool										error_occurred(short revents);
	bool										should_exit(){return _exit_server;};

	std::vector<pollfd>& 						get_pfds();
	const std::vector<Socket>					get_sockets() const;

private:
	std::vector<pollfd> 						_pfds;
	std::vector<std::pair<Socket, std::shared_ptr<HttpServer>>>	_sockets;
	bool										_exit_server;

	int											_poll_events();
	void 										_add_client(Socket s);
	void										_client_remove(int index);

	// TODO Maybe put all of this shit in somekind of clients container for e.x
	// in this case all the data inclient container will be sequential so we can still call.
	// `poll()` with `ClientContainer.get_pfds()`
	// class ClientContainer
	// {
	// std::vector<pollfd> 		_pfds;
	// std::vector<Socket> 		_sockets;
	// std::vector<HttpServer> 	_http_servers;
	//
	// public get_pfds();
	// }

	// This class will just have refenerces to that ClientContainer content at a certain index.
	// class Client
	// {
	// 	public Client(socket &s, pollfd &pfd, ServerInstance &server)
	// }

};



std::ostream& operator<< (std::ostream& os, const Server& rhs);
