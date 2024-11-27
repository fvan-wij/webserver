// this class will be accessible by both the `VirtualServer` and the main poll loop.
// #include "Server.hpp"
#include "Socket.hpp"
#include "ConnectionManager.hpp"
#include <HttpListener.hpp>
#include <FileHandler.hpp>

ConnectionManager::ConnectionManager()
{

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
void ConnectionManager::add_listener(Config config, uint16_t port)
{
	HttpListener *listener = new HttpListener(port, *this);
	listener->add_config(config);
	LOG_NOTICE("Adding listener socket for " << config.server_name[0] << " on port: " << port);
}

/**
 * @brief Add a client socket to the pollfd list and add a type to the fd_types list.
 * Also creates a new ConnectionInfo object and adds it to the _connection_info map.
 * A protocol is assigned to each new file descriptor. This protocol is determined from the config. (Currently only HttpProtocol)
 * The protocol handles the response format of the server.
 *
 * @param config The config of the server connection.
 * @param socket The socket to add.
 */
// void ConnectionManager::add_client(ConnectionInfo &ci)
// {
// 	short mask = POLLIN | POLLOUT;
// 	Config config = ci.get_config();
// 	Socket socket = ci.get_socket().accept();
// 	LOG_INFO("Client (fd " << socket.get_fd() << ") connected to: " << config.server_name[0] << " on port: " << socket.get_port());

// 	ConnectionInfo *new_ci = new ConnectionInfo(socket, new HttpProtocol(config), config);
// 	_pfds.push_back({socket.get_fd(), mask, 0});
// 	// _fd_types.push_back(FdType::CLIENT);
// 	_connection_info[socket.get_fd()] = std::shared_ptr<ConnectionInfo>(new_ci);
// }

/**
 * @brief CGI input pipe is added to the pollfd list.
 * This pipe is linked in the connection info map to the ConnectionInfo object of the client socket.
 * This way we can easily find the client socket that needs the response from the CGI.
 *
 * @param read_pipe
 */
void ConnectionManager::add_pipe(int client_fd, int read_pipe)
{
	short mask = POLLIN;
	_pfds.push_back({read_pipe, mask, 0});
	// _fd_types.push_back(FdType::PIPE);
	_connection_info[read_pipe] = _connection_info[client_fd];
	LOG_DEBUG("Adding pipe for client cgi request. client_fd: " << client_fd << ", pipe_fd: " << read_pipe);
}

/**
 * @brief using an index remove the file descriptor (socket, pollfd, pipe) and connectionInfo.
//  */
// void ConnectionManager::remove(size_t index)
// {
// 	int fd = _pfds[index].fd;
// 	LOG_INFO("Client (fd " << fd << ") disconnected from " << _connection_info[fd]->get_config().server_name[0] << " on port: " << _connection_info[fd]->get_socket().get_port());
// 	close(fd);
// 	_pfds.erase(_pfds.begin() + index);
// 	// // _fd_types.erase(_fd_types.begin() + index);
// 	_connection_info.erase(fd);
// }

void ConnectionManager::remove(int fd)
{
	for (size_t i = 0; i < _pfds.size(); i++)
	{
		if (fd == _pfds[i].fd)
			_pfds.erase(_pfds.begin() + i);
	}
	close(fd);
	_actions.erase(fd);
	LOG_INFO("Client (fd " << fd << ") disconnected");
}

/**
 * @brief using an client_fd to remove the pipe file descriptor (socket, pollfd, pipe) and connectionInfo.
 */
void ConnectionManager::remove_pipe(int client_fd)
{
	for (size_t i = 0; i < _pfds.size(); i++)
	{
		if (_pfds[i].fd == _connection_info[client_fd]->get_protocol()->get_pipe_fd())
		{
			int fd = _pfds[i].fd;
			LOG_DEBUG("Removing pipe_fd: " << fd);
			_pfds.erase(_pfds.begin() + i);
			// // _fd_types.erase(_fd_types.begin() + i);
			_connection_info.erase(fd);
		}
	}
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
 * @brief
 * @return vector of File descriptor types.
 */
// std::vector<FdType>&	ConnectionManager::get_fd_types()
// {
// 	return (_fd_types);
// }

/**
 * @brief
 * @return unordered connectionInfo map.
 */
std::unordered_map<int, std::shared_ptr<ConnectionInfo>> ConnectionManager::get_connection_info()
{
	return (_connection_info);
}

/**
 * @brief Loop over the pollfd list and handle the events.
 *
 * @param envp
 */
void ConnectionManager::handle_pfd_events(char *envp[])
{
	(void) envp;

	std::vector<pollfd> &pfds = get_pfds();
	for (size_t i = 0; i < pfds.size(); i++)
	{
		if (pfds[i].revents)
		{
			auto action = _actions[pfds[i].fd];
			try
			{
				action->execute(pfds[i].revents);
			} 
			catch (FileHandler::FileHandlerException &e)
			{
				LOG_ERROR(e.what());
				// Should disconnect
			}
		}
	}
}
