#pragma once

#include <cstdint>
#include <vector>
#include <sys/poll.h>
#include "Server.hpp"
#include "Socket.hpp"
#include "HttpProtocol.hpp"
// #include <functional>

class ConnectionManager {
	public:
		ConnectionManager();
		ConnectionManager(const ConnectionManager &) = default;
		ConnectionManager &operator=(const ConnectionManager &) = default;
		~ConnectionManager() = default;

		void	add(const Server &, Socket);
		void 	remove(const Server &, Socket &);

		std::vector<pollfd>			get_pfds();
		std::vector<Socket>			get_sockets();
		std::vector<HttpProtocol>	get_handlers();

	private:
		std::vector<pollfd>	_pfds;
		std::vector<Socket>	_sockets;
		// TODO BaseProtocol
		std::vector<HttpProtocol *>	_protocol_handlers;
};

