#include "Server.hpp"
#include "HttpServer.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include "meta.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <netinet/in.h>
#include <sched.h>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <type_traits>
#include <unistd.h>
#include <vector>

// #define LOG_INFO(x) do { } while(0);
// #define LOG_DEBUG(x) do { } while(0);

Server::Server(uint16_t port) : Server(std::vector<uint16_t>({port}))
{

}

Server::Server(std::vector<uint16_t> ports)
{
	for (uint16_t p : ports)
	{
		_add_client({SocketType::LISTENER, p});
	}
}


void Server::handle_events()
{

	for (size_t i = 0; i < _pfds.size(); i++)
	{
		pollfd &pfd = _pfds[i];
		Socket &s = _sockets[i];
		// LOG("checking fd: " << pfd.fd << " : socket fd : " << _sockets[i].get_fd());

		if (s.is_listener() && ready_to_read(pfd.revents))
		{
			Socket client_sock = s.accept();
			LOG_INFO(s << " new connection " << client_sock);
			_add_client(client_sock);
		}
		else if (s.is_client() && ready_to_read(pfd.revents))
		{
			// LOG_INFO(s << " POLLIN | fd : " << pfd.fd);

			std::string data = s.read();
			if (data.empty())
				continue;	
			try
			{
				_server_instances.at(std::cref(s)).handle(data);
			}
			catch (const std::out_of_range &e)
			{
				LOG_ERROR(s << "_server_instances.handle out of range");
			}
		}
		else if (s.is_client() && ready_to_write(pfd.revents))
		{
			// LOG_INFO(s << " POLLOUT | fd : " << pfd.fd);
			// TODO check if client's httpserver instance is ready to write;
			// NOTE we can maybe do the wait pid thing here?
			try
			{
				HttpServer &instance = _server_instances.at(std::cref(s));
				if (instance.is_ready())
				{
					std::string data = instance.get_data();
					s.write(data);
					_client_remove(s);
				}
			}
			catch (const std::out_of_range &e)
			{
				LOG_ERROR(s << "_server_instances.get_data out of range");
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

	// iterate over all `_server_instances` and waitpid their CGI.
	for (const Socket &s : _sockets)
	{
		if (s.is_client())
		{
			try
			{
				HttpServer &instance = _server_instances.at(std::cref(s));
				instance.poll_cgi();
				if (instance.is_ready())
				{
					LOG_INFO(s << " CGI is ready!");
				}
			}
			catch (const std::out_of_range &e)
			{
				static int counter = 0;
				LOG_ERROR(s << "_server_instances.is_ready out of range");
				if (counter > 5)
					exit(123);
				counter++;


			}
		}
	}

	return n_ready;
}




const std::vector<Socket>& Server::get_sockets() const
{
	return _sockets;
}



int Server::_poll_events()
{
	int n_ready;


	n_ready = ::poll(Server::_get_pfds().data(), Server::_get_pfds().size(), POLL_TIMEOUT);
	if (n_ready == -1)
	{
		LOG_ERROR("Failed polling: " << strerror(errno));
	}
	else if (n_ready)
	{
		static int n_ready_old = std::numeric_limits<int>::max();
		if (n_ready != n_ready_old)
		{
			n_ready_old = n_ready;
			LOG_INFO("[POLL] n of events set: " << n_ready);
		}
	}
	return n_ready;
}



template<typename T>
static void map_list_keys(T &map)
{
	for (const auto &it : map)
	{
		LOG_DEBUG("key[" << it.first.get() << "]");
	}
}

void Server::_add_client(Socket s)
{
	short mask = POLLIN;

	// we copy `s` into _sockets where is will reside until we call `_client_remove`.
	// This means that all other refences to still `s` will become invalid the moment this function returns.
	_sockets.push_back(s);

	Socket &r_s = _sockets.back();


	if (s.is_client())
	{
		// OOF
		_server_instances.insert(SocketRef_HttpServer_map::value_type(std::cref(r_s), HttpServer(r_s)));
		mask = POLLIN | POLLOUT;
	}

	_pfds.push_back({r_s.get_fd(), mask, 0});
	LOG_DEBUG("Added socket[" << s << "], total sockets: " << _sockets.size() - 1);
	map_list_keys(_server_instances);
}

void Server::_client_remove(Socket &s)
{

	LOG_DEBUG("Removing socket[" << s << "], total sockets: " << _sockets.size() - 1);

	if (!s.is_client())
	{
		LOG_ERROR("Trying to remove " << s << " which is not a client");
		return;
	}

	for (size_t  i = 0; i < _pfds.size(); i++)
	{
		if (_pfds.at(i).fd == s.get_fd())	
		{
			close(_pfds[i].fd);
			_pfds.erase(_pfds.begin() + i);
		}
	}
	
	for (size_t  i = 0; i < _sockets.size(); i++)
	{
		if (_sockets.at(i) == s)	
		{
			_sockets.erase(_sockets.begin() + i);
		}
	}
		_server_instances.erase(s);



	LOG_DEBUG("remaining sockets in map");
	map_list_keys(_server_instances);

	LOG_DEBUG("Removed socket[" << s << "], total sockets: " << _sockets.size() - 1);
}

std::vector<pollfd>& Server::_get_pfds()
{
	return _pfds;
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



// The `_server_instances` uses this func to compare the entries
bool operator<(const std::reference_wrapper<const Socket> a, const std::reference_wrapper<const Socket> b)
{
	return a.get().get_fd() < b.get().get_fd();
}


std::ostream& operator<< (std::ostream& os, const Server& rhs)
{
	bool first = true;

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
