// this class will be accessable by both the `VirtualServer` and the main poll loop.
// #include "Server.hpp"
#include "Socket.hpp"
#include "ConnectionManager.hpp"

ConnectionManager::ConnectionManager()
{

}

void ConnectionManager::add_listeners(std::vector<t_config> &configs)
{
	for (const auto& config : configs)
	{
		for (const auto& [name, port] : config.listen)
		{
			LOG_NOTICE("Adding listener socket for " << name << " on port: " << port);
			this->add(config, {SocketType::LISTENER, port});
		}
	}
}

// adds a client to `_pfd`s list
/**
 * @brief sock_ref is there to make sure
 */
void ConnectionManager::add(t_config config, Socket socket)
{
	short mask = POLLIN;

	if (socket.is_client())
	{
		mask = POLLIN | POLLOUT;
	}
	_protocol_map[socket.get_fd()] = new HttpProtocol(config);
	_pfds.push_back({socket.get_fd(), mask, 0});
	_sockets.push_back(socket);
	// _configs.push_back(config);
}

void ConnectionManager::remove(size_t index)
{
	const int fd = _pfds[index].fd;
	if (_sockets[index].is_client())
	{
		auto protocol_pair = _protocol_map.find(fd);
		delete protocol_pair->second;
		_protocol_map.erase(protocol_pair);
	}
	close(fd);
	_pfds.erase(_pfds.begin() + index);
	_sockets.erase(_sockets.begin() + index);
	LOG_DEBUG("Removed socket[" << fd << "], total sockets: " << _sockets.size());

}

std::vector<pollfd>	ConnectionManager::get_pfds()
{
	return (_pfds);
}

std::vector<Socket>	ConnectionManager::get_sockets()
{
	return (_sockets);
}

std::unordered_map<int, HttpProtocol *> ConnectionManager::get_handlers()
{
	return (_protocol_map);
}
