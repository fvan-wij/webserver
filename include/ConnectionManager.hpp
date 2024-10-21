#pragma once

#include <cstdint>
#include <vector>
#include <sys/poll.h>
#include "Socket.hpp"
#include "HttpProtocol.hpp"
#include <utility>
#include <memory>
#include <ConnectionInfo.hpp>

/**
 * @brief Enum class to differentiate between different fd types.
 * Types include: LISTENER, CLIENT, PIPE
 *
 */
enum class FdType
{
	LISTENER,
	CLIENT,
	PIPE,
};

/**
 * @brief The ConnectionManager manages listeners, clients and pipes in the pollfd list.
 * The types of each file descriptor are stored in the fd_types list. This is used to keep track of the types of the pollfd list.
 *
 * The connection_info map is used to keep track of the ConnectionInfo objects of the sockets. The keys in this map are the file descriptors of the sockets.
 *
 * CGI responses are handled by adding a pipe to the pollfd list.
 * This pipe is linked in the connection info map to the ConnectionInfo object of the client socket that send the request.
 *
 * @param _pfds: vector of pollfd objects.
 * @param _fd_types: vector of FdType objects.
 * @param _connection_info: map of file descriptors to ConnectionInfo objects.
 */
class ConnectionManager {
	public:
		ConnectionManager();
		ConnectionManager(const ConnectionManager &) = default;
		ConnectionManager &operator=(const ConnectionManager &) = default;
		~ConnectionManager() = default;

		void	add(t_config config, Socket);
		void 	remove(size_t index);

		void	add_listeners(std::vector<t_config> &configs);
		void	add_listener(t_config config, uint16_t port);
		void	add_client(ConnectionInfo &ci);
		void	add_pipe(int client_fd, int read_pipe);

		std::vector<pollfd>			get_pfds();
		std::vector<FdType>			get_fd_types();
		std::unordered_map<int, std::shared_ptr<ConnectionInfo>> get_connection_info();

	private:
		std::vector<pollfd>	_pfds;
		std::vector<FdType>	_fd_types;
		std::unordered_map<int, std::shared_ptr<ConnectionInfo>> _connection_info;
};
