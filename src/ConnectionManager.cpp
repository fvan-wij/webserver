// this class will be accessible by both the `VirtualServer` and the main poll loop.
#include "ConnectionManager.hpp"
#include <HttpListener.hpp>
#include <FileHandler.hpp>
#include <HttpExceptions.hpp>

ConnectionManager::ConnectionManager()
{
	_pfds.reserve(1024);
}

/**
 * @brief Using the config(s) as reference add new listener sockets. (I feel like this needs an update at some point)
 */
void ConnectionManager::add_listeners(std::vector<Config> &configs)
{
	for (const auto& config : configs)
	{
		for (const auto& [name, port] : config.listen)
		{
			this->add_listener(config, port);
		}
	}
}

/**
 * @brief This function adds a file descriptor to the pollfd list and adds an action coupled to that fd.
 *
 * @param fd
 * @param events
 * @param action
 */
void ConnectionManager::add(int fd, short events, ActionBase *action)
{
	_pfds.push_back({fd, events, 0});
	_actions[fd] = action;
}

/**
 * @brief adds a listener socket to the pollfd list and adds a type to the fd_types list.
 *
 * The fd_types list is parallel to the pollfd list. It is used to keep track of the types of the pollfd list.
 *
 * This function creates a new ConnectionInfo object and adds it to the _connection_info map.
 * The keys in this map are the file descriptors of the sockets. So you can use this map to get the ConnectionInfo object of a socket or "connection".
 *
 * A protocol is assigned to each new file descriptor. This protocol is determined from the config. (Currently only HttpProtocol)
 * The protocol handles the response format of the server.
 * This protocol is also passed on to any new connections that are created from this listener socket.
 *
 * @param config
 * @param port
 */
void ConnectionManager::add_listener(Config config, int port)
{
	HttpListener *listener;
	if (_listeners.find(port) != _listeners.end())
	{
		listener = _listeners[port].get();
		LOG_NOTICE("Adding config to socket for " << config.get_server_name(0).value_or("") << " on port: " << port);
	}
	else
	{
		listener = new HttpListener(port, *this);
		_listeners[port] = std::shared_ptr<HttpListener>(listener);
		LOG_NOTICE("Adding listener socket for " << config.get_server_name(0).value_or("") << " on port: " << port);
	}
	listener->add_config(config);
}

void ConnectionManager::remove(int fd)
{
	for (size_t i = 0; i < _pfds.size(); i++)
	{
		if (fd == _pfds[i].fd)
			_pfds.erase(_pfds.begin() + i);
	}
	close(fd);
	ActionBase *act = _actions[fd];
	_actions.erase(fd);
	act->cleanup();
	delete act;
}

/**
 * @brief
 * @return vector of pollfds
 */
std::vector<pollfd>&	ConnectionManager::get_pfds()
{
	return (_pfds);
}

/**
 * @brief Loop over the pollfd list and handle the events.
 *
 * @param envp
 */
void ConnectionManager::handle_pfd_events(char *envp[])
{
	(void) envp;

	for (size_t i = 0; i < _pfds.size(); i++)
	{
		if (_pfds[i].revents)
		{
			const auto& action = _actions[_pfds[i].fd];
			action->execute(_pfds[i].revents);
		}
	}
}
