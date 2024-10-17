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

		void	add(t_config config, Socket);
		void 	remove(size_t index);

		void	add_listeners(std::vector<t_config> &configs);

		std::vector<pollfd>			get_pfds();
		std::vector<Socket>			get_sockets();
		std::unordered_map<int, HttpProtocol *>	get_handlers();

	private:
		std::vector<pollfd>	_pfds;
		std::vector<Socket>	_sockets;
		// TODO BaseProtocol
		std::unordered_map<int, HttpProtocol *>	_protocol_map;

		// std::vector<HttpProtocol *>	_protocol_handlers;
};
