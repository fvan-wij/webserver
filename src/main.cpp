#include "ConnectionManager.hpp"
#include "Logger.hpp"
// #include "Server.hpp"
#include "HttpRequest.hpp"
#include "HandlerFactory.hpp"
#include "ConfigParser.hpp"

#include <cstring>
#include <optional>
#include <sys/poll.h>
#include <vector>

// #define USE_TEST_MAIN

#ifndef USE_TEST_MAIN

// std::optional<std::vector<Server>> create_servers(std::vector<t_config> &configs) {
//     if (!configs.empty()) {
//         LOG_NOTICE("Creating server(s):");
//         std::vector<Server> servers;
//         for (auto &config : configs) {
//             servers.emplace_back(config);
//         }
//         return servers;
//     } else {
//         LOG_ERROR("Could not create server(s) from the given config file. Did you supply a config file?");
//         return std::nullopt;
//     }
// }

// int	run_servers(std::vector<Server> &servers)
// {
// 	bool should_exit = false;

// 	while (!should_exit)
// 	{
// 		for(Server &s : servers)
// 		{
// 			if (s.should_exit())
// 			{
// 				should_exit = true;
// 				return 0;
// 			}
// 			else if (s.poll() > 0)
// 			{
// 				s.handle_events();
// 			}
// 		}
// 	}
// 	return 1;
// }

void loop(ConnectionManager &cm)
{
	while (1)
	{
		std::vector<pollfd> pfds = cm.get_pfds();
		std::vector<Socket> sockets = cm.get_sockets();
		std::unordered_map<int, HttpProtocol *> protocol_handlers = cm.get_handlers();
		int n_ready = ::poll(pfds.data(), pfds.size(), POLL_TIMEOUT);
		if (n_ready > 0)
		{
			for (size_t i = 0; i < pfds.size(); i++)
			{
				pollfd &pfd = pfds[i];
				Socket &socket = sockets[i];

				if (socket.is_listener() && pfd.revents & POLLIN)
				{
					Socket client_socket = socket.accept();
					cm.add(protocol_handlers[pfd.fd]->get_config(), client_socket);
				}
				else if (socket.is_client() && pfd.revents & POLLIN)
				{
					LOG_INFO("fd: " << pfd.fd << " POLLIN");

					std::optional<std::vector<char>> read_data = socket.read();
					if (read_data)
					{
						auto protocol = protocol_handlers[pfd.fd];
						std::vector<char> data = read_data.value();
						protocol->handle(data);
					}
					else
						cm.remove(i);
				}
				else if (socket.is_client() && pfd.revents & POLLOUT)
				{
					// LOG("fd: " << pfd.fd << " POLLOUT");
					auto protocol = cm.get_handlers()[pfd.fd];
					if (protocol->response.is_ready())
					{
						std::string data = protocol->get_data();
						LOG_INFO("Sending response: \n" << GREEN << protocol->response.to_string() << END);
						socket.write(data);
						cm.remove(i);
					}
				}
				else if (pfd.revents & POLLERR || pfd.revents & POLLNVAL)
				{
					LOG_ERROR("POLLERR | POLLNVAL error occurred: " << strerror(errno));
				}
				// map current fd  to `VirtualServer`
				// call VirtualServer.handle(pfd)
			}
		}
	}
}

int main(int argc, char *argv[])
{
	std::vector<t_config>	configs;
	ConnectionManager		cm;

	if (argc == 2 && argv[1])
	{
		configs = parse_config(argv[1]);
	}
	else
	{
		LOG_ERROR("Config is invalid or not present, using DEFAULT_CONFIG");
		configs.push_back(DEFAULT_CONFIG);
		// return -1;
	}

	cm.add_listeners(configs);
	loop(cm);


	// if (auto initialized_servers = create_servers(configs))
	// {
	// 	return (run_servers(*initialized_servers));
	// }
	// else
	// {
	// 	LOG_ERROR("Error initializing server(s)");
	// 	return -1;
	// }

	// init severs
}

#endif
