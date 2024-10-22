// #include "Server.hpp"
// #include "ConnectionManager.hpp"
// #include "HttpProtocol.hpp"
// #include "meta.hpp"
// #include <cstdint>
// #include <cstring>
// #include <functional>
// #include <iostream>
// #include <netinet/in.h>
// #include <sched.h>
// #include <string>
// #include <sys/poll.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <vector>

// Server::Server(uint16_t port) : Server(std::vector<uint16_t>({port}))
// {

// }

// Server::Server(t_config &config, ConnectionManager &cm) : _exit_server(false)
// {
// 	for (const auto& [name, port] : config.listen)
// 	{
// 		LOG_NOTICE("Adding listener socket for " << name << " on port: " << port);
// 		// _add_client({SocketType::LISTENER, port});
// 		cm.add(this, {SocketType::LISTENER, port});
// 	}
// 	_config = config;
// }

// Server::Server(std::vector<uint16_t> ports) : _exit_server(false)
// {
// 	for (uint16_t p : ports)
// 	{
// 		_add_client({SocketType::LISTENER, p});
// 	}
// }

// /*
// 	Handles a poll event.
// 	If the file descriptor the requested event came from is a listening socket, we add a new client.
// 	If it is not a listening socket we know a client sent a request to the webserver.

// 	If it is a read (POLLIN) event and client socket then we know a request came in.
// 	Then the data is read and put into a buffer to be parsed and handled.
// 	The data read is sent in chunks to the httpserver (handler / controller).
// 	Thus it can take a few loops to generate a full response.

// 	If it is a write (POLLOUT) and client socket then we know that we can send the generated response to the client.
// 	For every socket there is a http handler / controller.
// */
// void Server::handle_events()
// {
// 	for (size_t i = 0; i < _pfds.size(); i++)
// 	{
// 		pollfd &pfd = _pfds[i];
// 		Socket &s = _sockets[i];
// 		// LOG("checking fd: " << pfd.fd << " : socket fd : " << _sockets[i].get_fd());

// 		if (s.is_listener() && ready_to_read(pfd.revents))
// 		{
// 			Socket client_sock = s.accept();
// 			_add_client(client_sock);
// 		}
// 		else if (s.is_client() && ready_to_read(pfd.revents))
// 		{
// 			LOG_INFO("fd: " << pfd.fd << " POLLIN");

// 			std::optional<std::vector<char>> read_data = s.read();
// 			if (read_data)
// 			{
// 				auto http_server = _httpserver_map.at(s.get_fd());
// 				std::vector<char> data = read_data.value();
// 				http_server->handle(data);
// 			}
// 			else
// 				_client_remove(i);

// 		}
// 		else if (s.is_client() && ready_to_write(pfd.revents))
// 		{
// 			// LOG("fd: " << pfd.fd << " POLLOUT");
// 			auto http_server = _httpserver_map.at(s.get_fd());
// 			if (http_server->response.is_ready())
// 			{
// 				std::string data = http_server->get_data();
// 				LOG_INFO("Sending response: \n" << GREEN << http_server->response.to_string() << END);
// 				s.write(data);
// 				_client_remove(i);
// 			}
// 		}
// 		else if (error_occurred(pfd.revents))
// 		{
// 			LOG_ERROR("POLLERR | POLLNVAL error occurred: " << strerror(errno));
// 		}
// 	}
// }


// int Server::poll()
// {
// 	int n_ready = _poll_events();

// 	for (const Socket &s : _sockets)
// 	{
// 		if (s.is_client())
// 		{
// 			auto http_server = _httpserver_map.at(s.get_fd());
// 			http_server->poll_cgi();
// 		}
// 	}

// 	return n_ready;
// }

// std::vector<pollfd>& Server::get_pfds()
// {
// 	return _pfds;
// }

// const std::vector<Socket>& Server::get_sockets() const
// {
// 	return _sockets;
// }

// /**
//  * @brief Goes over all pollfd objects whithin the server and returns the amount of pfds that have an event.
//  * @return number of pfds with events. -1 if a poll error occured.
//  */
// int Server::_poll_events()
// {
// 	int n_ready;

// 	n_ready = ::poll(Server::get_pfds().data(), Server::get_pfds().size(), POLL_TIMEOUT);
// 	if (n_ready == -1)
// 	{
// 		LOG_ERROR("Failed polling: " << strerror(errno));
// 	}
// 	return n_ready;
// }

// /**
//  * @brief We add the socket 's' to the list of sockets that we will poll on.
//  *
//  * As 's' is not a reference, we copy it into the _sockets vector. This means that all other copies of 's' will become invalid the moment this function returns.
//  *
//  * The socket does not actually need to be a client socket, it can also be a listener socket. (Maybe we should rename this function)
//  *
//  * We also add the socket to the _httpserver_map if it is a client socket.
//  *
//  * @param s the socket to add.
//  */
// void Server::_add_client(Socket s)
// {
// 	short mask = POLLIN;

// 	_sockets.push_back(s);

// 	Socket &r_s = _sockets.back();

// 	if (s.is_client())
// 	{
// 		_httpserver_map[r_s.get_fd()] = new HttpProtocol(_config);
// 		mask = POLLIN | POLLOUT;
// 	}

// 	_pfds.push_back({r_s.get_fd(), mask, 0});
// }

// /**
//  * @brief Removes the socket at index 'index' from the list of sockets that we poll on.
//  *
//  * This function also closes the file descriptor of the socket.
//  *
//  * @param index the index of the socket to remove.
//  */
// void Server::_client_remove(int index)
// {
// 	const int fd = _pfds[index].fd;

// 	if (_sockets[index].is_client())
// 	{
// 		auto http_server = _httpserver_map.find(_sockets[index].get_fd());
// 		delete http_server->second;
// 		_httpserver_map.erase(http_server);
// 	}

// 	close(fd);
// 	_pfds.erase(_pfds.begin() + index);
// 	_sockets.erase(_sockets.begin() + index);
// 	LOG_DEBUG("Removed socket[" << fd << "], total sockets: " << _sockets.size());
// }

// /**
//  * @brief checks if an error occured in revents.
//  * @return true if a poll error (POLLERR) or invalid poll request (POLLNVAL) happened.
//  */
// bool Server::error_occurred(short revents)
// {
// 	return revents & POLLERR || revents & POLLNVAL;
// }

// /**
//  * @brief Checks if POLLIN is set in revents.
//  * @return true if the POLLIN is set in revents, else false.
//  */
// bool Server::ready_to_read(short revents)
// {
// 	return revents & POLLIN;
// }

// /**
//  * @brief Checks if POLLOUT is set in revents.
//  * @return true if the POLLOUT is set in revents, else false.
//  */
// bool Server::ready_to_write(short revents)
// {
// 	return revents & POLLOUT;
// }

// // The `_server_instances` uses this func to compare the entries
// bool operator<(const std::reference_wrapper<const Socket> a, const std::reference_wrapper<const Socket> b)
// {
// 	return a.get().get_fd() < b.get().get_fd();
// }

// std::ostream& operator<< (std::ostream& os, const Server& rhs)
// {
// 	bool first = true;


// 	// TODO Print server name aswell
// 	os << "{";
// 	for(const Socket &s : rhs.get_sockets())
// 	{
// 		if (!first) os << "|";
// 		os << s;
// 		first = false;
// 	}
// 	os << "}";
// 	return os;
// }
