#pragma once

#include <cstdint>
#include <vector>
#include <sys/poll.h>
#include "Socket.hpp"
#include "HttpProtocol.hpp"
// #include <functional>

class ConnectionManager {
	public:
		ConnectionManager();
		ConnectionManager(const ConnectionManager &) = default;
		ConnectionManager &operator=(const ConnectionManager &) = default;
		~ConnectionManager() = default;

		void	add_listener(uint16_t port);
		void	add_client(uint16_t port);

		std::vector<pollfd>	get_pfds();
		std::vector<Socket>	get_sockets();
		std::vector<HttpProtocol> get_handlers();

	private:
		std::vector<pollfd>	_pfds;
		std::vector<Socket>	_sockets;
		std::vector<HttpProtocol *>	_protocol_handlers;
};
