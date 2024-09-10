#include <algorithm>
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


int main1()
{
	std::vector<int> vec;

	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);


	for(const auto &i : vec)
	{
		LOG_DEBUG("vec: " << i);
	}

	int &target = vec.at(1);

	auto it = std::find(vec.begin(), vec.end(), target);
	LOG_DEBUG("erasing with : " << target);
	vec.erase(it);


	for(const auto &i : vec)
	{
		LOG_DEBUG("vec: " << i);
	}

	return 0;
}
