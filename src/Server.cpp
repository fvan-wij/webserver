#include "Server.hpp"
#include "meta.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

static bool echo(int fd)
{
	const size_t buf_size = 1024;
	char buffer[buf_size];
	const std::string bye_str = "Cya!\n";
	bzero(&buffer, sizeof(buffer));
	// read up on EWOULDBLOCK
	recv(fd, buffer, buf_size, 0);
	LOG("Received: [" << buffer << "]");


	std::string s = 
	"HTTP/1.1 200 OK\r\n"
	"\r\n<h1> Fakka strijders </h1>\r\n";

	if (!s[0])
	{
		return false;
	}
	send(fd, s.c_str(), s.length(), 0);


	// NOTE according to the http docs we can use the same connection for multiple requests.
	// Therefore we dont HAVE to close the connection after sending this response.
	// However in our current setup (in which we will send only 1 response this will work)
	// https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview

	return false;
}



Server::Server(uint16_t port) : Server(std::vector<uint16_t>({port}))
{

}

Server::Server(std::vector<uint16_t> ports)
{
	for (uint16_t p : ports)
	{
		_add_connection({SocketType::LISTENER, p});
		
	}
}

void Server::_add_connection(Socket s)
{
	uint8_t mask = POLLIN;

	if (s.is_client())
		mask = POLLIN | POLLOUT;

	_sockets.push_back(s);
	_pfds.push_back({s.get_fd(), mask, 0});

}

void Server::handle_events()
{
	// loop over connections
	for (size_t i = 0; i < _pfds.size(); i++)
	{
		pollfd &pfd = _pfds[i];
		LOG("checking fd: " << pfd.fd << " : socket fd : " << _sockets[i].get_fd());

		if (ready_to_read(pfd.revents) && _sockets[i].is_listener())
		{
			Socket client_sock = _sockets[i].accept();
			_add_connection(client_sock);
		}
		else if (ready_to_read(pfd.revents))
		{
			LOG("fd: " << pfd.fd << " POLLIN");
			if (!echo(pfd.fd))
			{
				_close_connection(i);
			}
		}
		else if (ready_to_write(pfd.revents))
		{
			LOG("fd: " << pfd.fd << " POLLOUT");
		}
		else if (error_occurred(pfd.revents))
		{
			LOG_ERROR("POLLERR | POLLNVAL error occurred: " << strerror(errno));
		}
	}
}

int Server::poll_events()
{
	int n_ready;
	bool print_ready = true;

	n_ready = poll(Server::get_pfds().data(), Server::get_pfds().size(), POLL_TIMEOUT);
	if (n_ready == -1)
	{
		LOG_ERROR("Failed polling: " << strerror(errno));
		return -1;
	}
	else if (print_ready && !n_ready)
	{
		LOG("Polling... n of events set: " << n_ready);
		print_ready = false;
	}
	else if (n_ready)
	{
		LOG("Polling... n of events set: " << n_ready);
		print_ready = true;
		return 1;
	}
	return -1;
}

std::vector<pollfd>& Server::get_pfds()
{
	return _pfds;
}

std::vector<Socket>& Server::get_sockets()
{
	return _sockets;
}

int Server::_socket_create()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd == -1)
	{
		LOG_ERROR(strerror(errno));
		return fd;
	}
	else
		LOG("Created socket with fd: " << fd);

	int enable = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
	{
		LOG_ERROR(strerror(errno));
		close(fd);
	}
	return fd;
}

// int Server::_socket_accept(int fd)
// {
// 	int clientFd = accept(fd, nullptr, nullptr);
// 	if (clientFd == -1)
// 	{
// 		LOG_ERROR("Failed accepting client on fd: " << fd << ", " << strerror(errno));
// 		close(fd);
// 		return -1;
// 	}
// 	else
// 	{
// 		LOG("Accepted new client on listening socket fd: " << fd << " with clientFd " << clientFd);
// 	}
// 	Socket client_socket = Socket(CLIENT, clientFd);
// 	_sockets.push_back(client_socket);
// 	return clientFd;
// }

void Server::_close_connection(int index)
{
	close(_pfds[index].fd);
	_pfds.erase(_pfds.begin() + index);
	_sockets.erase(_sockets.begin() + index);
	LOG(" Disconnected socket[" << index << "], total sockets: " << _sockets.size());
}

bool						Server::error_occurred(short revents)
{
	return revents & POLLERR || revents & POLLNVAL;	
}

bool						Server::ready_to_read(short revents)
{
	return revents & POLLIN;
}

bool						Server::ready_to_write(short revents)
{
	return revents & POLLOUT;
}

