#include "Server.hpp"
#include "HttpServer.hpp"
#include "Logger.hpp"
#include "meta.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sched.h>
#include <string>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <vector>

Server::Server(uint16_t port) : Server(std::vector<uint16_t>({port}))
{

}

Server::Server(std::vector<uint16_t> ports) : _exit_server(false)
{
	for (uint16_t p : ports)
	{
		_add_client({SocketType::LISTENER, p});
	}
}

Server::Server(const Server &other)
{
       this->_sockets = other._sockets;
       this->_pfds = other._pfds;
       this->_exit_server = other._exit_server;
}

void Server::handle_events()
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		pollfd &pfd = _pfds[i];
		Socket &s = _sockets[i].first;
		auto server = _sockets[i].second;
		// LOG("checking fd: " << pfd.fd << " : socket fd : " << _sockets[i].get_fd());

		if (s.is_listener() && ready_to_read(pfd.revents))
		{
			Socket client_sock = s.accept();
			_add_client(client_sock);
		}
		else if (s.is_client() && ready_to_read(pfd.revents))
		{
			// LOG("fd: " << pfd.fd << " POLLIN");

			std::string data = s.read();
			if (data.empty())
			{
				LOG_DEBUG("received empty data, removing client " << s);
				_client_remove(i);
			}
			else
			{
				LOG_INFO(s << " received data");
				// LOG_INFO(s << " received:\n ["  << data << "]");
				HttpRequest request = HttpRequest();
				request.parse(data);
				server->handle(request);

			}

		}
		else if (s.is_client() && ready_to_write(pfd.revents))
		{
			// LOG("fd: " << pfd.fd << " POLLOUT");
			if (server->response.is_ready())
			{
				std::string data = server->get_data();
				// LOG_INFO(s << " Sending response: \n"  << http_server->response.to_string());
				LOG_INFO(s << " Sending response");
				s.write(data);
				_client_remove(i);
			}
		}
		else if (error_occurred(pfd.revents))
		{
			LOG_ERROR("POLLERR | POLLNVAL error occurred: " << strerror(errno));
		}
	}
}

int Server::poll()
{
	int n_ready = _poll_events();

	for (const auto &p : _sockets)
	{
		if (p.first.is_client())
		{
			auto http_server = p.second;
			http_server->poll_cgi();
			// -----------------------
			if (http_server->is_ready())
			{
				LOG_DEBUG("http_server of " << p.first << " is ready");
			}
			// -----------------------
		}
	}

	return n_ready;
}



std::vector<pollfd>& Server::get_pfds()
{
	return _pfds;
}

const std::vector<Socket> Server::get_sockets() const
{
	std::vector<Socket> socks;
	for(const auto &p : _sockets)
	{
		socks.push_back(p.first);
	}
	return socks;
}

int Server::_poll_events()
{
	int n_ready;

	n_ready = ::poll(Server::get_pfds().data(), Server::get_pfds().size(), POLL_TIMEOUT);
	if (n_ready == -1)
	{
		LOG_ERROR("Failed polling: " << strerror(errno));
	}
	return n_ready;
}

void Server::_add_client(Socket s)
{
	short mask = POLLIN;

	// we copy `s` into _sockets where is will reside until we call `_client_remove`.
	// This means that all other refences to still `s` will become invalid the moment this function returns.
	_sockets.push_back(std::make_pair(s, std::make_shared<HttpServer>(s)));

	Socket &r_s = _sockets.back().first;


	if (s.is_client())
	{
		mask = POLLIN | POLLOUT;
	}

	_pfds.push_back({r_s.get_fd(), mask, 0});
	LOG_DEBUG(r_s << " added, total sockets: " << _sockets.size());
}

void Server::_client_remove(int index)
{
	const int fd = _pfds[index].fd;


	close(fd);
	_pfds.erase(_pfds.begin() + index);
	const Socket tmp = _sockets[index].first;
	_sockets.erase(_sockets.begin() + index);
	LOG_DEBUG(tmp << " Removed, total sockets: " << _sockets.size());
}

bool Server::error_occurred(short revents)
{
	return revents & POLLERR || revents & POLLNVAL;	
}

bool Server::ready_to_read(short revents)
{
	return revents & POLLIN;
}

bool Server::ready_to_write(short revents)
{
	return revents & POLLOUT;
}


std::ostream& operator<< (std::ostream& os, const Server& rhs)
{
	bool first = true;


	// TODO Print server name aswell
	os << "{";
	for(const Socket &s : rhs.get_sockets())
	{
		if (!first) os << "|";
		os << s;
		first = false;
	}
	os << "}";
	return os;
}
