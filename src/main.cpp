#include "Server.hpp"
#include <cstring>
#include "ConfigParser.hpp"

int main(int argc, char **argv)
{
	std::vector<Server> servers;
	std::vector<t_config>	configs;

	// parse config
	if (argc > 1)
		configs = parse_config(argv[1]);
	// for each server_config in config.serverblock
	// servers.push_back(server(serverrconfig))

	servers.push_back({{8080, 8081}});
	servers.push_back({{9090, 9091}});

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
