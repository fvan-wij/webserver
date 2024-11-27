#pragma once

#include <cstdint>
#include <vector>
#include <sys/poll.h>
#include <Action.hpp>
#include <Config.hpp>


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

class HttpListener;

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

		void	add(int fd, short events, ActionBase* action);
		void 	remove(int fd);

		void	add_listeners(std::vector<Config> &configs);
		void	add_listener(Config config, uint16_t port);


		std::vector<pollfd>&		get_pfds();
		void 						handle_pfd_events(char *envp[]);

	private:
		std::vector<pollfd>	_pfds;
		std::unordered_map<int, ActionBase *>	_actions;
};
