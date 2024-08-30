#include "Server.hpp"
#include "meta.hpp"
#include <iostream>
#include <cstring>


int main()
{
	std::vector<Server> servers;

	// parse config
	// for each server_config in config.serverblock
	// servers.push_back(server(serverrconfig))

	servers.push_back({{8080, 8081}});
	servers.push_back({{9090, 9091}});

	while (1)
	{
		for(auto &s : servers)
		{
			if (s.poll_events() != -1)
				s.handle_events();
		}
	}
	return 0;
}
