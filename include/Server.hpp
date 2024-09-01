#pragma once

#include <cstdint>
#include <sys/poll.h>
#include <vector>
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
	int							poll_events();

	bool						is_client(int fd);
	bool						is_listener(int fd);
	bool						ready_to_read(short revents);
	bool						ready_to_write(short revents);
	bool						error_occurred(short revents);

	std::vector<pollfd>& 		get_pfds();
	std::vector<Socket>& 		get_sockets();

private:
	std::vector<pollfd> 		_pfds;
	std::vector<Socket> 		_sockets;
	// HttpServer				_http_server; //Psst, will be ServerImplementation at some point!!!


	void 						_socket_add(Socket s);
	void						_socket_remove(int index);
};
