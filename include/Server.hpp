#pragma once

#include <cstdint>
#include <sys/poll.h>
#include <vector>
#include "Socket.hpp"

class Server
{
public:
	Server();
	Server(const Server &) = default;
	Server &operator=(const Server &) = default;
	~Server() = default;


	Server(uint16_t port);
	Server(std::vector<uint16_t> ports);


	void handle_events();

	const std::vector<int>& getListenFds() const;
	std::vector<pollfd>& getFds();

private:
	std::vector<pollfd> 	_pfds;
	std::vector<Socket> 	_sockets;
	// HttpServer				_http_server; //Psst, will be ServerImplementation at some point!!!

	int 	_socket_create();
	bool 	_socket_bind(int fd, uint16_t port);
	int 	_socket_accept(int fd);
};
