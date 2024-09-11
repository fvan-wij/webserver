#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <sys/poll.h>
#include <vector>
#include "HttpServer.hpp"
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

	Server(Server &&) = delete;
	Server &operator=(Server &&) = delete;

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
	const std::vector<Socket>& 					get_sockets() const;

private:
	std::unordered_map<int, std::shared_ptr<HttpServer>>	_fd_map;
	std::vector<pollfd> 									_pfds;
	std::vector<Socket> 									_sockets;
	bool													_exit_server;

	int											_poll_events();
	void 										_add_client(Socket s);
	void										_client_remove(int index);

	class ClientContainer
	{
		public:
			// std::vector<HttpServer> 	_http_servers;

			static std::vector<pollfd> &get_pfds();

			static void add(Socket s);
			static void remove(int index);



		private:
			static std::vector<pollfd> 											_pfds;
			static std::vector<Socket> 											_sockets;
			static std::map<std::reference_wrapper<const Socket>, HttpServer>	_map;


	};

};

std::ostream& operator<< (std::ostream& stream, const Server& rhs);
