#include <cstdlib>
#include <vector>
#include "Logger.hpp"
#include "Server.hpp"

int main()
{
	std::vector<Server> servers;

	// parse config
	// for each server_config in config.serverblock
	// servers.push_back(server(serverrconfig))

	// servers.push_back({{8080, 8081}});
	servers.push_back({{9090, 9091}});

	for(const Server &s : servers)
		LOG_NOTICE(s << " started.");



	while (1)
	{
		for(auto &s : servers)
		{
			if (s.poll() > 0)
				s.handle_events();
		}
	}
	return 0;
}
