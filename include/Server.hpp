#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <sys/poll.h>
#include <vector>
#include "HttpServer.hpp"
#include "Socket.hpp"

class Server
{
public:
	//Default Constructors
	Server();
	Server(const Server &) = default;
	Server &operator=(const Server &) = default;
	~Server() = default;

	//Custom constructors
	Server(uint16_t port);
	Server(std::vector<uint16_t> ports);

	//Methods
	void 						handle_events();
	int 						poll();

	bool						ready_to_read(short revents);
	bool						ready_to_write(short revents);
	bool						error_occurred(short revents);

	std::vector<pollfd>& 		get_pfds();
	std::vector<Socket>& 		get_sockets();

private:
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

	typedef typename std::map<std::reference_wrapper<const Socket>, HttpServer> SocketRef_HttpServer_map;
	
	std::vector<pollfd> 			_pfds;
	std::vector<Socket> 			_sockets;

	// Because you cannot have a STL-container which stores references we use this wrapper class.
	SocketRef_HttpServer_map		_server_instances;



	int							_poll_events();
	void 						_add_client(Socket s);
	void						_client_remove(int index);
};
