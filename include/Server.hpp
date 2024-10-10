#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <sys/poll.h>
#include <vector>
#include "ConnexxionManager.hpp"
#include "HttpProtocol.hpp"
#include "Socket.hpp"
#include <memory>
#include <algorithm>

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
		Server(t_config &config, ConnectionManager &cm);

		//Methods
		void 													handle_events();
		int 													poll();

		bool													ready_to_read(short revents);
		bool													ready_to_write(short revents);
		bool													error_occurred(short revents);
		bool													should_exit(){return _exit_server;};

		std::vector<pollfd>& 									get_pfds();
		const std::vector<Socket>& 								get_sockets() const;

	private:
		/**
		 *	_fd_map		: A map of file descriptor keys, with the value being shared pointers to HttpServer objects.
		 *	_pfds		: A vector of the poll fd objects used in the server loop.
		 *	_sockets	: A vector of all active sockets. Is somewhat linked to _pfds.
		 *	_exit_server: Boolean saying if the server should exit.
		 *	_config		: Struct with the servers configuration.
		 **/
		std::unordered_map<int, HttpProtocol *>	_httpserver_map;
		std::vector<pollfd> 									_pfds;
		std::vector<Socket> 									_sockets;
		bool													_exit_server;
		t_config												_config;

		int														_poll_events();
		void 													_add_client(Socket s);
		void													_client_remove(int index);

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

std::ostream& operator<< (std::ostream& stream, const Server& rhs);
