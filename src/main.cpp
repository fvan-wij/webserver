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


	bool printReady = true;

	while (1)
	{
		for(auto &s : servers)
		{
			int nReady = poll(s.getFds().data(), s.getFds().size(), POLL_TIMEOUT);
			if (nReady == -1)
			{
				LOG_ERROR("Failed polling: " << strerror(errno));
				return 0;
			}
			else if (printReady && !nReady)
			{
				LOG("nReady: " << nReady);
				printReady = false;
			}
			else if (nReady)
			{
				LOG("nReady: " << nReady);
				printReady = true;
				s.handle_events();
			}
		}
	}
	return 0;
}
